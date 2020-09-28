"""Application-specific logic."""

import collections
import logging
from typing import Deque, Dict, Mapping, Optional, Type

import attr

import betterproto

from ventserver.protocols import events
from ventserver.protocols import exceptions
from ventserver.sansio import protocols


@attr.s
class UpdateEvent(events.Event):
    """State update event."""

    time: Optional[float] = attr.ib(default=None)
    pb_message: Optional[betterproto.Message] = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return self.time is not None or self.pb_message is not None


@attr.s
class ScheduleEntry:
    """Output schedule entry."""

    time: float = attr.ib()
    type: Type[betterproto.Message] = attr.ib()


@attr.s
class Synchronizer(
        protocols.Filter[UpdateEvent, betterproto.Message]
):
    """State synchronization filter.

    Inputs are clock updates or state updatess received from the peer. Outputs
    are state updates for the peer.
    """

    _logger = logging.getLogger('.'.join((__name__, 'Synchronizer')))

    message_classes: Mapping[int, Type[betterproto.Message]] = attr.ib()
    current_time: float = attr.ib(default=0)
    all_states: Dict[
        Type[betterproto.Message], Optional[betterproto.Message]
    ] = attr.ib()
    output_schedule: Deque[ScheduleEntry] = attr.ib()
    output_deadline: Optional[float] = attr.ib(default=None)

    @all_states.default
    def init_all_states(self) -> Dict[
            Type[betterproto.Message], Optional[betterproto.Message]
    ]:  # pylint: disable=no-self-use
        """Initialize the synchronizable states.

        Each pair consists of the type class to specify the states, and an
        actual object to store the state values.
        """
        return {type: None for type in self.message_classes.values()}

    @output_schedule.default
    def init_output_schedule(self) -> Deque[ScheduleEntry]:  # pylint: disable=no-self-use
        """Initialize the output schedule.

        Each pair consists of the type class to specify the message to output
        for the next output, and the time (in seconds) to wait after outputting
        that message before proceeding to outputting the next message.
        """
        return collections.deque([])

    def input(self, event: Optional[UpdateEvent]) -> None:
        """Handle input events."""
        if event is None or not event.has_data():
            return

        if event.time is not None:
            self._logger.debug('Time: %f', event.time)
            self.current_time = event.time
            if self.output_deadline is None:
                self.output_deadline = (
                    self.current_time + self.output_schedule[0].time
                )
        if event.pb_message is None:
            return

        self._logger.debug('Received: %s', event.pb_message)
        message_type = type(event.pb_message)
        try:
            self.all_states[message_type] = event.pb_message
        except KeyError as exc:
            raise exceptions.ProtocolDataError(
                'Received message type is not a synchronizable state: {}'
                .format(message_type)
            ) from exc

    def output(self) -> Optional[betterproto.Message]:
        """Emit the next output event."""
        if self.output_deadline is None:
            return None

        if self.current_time < self.output_deadline:
            return None

        output_type = self.output_schedule[0].type
        try:
            output_event = self.all_states[output_type]
        except KeyError as exc:
            raise exceptions.ProtocolDataError(
                'Scheduled message type is not a synchronizable state: {}'
                .format(output_type)
            ) from exc

        self.output_schedule.rotate(-1)
        self.output_deadline = (
            self.current_time + self.output_schedule[0].time
        )
        if output_event is None:
            return None

        self._logger.debug('Sending: %s', output_event)
        return output_event
