"""Rotary Encoder communication protocol"""

import logging
from typing import Optional, Tuple
import time

import attr

# from ventserver.protocols import exceptions
from ventserver.protocols import events
from ventserver.sansio import protocols
from ventserver.sansio import channels
from ventserver.protocols.protobuf import frontend_pb

@attr.s
class ReceiveEvent(events.Event):
    """Rotary encoder input receive event"""

    time: float = attr.ib(default=None)
    re_data: Tuple[int, bool] = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return (
            self.time is not None and self.re_data is not None
        )


LowerEvent = ReceiveEvent
UpperEvent = frontend_pb.RotaryEncoder


@attr.s
class ReceiveFilter(protocols.Filter[LowerEvent, UpperEvent]):
    """Filter which passes input data in an event class."""

    _logger = logging.getLogger('.'.join((__name__, 'ReceiveFilter')))
    _buffer: channels.DequeChannel[LowerEvent] = attr.ib(
        factory=channels.DequeChannel
    )
    _current_time: float = attr.ib(default=time.time(), repr=False)
    _last_button_down: float = attr.ib(default=0, repr=False)
    _last_button_up: float = attr.ib(default=0, repr=False)
    _last_step_change: float = attr.ib(default=0, repr=False)
    _last_step: int = attr.ib(default=0, repr=False)
    _button_pressed: bool = attr.ib(default=False, repr=False)

    def input(self, event: Optional[LowerEvent]) -> None:
        """Handle input events for the receive filter."""
        if event is None or not event.has_data():
            return

        self._buffer.input(event)

    def output(self) -> Optional[UpperEvent]:
        """Process the input events of the receive filter."""
        event = self._buffer.output()

        if not event:
            return None

        self._current_time = event.time
        if event.re_data[0] != self._last_step:
            self._last_step = event.re_data[0]
            self._last_step_change = self._current_time

        if event.re_data[1] != self._button_pressed:
            if event.re_data[1]:
                self._last_button_down = self._current_time
            else:
                self._last_button_up = self._current_time

            self._button_pressed = event.re_data[1]

        pb_state = frontend_pb.RotaryEncoder(
            step=self._last_step,
            last_step_change=self._last_step_change,
            button_pressed=self._button_pressed,
            last_button_down=self._last_button_down,
            last_button_up=self._last_button_up)

        return pb_state
