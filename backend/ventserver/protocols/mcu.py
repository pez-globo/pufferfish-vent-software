"""Sans-I/O MCU device communication protocol."""

import logging
from typing import Mapping, Optional, Type

import attr

import betterproto

from ventserver.protocols import datagrams
from ventserver.protocols import exceptions
from ventserver.protocols import frames
from ventserver.protocols import messages
from ventserver.protocols import crcelements
from ventserver.protocols.protobuf import mcu_pb
from ventserver.sansio import protocols


# Events


LowerEvent = bytes
UpperEvent = betterproto.Message


# Types


MESSAGE_CLASSES: Mapping[int, Type[betterproto.Message]] = {
    1: mcu_pb.Alarms,
    2: mcu_pb.SensorMeasurements,
    3: mcu_pb.CycleMeasurements,
    4: mcu_pb.Parameters,
    5: mcu_pb.ParametersRequest,
    6: mcu_pb.AlarmLimits,
    7: mcu_pb.AlarmLimitsRequest,
    8: mcu_pb.ExpectedLogEvent,
    9: mcu_pb.NextLogEvents,
    10: mcu_pb.ActiveLogEvents,
    254: mcu_pb.Ping,
    255: mcu_pb.Announcement
}

MESSAGE_TYPES: Mapping[Type[betterproto.Message], int] = {
    pb_class: type for (type, pb_class) in MESSAGE_CLASSES.items()
}


# Filters


@attr.s
class ReceiveFilter(protocols.Filter[LowerEvent, UpperEvent]):
    """Filter which passes input data in an event class."""

    _logger = logging.getLogger('.'.join((__name__, 'ReceiveFilter')))

    _splitter: frames.ChunkSplitter = attr.ib(factory=frames.ChunkSplitter)
    _cobs_decoder: frames.COBSDecoder = attr.ib(factory=frames.COBSDecoder)
    _crc_receiver: crcelements.CRCReceiver = attr.ib(
        factory=crcelements.CRCReceiver
    )
    _datagram_receiver: datagrams.DatagramReceiver = attr.ib(
        factory=datagrams.DatagramReceiver
    )
    _message_receiver: messages.MessageReceiver = attr.ib()

    @_message_receiver.default
    def init_message_receiver(self) -> messages.MessageReceiver:  # pylint: disable=no-self-use
        """Initialize the mcu message receiver."""
        return messages.MessageReceiver(message_classes=MESSAGE_CLASSES)

    def input(self, event: Optional[LowerEvent]) -> None:
        """Handle input events."""
        self._splitter.input(event)

    def output(self) -> Optional[UpperEvent]:
        """Emit the next output event."""
        chunk = self._splitter.output()
        if chunk is None:
            return None

        self._cobs_decoder.input(chunk)
        frame_payload = None
        try:
            frame_payload = self._cobs_decoder.output()
        except exceptions.ProtocolDataError:
            if self._datagram_receiver.expected_seq is None:
                self._logger.info(
                    'Discarding incomplete frame because DatagramReceiver has '
                    'not yet received a datagram: %s', chunk
                )
            else:
                self._logger.exception('COBSDecoder: %s', chunk)
        if frame_payload:
            self._crc_receiver.input(frame_payload)
        crc_payload = None
        try:
            crc_payload = self._crc_receiver.output()
        except exceptions.ProtocolDataError as err:
            self._logger.exception('CRCReceiver: %s', err)
        if crc_payload:
            self._datagram_receiver.input(crc_payload)
        datagram_payload = None
        try:
            datagram_payload = self._datagram_receiver.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('DatagramReceiver: %s', frame_payload)

        self._message_receiver.input(datagram_payload)
        message: Optional[betterproto.Message] = None
        try:
            message = self._message_receiver.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('MessageReceiver: %s', datagram_payload)

        return message


@attr.s
class SendFilter(protocols.Filter[UpperEvent, LowerEvent]):
    """Filter which unwraps output data from an event class."""

    _logger = logging.getLogger('.'.join((__name__, 'SendFilter')))

    _message_sender: messages.MessageSender = attr.ib()
    _datagram_sender: datagrams.DatagramSender = attr.ib(
        factory=datagrams.DatagramSender
    )
    _crc_sender: crcelements.CRCSender = attr.ib(factory=crcelements.CRCSender)
    _cobs_encoder: frames.COBSEncoder = attr.ib(factory=frames.COBSEncoder)
    _merger: frames.ChunkMerger = attr.ib(factory=frames.ChunkMerger)

    @_message_sender.default
    def init_message_sender(self) -> messages.MessageSender:  # pylint: disable=no-self-use
        """Initialize the message sender."""
        return messages.MessageSender(message_types=MESSAGE_TYPES)

    def input(self, event: Optional[UpperEvent]) -> None:
        """Handle input events."""
        self._message_sender.input(event)

    def output(self) -> Optional[LowerEvent]:
        """Emit the next output event."""
        message_body = None
        try:
            message_body = self._message_sender.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('MessageSender:')

        self._datagram_sender.input(message_body)
        datagram_body = None
        try:
            datagram_body = self._datagram_sender.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('DatagramSender: %s', message_body)

        if datagram_body:
            self._crc_sender.input(datagram_body)
        crc_body = None
        try:
            crc_body = self._crc_sender.output()
        except exceptions.ProtocolDataError as err:
            self._logger.exception('CRCSender: %s', err)

        if crc_body:
            self._cobs_encoder.input(crc_body)
        cobs_body = None
        try:
            cobs_body = self._cobs_encoder.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('COBSEncoder: %s', datagram_body)

        self._merger.input(cobs_body)
        return self._merger.output()
