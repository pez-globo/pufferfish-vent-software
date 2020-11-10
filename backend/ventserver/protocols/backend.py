"""Sans-I/O backend service protocol."""

import collections
import logging
import typing
from typing import Dict, Optional, Type, Union

import attr

import betterproto

from ventserver.protocols import events
from ventserver.protocols import exceptions
from ventserver.protocols import frontend
from ventserver.protocols import mcu
from ventserver.protocols.application import lists, states
from ventserver.protocols.protobuf import frontend_pb, mcu_pb
from ventserver.sansio import channels
from ventserver.sansio import protocols


MCU_SYNCHRONIZER_SCHEDULE = collections.deque([
    states.ScheduleEntry(time=0.05, type=mcu_pb.ParametersRequest),
])

FRONTEND_SYNCHRONIZER_SCHEDULE = collections.deque([
    states.ScheduleEntry(time=0.01, type=mcu_pb.SensorMeasurements),
    states.ScheduleEntry(time=0.01, type=mcu_pb.Parameters),
    states.ScheduleEntry(time=0.01, type=mcu_pb.ParametersRequest),
    states.ScheduleEntry(time=0.01, type=mcu_pb.SensorMeasurements),
    states.ScheduleEntry(time=0.01, type=mcu_pb.AlarmLimits),
    states.ScheduleEntry(time=0.01, type=mcu_pb.AlarmLimitsRequest),
    states.ScheduleEntry(time=0.01, type=mcu_pb.SensorMeasurements),
    states.ScheduleEntry(time=0.01, type=frontend_pb.RotaryEncoder),
    states.ScheduleEntry(time=0.01, type=mcu_pb.CycleMeasurements),
    states.ScheduleEntry(time=0.01, type=mcu_pb.SensorMeasurements),
    states.ScheduleEntry(time=0.01, type=mcu_pb.ActiveLogEvents),
    states.ScheduleEntry(time=0.01, type=mcu_pb.NextLogEvents),
])

FILE_SYNCHRONIZER_SCHEDULE = collections.deque([
    states.ScheduleEntry(time=0.3, type=mcu_pb.SensorMeasurements),
    states.ScheduleEntry(time=0.3, type=mcu_pb.Parameters),
    states.ScheduleEntry(time=0.3, type=mcu_pb.Alarms),
    states.ScheduleEntry(time=0.3, type=mcu_pb.ParametersRequest),
    states.ScheduleEntry(time=0.3, type=mcu_pb.CycleMeasurements),
])

# Events


@attr.s
class ReceiveEvent(events.Event):
    """Backend receive input event."""

    time: Optional[float] = attr.ib(default=None)
    mcu_receive: Optional[mcu.UpperEvent] = attr.ib(default=None)
    frontend_receive: Optional[frontend.UpperEvent] = attr.ib(default=None)
    file_receive: Optional[mcu.UpperEvent] = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return (
            self.time is not None
            or self.mcu_receive is not None
            or self.frontend_receive is not None
        )


@attr.s
class OutputEvent(events.Event):
    """Backend receive output/send output/send input event."""

    mcu_send: Optional[mcu.UpperEvent] = attr.ib(default=None)
    frontend_send: Optional[frontend.UpperEvent] = attr.ib(default=None)
    file_send: Optional[frontend.UpperEvent] = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return self.mcu_send is not None or self.frontend_send is not None


@attr.s
class Announcement(events.Event):
    """Backend send input event."""

    message: bytes = attr.ib(default=b'ping')

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return True


SendEvent = Union[Announcement, OutputEvent]


# Filters


