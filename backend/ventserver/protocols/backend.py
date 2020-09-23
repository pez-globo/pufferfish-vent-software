"""Sans-I/O backend service protocol."""

import collections
import logging
from typing import Dict, Optional, Type, Union

import attr

import betterproto

from ventserver.protocols import application
from ventserver.protocols import events
from ventserver.protocols import exceptions
from ventserver.protocols import frontend
from ventserver.protocols import mcu
from ventserver.protocols.protobuf import frontend_pb, mcu_pb
from ventserver.sansio import channels
from ventserver.sansio import protocols


MCU_SYNCHRONIZER_SCHEDULE = collections.deque([
    application.ScheduleEntry(time=0.05, type=mcu_pb.ParametersRequest),
])

FRONTEND_SYNCHRONIZER_SCHEDULE = collections.deque([
    application.ScheduleEntry(time=0.01, type=mcu_pb.SensorMeasurements),
    application.ScheduleEntry(time=0.01, type=mcu_pb.Parameters),
    application.ScheduleEntry(time=0.01, type=mcu_pb.Alarms),
    application.ScheduleEntry(time=0.01, type=mcu_pb.SensorMeasurements),
    application.ScheduleEntry(time=0.01, type=frontend_pb.RotaryEncoder),
    application.ScheduleEntry(time=0.01, type=mcu_pb.Alarms),
    application.ScheduleEntry(time=0.01, type=mcu_pb.SensorMeasurements),
    application.ScheduleEntry(time=0.01, type=mcu_pb.ParametersRequest),
    application.ScheduleEntry(time=0.01, type=mcu_pb.CycleMeasurements),
])


# Events


@attr.s
class ReceiveEvent(events.Event):
    """Backend receive input event."""

    time: Optional[float] = attr.ib(default=None)
    mcu_receive: Optional[mcu.UpperEvent] = attr.ib(default=None)
    frontend_receive: Optional[frontend.UpperEvent] = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return (
            self.time is not None
            or self.mcu_receive is not None or self.frontend_receive is not None
        )


@attr.s
class OutputEvent(events.Event):
    """Backend receive output/send output/send input event."""

    mcu_send: Optional[mcu.UpperEvent] = attr.ib(default=None)
    frontend_send: Optional[frontend.UpperEvent] = attr.ib(default=None)

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
    }

    _logger = logging.getLogger('.'.join((__name__, 'ReceiveFilter')))

    _buffer: channels.DequeChannel[ReceiveEvent] = attr.ib(
        factory=channels.DequeChannel
    )
    current_time: float = attr.ib(default=0)
    all_states: Dict[
        Type[betterproto.Message], Optional[betterproto.Message]
    ] = attr.ib()
    _mcu_state_synchronizer: application.StateSynchronizer = attr.ib()
    _frontend_state_synchronizer: application.StateSynchronizer = attr.ib()

    @all_states.default
    def init_all_states(self) -> Dict[
            Type[betterproto.Message], Optional[betterproto.Message]
    ]:  # pylint: disable=no-self-use
        """Initialize the synchronizable states.

        Each pair consists of the type class to specify the states, and an
        actual object to store the state values.
        """
        return {
            type: None for type in application.FRONTEND_MESSAGE_CLASSES.values()
            # FRONTEND_MESSAGE_CLASSES is a superset of MCU_MESSAGE_CLASSES
        }

    @_mcu_state_synchronizer.default
    def init_mcu_synchronizer(self) -> application.StateSynchronizer:  # pylint: disable=no-self-use
        """Initialize the mcu state synchronizer."""
        return application.StateSynchronizer(
            message_classes=application.MCU_MESSAGE_CLASSES,
            all_states=self.all_states,
            output_schedule=MCU_SYNCHRONIZER_SCHEDULE
        )

    @_frontend_state_synchronizer.default
    def init_frontend_synchronizer(self) -> application.StateSynchronizer:
        """Initialize the frontend state synchronizer."""
        return application.StateSynchronizer(
            message_classes=application.FRONTEND_MESSAGE_CLASSES,
            all_states=self.all_states,
            output_schedule=FRONTEND_SYNCHRONIZER_SCHEDULE
        )

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

        # Handle clock update
        if event.time is not None:
            self.current_time = event.time
        self._mcu_state_synchronizer.input(application.StateUpdateEvent(
            time=self.current_time
        ))
        self._frontend_state_synchronizer.input(application.StateUpdateEvent(
            time=self.current_time
        ))

        # Handle inbound state update from MCU
        if (
                event.mcu_receive is not None
                and type(event.mcu_receive) in self.MCU_INPUT_TYPES
        ):
            try:
                self._mcu_state_synchronizer.input(
                    application.StateUpdateEvent(pb_message=event.mcu_receive)
                )
            except exceptions.ProtocolDataError:
                self._logger.exception(
                    'MCU StateSynchronizer: %s', event.mcu_receive
                )

        # Handle inbound state update from frontend
        if (
                event.frontend_receive is not None
                and type(event.frontend_receive) in self.FRONTEND_INPUT_TYPES
        ):
            try:
                self._frontend_state_synchronizer.input(
                    application.StateUpdateEvent(
                        pb_message=event.frontend_receive
                    )
                )
            except exceptions.ProtocolDataError:
                self._logger.exception(
                    'Frontend StateSynchronizer: %s', event.frontend_receive
                )

        # Output any scheduled outbound state update
        mcu_send = None
        try:
            mcu_send = self._mcu_state_synchronizer.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('MCU StateSynchronizer:')
        frontend_send = None
        try:
            frontend_send = self._frontend_state_synchronizer.output()
        except exceptions.ProtocolDataError:
            self._logger.exception('Frontend StateSynchronizer:')

        return OutputEvent(mcu_send=mcu_send, frontend_send=frontend_send)


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
