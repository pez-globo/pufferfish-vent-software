"""Handling of messages."""

import logging
import struct
from typing import Mapping, Optional, Type

import attr

import betterproto

from ventserver.protocols import exceptions
from ventserver.sansio import channels
from ventserver.sansio import protocols


# Classes


@attr.s
class Message:
    """A Message contains a field specifying (de)serialization schema/type."""

    _HEADER_FORMAT = '> B'
    _HEADER_PARSER = struct.Struct(_HEADER_FORMAT)
    HEADER_SIZE = struct.calcsize(_HEADER_FORMAT)
    PAYLOAD_OFFSET = 1

    type: int = attr.ib(default=0)
    payload: betterproto.Message = attr.ib(default=None)

    def parse(
            self, buffer: bytes,
            message_classes: Mapping[int, Type[betterproto.Message]]
    ) -> None:
        """Parse message contents from a buffer.

        Args:
            buffer: The message body bytestring from which header values and the
                payload are to be parsed and stored in the message's own
                attributes.
            message_classes: The mapping to look up Protocol Buffer message
                classes from the message type code.

        Raises:
            exceptions.ProtocolDataError: The header cannot be parsed.

        """
        body_header = buffer[:self.HEADER_SIZE]
        try:
            results = self._HEADER_PARSER.unpack(body_header)
        except struct.error as exc:
            raise exceptions.ProtocolDataError(
                'Unparseable header: {!r}'.format(body_header)
            ) from exc

        (self.type,) = results
        try:
            message_object = message_classes[self.type]()
        except KeyError as exc:
            raise exceptions.ProtocolDataError(
                'Unknown message type code: {}'.format(self.type)
            ) from exc

        body_payload = buffer[self.HEADER_SIZE:]
        try:
            self.payload = message_object.parse(body_payload)
        except Exception as exc:
            # Wrap and re-raise any betterproto error as a ProtocolDataError
            raise exceptions.ProtocolDataError(
                'Unparseable payload: {!r}'.format(body_payload)
            ) from exc

    def update_from_payload(
            self, message_types: Mapping[Type[betterproto.Message], int]
    ) -> None:
        """Update header fields from the payload."""
        message_type = type(self.payload)
        try:
            self.type = message_types[message_type]
        except KeyError as exc:
            raise exceptions.ProtocolDataError(
                'Message type does not have a code: {}'.format(message_type)
            ) from exc

    @property
    def body(self) -> bytes:
        """Return the body of the message, including the header and payload."""
        serialized_payload = bytes(self.payload)
        return self._HEADER_PARSER.pack(self.type) + serialized_payload


# Filters


@attr.s
class MessageReceiver(protocols.Filter[bytes, betterproto.Message]):
    """Message receiver."""

    _logger = logging.getLogger('.'.join((__name__, 'MessageReceiver')))

    message_classes: Mapping[int, Type[betterproto.Message]] = attr.ib(
        factory=dict
    )
    _buffer: channels.DequeChannel[bytes] = attr.ib(
        factory=channels.DequeChannel
    )

    def input(self, event: Optional[bytes]) -> None:
        """Handle input events."""
        self._buffer.input(event)

    def output(self) -> Optional[betterproto.Message]:
        """Generate the next message payload from received message bodies."""
        body = self._buffer.output()
        if body is None:
            return None

        message = Message()
        message.parse(body, self.message_classes)
        self._logger.debug(message)
        return message.payload


@attr.s
class MessageSender(protocols.Filter[betterproto.Message, bytes]):
    """Message sender."""

    _logger = logging.getLogger('.'.join((__name__, 'MessageSender')))

    message_types: Mapping[Type[betterproto.Message], int] = attr.ib(
        factory=dict
    )
    _buffer: channels.DequeChannel[betterproto.Message] = attr.ib(
        factory=channels.DequeChannel
    )

    def input(self, event: Optional[betterproto.Message]) -> None:
        """Handle input events."""
        self._buffer.input(event)

    def output(self) -> Optional[bytes]:
        """Generate the next message body from received payloads."""
        payload = self._buffer.output()
        if payload is None:
            return None

        message = Message()
        message.payload = payload
        message.update_from_payload(self.message_types)
        self._logger.debug(message)
        return message.body