@attr.s
class ReceiveFilter(protocols.Filter[ReceiveEvent, OutputEvent]):
    """Filter which passes input data in an event class."""

    MCU_INPUT_TYPES = {
        mcu_pb.Alarms,
        mcu_pb.SensorMeasurements,
        mcu_pb.CycleMeasurements,
        mcu_pb.Parameters,
    }
    FRONTEND_INPUT_TYPES = {
        mcu_pb.ParametersRequest,
        mcu_pb.Ping,
        mcu_pb.Announcement,
        frontend_pb.RotaryEncoder
    }

    _logger = logging.getLogger('.'.join((__name__, 'ReceiveFilter')))

    _buffer: channels.DequeChannel[ReceiveEvent] = attr.ib(
        factory=channels.DequeChannel
    )
    current_time: float = attr.ib(default=0)
    all_states: Dict[
        Type[betterproto.Message], Optional[betterproto.Message]
    ] = attr.ib()

    _mcu_state_synchronizer: states.Synchronizer = attr.ib()
    _frontend_state_synchronizer: states.Synchronizer = attr.ib()
    _file_state_synchronizer: states.Synchronizer = attr.ib()
    log_events_sender: lists.SendSynchronizer[mcu_pb.LogEvent] = attr.ib()

    @all_states.default
    def init_all_states(self) -> Dict[
            Type[betterproto.Message], Optional[betterproto.Message]
    ]:  # pylint: disable=no-self-use
        """Initialize the synchronizable states.

        Each pair consists of the type class to specify the states, and an
        actual object to store the state values.
        """
        return {
            type: None for type in frontend.MESSAGE_CLASSES.values()
            # FRONTEND_MESSAGE_CLASSES is a superset of MCU_MESSAGE_CLASSES
        }

    @_mcu_state_synchronizer.default
    def init_mcu_synchronizer(self) -> states.Synchronizer:  # pylint: disable=no-self-use
        """Initialize the mcu state synchronizer."""
        return states.Synchronizer(
            message_classes=mcu.MESSAGE_CLASSES,
            all_states=self.all_states,
            output_schedule=MCU_SYNCHRONIZER_SCHEDULE
        )

    @_frontend_state_synchronizer.default
    def init_frontend_synchronizer(self) -> states.Synchronizer:
        """Initialize the frontend state synchronizer."""
        return states.Synchronizer(
            message_classes=frontend.MESSAGE_CLASSES,
            all_states=self.all_states,
            output_schedule=FRONTEND_SYNCHRONIZER_SCHEDULE
        )

    @_file_state_synchronizer.default
    def init_file_synchronizer(self) -> states.Synchronizer:  # pylint: disable=no-self-use
        """Initialize the file state synchronizer."""
        return states.Synchronizer(
            message_classes=mcu.MESSAGE_CLASSES,
            all_states=self.all_states,
            output_schedule=FILE_SYNCHRONIZER_SCHEDULE
        )

    @log_events_sender.default
    def init_log_events_list_sender(self) -> lists.SendSynchronizer[
            mcu_pb.LogEvent
    ]:   # pylint: disable=no-self-use
        """Initialize the frontend log events list sender."""
        return lists.SendSynchronizer(segment_type=mcu_pb.NextLogEvents)

    def input(self, event: Optional[ReceiveEvent]) -> None:
        """Handle input events."""
        if event is None or not event.has_data():
            return

        self._buffer.input(event)

    def output(self) -> Optional[OutputEvent]:
        """Emit the next output event."""
        event = self._buffer.output()
        if event is None:
            return None

        # Process input event
        self._update_clock(event)
        self._handle_mcu_inbound_state(event)
        self._handle_file_inbound_state(event)
        self._handle_frontend_inbound_state(event)

        # Maintain internal data connections
        self._handle_log_events_sending()

        # Output any scheduled outbound state update
        mcu_send = None
        try:
            mcu_send = self._mcu_state_synchronizer.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('MCU State Synchronizer:')
        frontend_send = None
        try:
            frontend_send = self._frontend_state_synchronizer.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('Frontend State Synchronizer:')
        file_send = None
        try:
            file_send = self._file_state_synchronizer.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('File State Synchronizer:')
        return OutputEvent(
            mcu_send=mcu_send,
            frontend_send=frontend_send,
            file_send=file_send
        )

    def _update_clock(self, event: ReceiveEvent) -> None:
        """Handle any clock update."""
        if event.time is not None:
            self.current_time = event.time
        self._mcu_state_synchronizer.input(states.UpdateEvent(
            time=self.current_time
        ))
        self._frontend_state_synchronizer.input(states.UpdateEvent(
            time=self.current_time
        ))
        self._file_state_synchronizer.input(states.UpdateEvent(
            time=self.current_time
        ))

    def _handle_mcu_inbound_state(self, event: ReceiveEvent) -> None:
        """Handle any inbound state update from the MCU."""
        if (
                event.mcu_receive is None
                or type(event.mcu_receive) not in self.MCU_INPUT_TYPES
        ):
            return

        try:
            self._mcu_state_synchronizer.input(
                states.UpdateEvent(pb_message=event.mcu_receive)
            )
        except exceptions.ProtocolDataError:
            self._logger.exception(
                'MCU State Synchronizer: %s', event.mcu_receive
            )

        try:
            self._file_state_synchronizer.input(
                states.UpdateEvent(pb_message=event.mcu_receive)
            )
        except exceptions.ProtocolDataError:
            self._logger.exception(
                'File Save State Synchronizer Save: %s', event.mcu_receive
            )

    def _handle_file_inbound_state(self, event: ReceiveEvent) -> None:
        """Handle any inbound state update from the filesystem."""
        if (
                event.file_receive is None
                or type(event.file_receive) not in self.MCU_INPUT_TYPES
        ):
            return

        try:
            self._file_state_synchronizer.input(
                states.UpdateEvent(pb_message=event.file_receive)
            )
        except exceptions.ProtocolDataError:
            self._logger.exception(
                'File State Synchronizer Read: %s', event.file_receive
            )

    def _handle_frontend_inbound_state(self, event: ReceiveEvent) -> None:
        """Handle any inbound state update from the frontend."""
        if (
                event.frontend_receive is None
                or type(event.frontend_receive) not in self.FRONTEND_INPUT_TYPES
        ):
            return

        try:
            self._frontend_state_synchronizer.input(
                states.UpdateEvent(pb_message=event.frontend_receive)
            )
        except exceptions.ProtocolDataError:
            self._logger.exception(
                'Frontend State Synchronizer: %s', event.frontend_receive
            )

    def _handle_log_events_sending(self) -> None:
        """Handle any updates to log events list sending."""
        expected_log_event = typing.cast(
            mcu_pb.ExpectedLogEvent, self.all_states[mcu_pb.ExpectedLogEvent]
        )
        if expected_log_event is not None:
            self.log_events_sender.input(lists.UpdateEvent(
                next_expected=expected_log_event.id
            ))
        next_log_events = self.log_events_sender.output()
        if next_log_events is not None:
            assert isinstance(next_log_events, mcu_pb.NextLogEvents)
            self.all_states[mcu_pb.NextLogEvents] = next_log_events


