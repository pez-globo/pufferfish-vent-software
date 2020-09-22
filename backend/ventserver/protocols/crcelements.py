"""Filter to generate protected bytes using crc function.

Provides Filters which compute and check data integrity fields in headers
prepended to arbitrary data payloads (limited by payload length) to detect and
handle data corruption.

A CRCProtectedData is defined as the data unit of the data integrity layer,
consisting of a payload preceded by a header which contains a 32-bit CRC
checksum field. The CRC checksum fields allow detection of byte corruption in
the header and payload.

Typical usage example:

    sender = CRCSender()
    receiver = CRCReceiver()

    # Send direction
    sender.input(bytes([]))
    body = sender.output()
    print(body)  # bytes([0xf1, 0x61, 0x77, 0xd2, 0x00, 0x00])

    # Send direction exception
    sender.input(bytes([0xff for i in range(300)]))
    try:
        body = receiver.output()
    except ventserver.exceptions.ProtocolDataError:
        print('Invalid payload!')  # 'Invalid payload!'

    # Receive direction
    receiver.input(body)
    payload = receiver.output()
    print(payload)  # bytes([])

    # Receive direction exception
    receiver.input(bytes([]))
    try:
        payload = receiver.output()
    except ventserver.exceptions.ProtocolDataError:
        print('Invalid payload!')  # 'Invalid payload!'

"""

import logging
import struct
from typing import Optional, Any
from crcmod import predefined  # type: ignore

import attr

from ventserver.protocols import exceptions
from ventserver.sansio import channels
from ventserver.sansio import protocols


CRC_FUNC = predefined.mkCrcFun('crc-32c')

def quad_byte_attr(
        _: Any, __: 'attr.Attribute[bytes]', value: bytes
) -> None:
    """Validate the attr input as a byte re.

    Raises:
        ValueError: attr init value cannot be represented as a single byte.

    """
    if len(bytes(value)) != 4:
        raise ValueError(
            'Attr must be a bytes representation of unsigned long: {!r}'
            .format(value)
        )


@attr.s
class CRCElement:
    """CRC element class defines crc properties of and crc
    operations on data.

    Attributes:
        HEADER_SIZE: the number of bytes used by the header. This is also the
            data overhead for encapsulating a payload in a datagram.
        crc: the value of the 32-bit CRC field of the datagram. This is computed
            as the CRC-32C (Castagnoli) of the CRC-protected section of the
            datagram body.
        payload: the contents of the payload section of the datagram. This is
            a bytestring.

    """

    _HEADER_FORMAT = '> L'
    _HEADER_PARSER = struct.Struct(_HEADER_FORMAT)
    HEADER_SIZE = struct.calcsize(_HEADER_FORMAT)

    payload: bytes = attr.ib(default=b'')

    crc: bytes = attr.ib(
        default=b'\x00\x00\x00\x00',
        validator=[quad_byte_attr], repr=(
            lambda value: '0x{:08x}'.format(value)  # pylint: disable=unnecessary-lambda
        )
    )


    def parse(self, body: bytes) -> None:
        """Extracts CRC and payload from incoming message.
        Args:
            body:data for which crc has to be computed

        Raises:
            exceptions.ProtocolDataError: The size of the packet received
            should be between 4 bytes and 256 bytes but found to be n bytes.

        """
        if len(body) > 256 or len(body) < 4:
            raise exceptions.ProtocolDataError(
                'The size of the packet received should be between 4 bytes '
                'and 256 bytes but found to be {0} bytes.'.format(len(body))
            )
        self.crc = body[:self.HEADER_SIZE]
        self.payload = body[self.HEADER_SIZE:]

    def _compute_crc(self) -> bytes:
        """Computes CRC for the payload.

        Raises:
            exceptions.ProtocolDataError: Could not compute crc: TypeError.

            exceptions.ProtocolDataError: Could not pack header
                fields: crc=0xXX.

        """
        try:
            # raises TypeError for invalid input type.
            crc_key = int(CRC_FUNC(self.payload))
        except TypeError as terr:
            raise exceptions.ProtocolDataError(
                'Could not compute crc: {0}'
                .format(terr))
        parsed_key = None
        try:
            parsed_key = self._HEADER_PARSER.pack(crc_key)
        except struct.error as exc:
            raise exceptions.ProtocolDataError(
                'Could not pack header fields: crc=0x{:x}'
                .format(crc_key)
            ) from exc

        return parsed_key

    def check_integrity(self) -> None:
        """Matches computed CRC with that extracted from message.

            Raises:
                exceptions.ProtocolDataError: The specified CRC of the
                datagram\'s protected section, 0xXX, is inconsistent with the
                actual computed CRC of the received protected section, 0xXX.

        """
        computed_crc = self._compute_crc()
        if self.crc != computed_crc:
            raise exceptions.ProtocolDataError(
                'The specified CRC of the datagram\'s protected section, '
                '{!r}, is inconsistent with the actual computed CRC '
                'of the received protected section, {!r}'
                .format(self.crc, computed_crc)
            )

    def get_body(self) -> bytes:
        """Returns the CRC combined with the payload."""
        self.crc = self._compute_crc() # raises ProtocolDataError
        return self.crc + self.payload

# CRC Filters

@attr.s
class CRCSender(protocols.Filter[bytes, bytes]):
    """Generates the crc for incoming bytes data and attaches
    the generated crc key to the message and returns it back."""
    _logger = logging.getLogger(".".join((__name__, "CRCSender")))

    _buffer: channels.DequeChannel[bytes] = attr.ib(
        factory=channels.DequeChannel
    )

    def input(self, event: Optional[bytes]) -> None:
        """Stores input data in internal buffer.

        Args:
            event: An optional datagram body represented as a bytestring. If it
            is None, it will be ignored and the buffer will not be modified.

        Raises:
            IndexError: The input data item can't be stored on the buffer
                because the buffer is full.

        """
        if event:
            self._buffer.input(event)

    def output(self) -> Optional[bytes]:
        """Extracts the next event in the buffer and processes it.

        Returns:
            An optional bytestring of the crc protected data body from the next
            available datagram body in the internal buffer. Returns None if
            there are no bodies available in the internal buffer.

        """
        event = self._buffer.output()
        if event is None:
            return None
        crc_element = CRCElement(payload=event)
        message = crc_element.get_body() # raises ProtocolDataError
        return message

@attr.s
class CRCReceiver(protocols.Filter[bytes, bytes]):
    """Generates the crc for incoming bytes data and attaches
    the generated crc key to the message and returns it back."""

    _logger = logging.getLogger(".".join((__name__, "CRCReceiver")))

    _buffer: channels.DequeChannel[bytes] = attr.ib(
        factory=channels.DequeChannel
    )

    def input(self, event: Optional[bytes]) -> None:
        """Stores input data in internal buffer.

        Args:
            event: An optional cobs decoded body represented as a
                bytestring. If it is None, it will be ignored and
                the buffer will not be modified.

        Raises:
            IndexError: The input data item can't be stored on the
            buffer because the buffer is full.

        """
        if event:
            self._buffer.input(event)

    def output(self) -> Optional[bytes]:
        """Extracts the next event in the buffer and processes it.

        Returns:
            An optional bytestring of the crc protected data payload from the
            next available cobs decoded body in the internal buffer. Returns
            None if there are no bodies available in the internal buffer.

        """
        event = self._buffer.output()
        if event is None:
            return None
        crc_element = CRCElement()
        crc_element.parse(event) # raises ProtocolDataError
        crc_element.check_integrity() # raises ProtocolDataError
        return crc_element.payload
