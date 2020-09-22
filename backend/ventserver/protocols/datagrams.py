"""Filters for handling data integrity over an unreliable messaging channel.

Provides Filters which check data integrity fields in headers prepended to
arbitrary data payloads (limited by payload length) to detect and handle
data loss.

A Datagram is defined as the data unit of the data integrity layer, consisting
of a payload preceded by a header which contains a 1-byte sequence number field,
and a 1-byte length field. The length fields allow detection of byte loss in the
header and payload. The sequence number field allows detection of loss or
reordering of entire datagrams.

Typical usage example:

    sender = DatagramSender()
    receiver = DatagramReceiver()

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
from typing import Any, Optional

import attr

from ventserver.protocols import exceptions
from ventserver.sansio import channels
from ventserver.sansio import protocols


# Constants


SEQ_NUM_SPACE = 256  # the modulo base for sequence numbers


# Classes


def single_byte_attr(
        _: Any, __: 'attr.Attribute[int]', value: int
) -> None:
    """Validate the attr input as a single byte.

    Raises:
        ValueError: attr init value cannot be represented as a single byte.

    """
    bytes([value])

@attr.s
class Datagram:
    """A data unit containing header fields for data integrity checking.

    A Datagram consists of a header and a payload. The header contains several
    big-endian (network-order) fields for data integrity checking.

    Attributes:
        HEADER_SIZE: the number of bytes used by the header. This is also the
            data overhead for encapsulating a payload in a datagram.
        seq: the value of the 8-bit sequence number field of the datagram. This
            is expected to increment by 1 with every datagram sent by a datagram
            sender, and it rolls over from 255 to 0. It is up to the receiver
            what to do with this field.
        length: the value of the 8-bit length field of the datagram. This is
            expected to match the actual length of the datagram payload.
        payload: the contents of the payload section of the datagram. This is
            a bytestring.

    """

    _HEADER_FORMAT = '> B B'
    _HEADER_PARSER = struct.Struct(_HEADER_FORMAT)
    HEADER_SIZE = struct.calcsize(_HEADER_FORMAT)

    seq: int = attr.ib(
        default=0, validator=[single_byte_attr]
    )
    length: int = attr.ib(
        default=0, validator=[single_byte_attr]
    )
    payload: bytes = attr.ib(default=b'')

    @length.validator
    def _check_length(  # pylint: disable=no-self-use
            self, _: 'attr.Attribute[int]', value: int
    ) -> None:
        """Validate the attr input for delimiter."""
        if value < 0 or value > 255:
            raise ValueError(
                'Value must be one byte: {!r}'.format(value)
            )

    def parse(self, buffer: bytes) -> None:
        """Parse datagram contents from a buffer.

        Args:
            buffer: The datagram body bytestring from which header field values
                and the payload are to be parsed and stored in the datagram's
                own attributes.

        Raises:
            exceptions.ProtocolDataError: The header cannot be parsed.

        """
        try:
            results = self._HEADER_PARSER.unpack(buffer[:self.HEADER_SIZE])
        except struct.error as exc:
            raise exceptions.ProtocolDataError(
                'Unparseable header: {!r}'.format(buffer[:self.HEADER_SIZE])
            ) from exc

        (self.seq, self.length) = results
        self.payload = buffer[self.HEADER_SIZE:]

    def update_from_payload(self) -> None:
        """Update header fields from the payload.

        Raises:
            exceptions.ProtocolDataError: the protected header fields of the
                datagram could not be packed together into the protected section
                of the datagram body.

        """
        self.length = len(self.payload)  # update length before computing crc

    def pack_protected(self) -> bytes:
        """Return the protected section of the datagram body.

        Raises:
            exceptions.ProtocolDataError: the protected header fields of the
                datagram could not be packed together.

        """
        try:
            header_protected = self._HEADER_PARSER.pack(
                self.seq, self.length
            )
        except struct.error as exc:
            raise exceptions.ProtocolDataError(
                'Could not pack protected header fields: seq={}, len={}'
                .format(self.seq, self.length)
            ) from exc

        return header_protected + self.payload

    def compute_body(self) -> bytes:
        """Return the body of the datagram, including the header and payload.

        Raises:
            exceptions.ProtocolDataError: the header fields of the datagram
                could not be packed together into the header section of the
                datagram body.

        """
        try:
            header = self._HEADER_PARSER.pack(self.seq, self.length)
        except struct.error as exc:
            raise exceptions.ProtocolDataError(
                'Could not pack header fields: seq={}, len={}'
                .format(self.seq, self.length)
            ) from exc

        return header + self.payload

# Filters


@attr.s
class DatagramReceiver(protocols.Filter[bytes, bytes]):
    """A Filter to parse and check payloads from datagram bodies.

    This filter buffers input datagram bodies, represented as bytestrings,
    processes them to parse datagram header field values and datagram payloads,
    performs data integrity checks, and returns zero or one payloads with every
    call of the output method.

    Logging Statements:
        DEBUG: Every datagram parsed by the output method.
        INFO: Initialization of the expected sequence number and the datagram
            which was used to initialize it.
        WARNING: The expected sequence number and the datagram for every parsed
            datagram with an unexpected sequence number.

    """

    _logger = logging.getLogger('.'.join((__name__, 'DatagramReceiver')))

    expected_seq: Optional[int] = attr.ib(default=None, init=False)
    _buffer: channels.DequeChannel[bytes] = attr.ib(
        factory=channels.DequeChannel
    )

    def input(self, event: Optional[bytes]) -> None:
        """Store input datagram bodies in the internal buffer.

        Args:
            event: An optional datagram body represented as a bytestring. If it
            is None, it will be ignored and the buffer will not be modified.

        Raises:
            IndexError: The input data item can't be stored on the buffer
                because the buffer is full.

        """
        self._buffer.input(event)

    def output(self) -> Optional[bytes]:
        """Produce the next datagram payload.

        Returns:
            An optional bytestring of the datagram payload from the next
            available datagram body in the internal buffer. Returns None if
            there are no bodies available in the internal buffer.

        Raises:
            exceptions.ProtocolDataError: the header fields of the datagram body
                are inconsistent with the payload, or the header cannot be
                parsed from the datagram body.

        """
        body = self._buffer.output()
        if body is None:
            return None

        datagram = Datagram()
        datagram.parse(body)  # may raise ProtocolDataError
        self._logger.debug(datagram)

        if datagram.length != len(datagram.payload):
            raise exceptions.ProtocolDataError(
                'The specified length of the datagram payload, {}, is '
                'inconsistent with the actual received length, {}'
                .format(datagram.length, len(datagram.payload))
            )
        if self.expected_seq is None:
            self._logger.info('Initialized expected seq num from: %s', datagram)
            self.expected_seq = datagram.seq
        elif self.expected_seq != datagram.seq:
            self._logger.warning(
                'Expected datagram with seq num %d, but received: %s',
                self.expected_seq, datagram
            )
            self.expected_seq = datagram.seq
        self.expected_seq = (self.expected_seq + 1) % SEQ_NUM_SPACE
        return datagram.payload


@attr.s
class DatagramSender(protocols.Filter[bytes, bytes]):
    """A Filter to create datagram bodies from payloads.

    This filter buffers input datagram payloads, represented as bytestrings,
    processes them to compute data integrity information included in the headers
    of datagram bodies, and returns zero or one datagram bodies with every call
    of the output method.

    Logging Statements:
        DEBUG: Every datagram created by the output method.

    """

    _logger = logging.getLogger('.'.join((__name__, 'DatagramSender')))

    _seq = attr.ib(default=0, init=False)
    _buffer: channels.DequeChannel[bytes] = attr.ib(
        factory=channels.DequeChannel
    )

    def input(self, event: Optional[bytes]) -> None:
        """Store input datagram payloads in the internal buffer.

        Args:
            event: An optional datagram payload represented as a bytestring. If
            it is None, it will be ignored and the buffer will not be modified.

        Raises:
            IndexError: The input data item can't be stored on the buffer
                because the buffer is full.

        """
        self._buffer.input(event)

    def output(self) -> Optional[bytes]:
        """Produce the next datagram body.

        Returns:
            An optional bytestring of the datagram body from the next available
            datagram payload in the internal buffer. Returns None if there are
            no payloads available in the internal buffer.

        Raises:
            exceptions.ProtocolDataError: The payload is too long to fit in a
                datagram; specifically, it is longer than can be represented by
                the length field of the datagram header.

        """
        payload = self._buffer.output()
        if payload is None:
            return None

        if len(payload) > 255:
            raise exceptions.ProtocolDataError(
                'Datagram payload may not be {} bytes long!'
                .format(len(payload))
            )

        datagram = Datagram(seq=self._seq, payload=payload)
        datagram.update_from_payload()
        self._logger.debug(datagram)
        self._seq = (self._seq + 1) % SEQ_NUM_SPACE
        return datagram.compute_body()