@attr.s
class SendFilter(protocols.Filter[SendEvent, OutputEvent]):
    """Filter which unwraps output data from an event class."""

    _buffer: channels.DequeChannel[SendEvent] = attr.ib(
        factory=channels.DequeChannel
    )

    def input(self, event: Optional[SendEvent]) -> None:
        """Handle input events."""
        if event is None or not event.has_data():
            return

        self._buffer.input(event)

    def output(self) -> Optional[OutputEvent]:
        """Emit the next output event."""
        event = self._buffer.output()
        if event is None:
            return None

        if isinstance(event, OutputEvent):
            return event

        if isinstance(event, Announcement):
            message = mcu_pb.Announcement(announcement=event.message)
            return OutputEvent(mcu_send=message, frontend_send=message)

        return None


# Protocols


def get_mcu_send(
        backend_output: Optional[OutputEvent]
) -> Optional[mcu.UpperEvent]:
    """Convert a OutputEvent to an MCUUpperEvent."""
    if backend_output is None:
        return None
    if backend_output.mcu_send is None:
        return None

    return backend_output.mcu_send


def get_frontend_send(
        backend_output: Optional[OutputEvent]
) -> Optional[frontend.UpperEvent]:
    """Convert a OutputEvent to an FrontendUpperEvent."""
    if backend_output is None:
        return None
    if backend_output.frontend_send is None:
        return None

    return backend_output.frontend_send


def get_file_send(
        backend_output: Optional[OutputEvent]
) -> Optional[mcu.UpperEvent]:
    """Convert a OutputEvent to an MCUUpperEvent."""
    if backend_output is None:
        return None
    if backend_output.file_send is None:
        return None

    return backend_output.file_send
