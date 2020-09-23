"""Sans-I/O frontend device communication protocol."""

import logging
from typing import Optional

import attr

import betterproto

from ventserver.protocols import application
from ventserver.protocols import exceptions
from ventserver.protocols import messages
from ventserver.sansio import channels
from ventserver.sansio import protocols


# Events


LowerEvent = bytes
UpperEvent = betterproto.Message


# Filters


@attr.s
class ReceiveFilter(protocols.Filter[bytes, UpperEvent]):
    """Filter which passes input data in an event class."""

    _logger = logging.getLogger('.'.join((__name__, 'ReceiveFilter')))

    _buffer: channels.DequeChannel[bytes] = attr.ib(
        factory=channels.DequeChannel
    )
    _message_receiver: messages.MessageReceiver = attr.ib()

    @_message_receiver.default
    def init_message_receiver(self) -> messages.MessageReceiver:  # pylint: disable=no-self-use
        """Initialize the frontend message receiver."""
        return messages.MessageReceiver(
            message_classes=application.FRONTEND_MESSAGE_CLASSES
        )

    def input(self, event: Optional[bytes]) -> None:
        """Handle input events."""
        self._buffer.input(event)

    def output(self) -> Optional[UpperEvent]:
        """Emit the next output event."""
        event = self._buffer.output()
        if event is None:
            return None

        self._message_receiver.input(event)
        message: Optional[betterproto.Message] = None
        try:
            message = self._message_receiver.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('MessageReceiver: %s', event)

        return message


@attr.s
class SendFilter(protocols.Filter[UpperEvent, bytes]):
    """Filter which unwraps output data from an event class."""

    _logger = logging.getLogger('.'.join((__name__, 'SendFilter')))

    _message_sender: messages.MessageSender = attr.ib()

    @_message_sender.default
    def init_message_sender(self) -> messages.MessageSender:  # pylint: disable=no-self-use
        """Initialize the message sender."""
        return messages.MessageSender(
            message_types=application.FRONTEND_MESSAGE_TYPES
        )

    def input(self, event: Optional[UpperEvent]) -> None:
        """Handle input events."""
        self._message_sender.input(event)

    def output(self) -> Optional[bytes]:
        """Emit the next output event."""
        message_body = None
        try:
            message_body = self._message_sender.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('MessageSender:')

        return message_body
