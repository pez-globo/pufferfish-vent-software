import logging
import typing
from typing import Optional, Type, Tuple
import time

import attr

from ventserver.protocols import exceptions
from ventserver.protocols import events
from ventserver.sansio import protocols
from ventserver.sansio import channels
from ventserver.protocols.protobuf import frontend_pb


class ReceiveEvent(events.Event):
    """Rotary encoder input receive event"""

    time: Optional[float] = attr.ib(default=None)
    step: int = attr.ib(default=None)
    button_pressed: bool = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return (
            self.time is not None
            or self.step is not None or self.button_pressed is not None
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
    _last_button_down: int = attr.ib(default=0, repr=False)
    _last_button_up: int = attr.ib(default=0, repr=False)
    _last_step_change: int = attr.ib(default=0, repr=False)
    _last_step: int = attr.ib(default=0, repr=False)
    _button_pressed: bool = attr.ib(default=False, repr=False)

    def input(self, event: Optional[LowerEvent]) -> None:
        """"""
        if event is not None:
            self._buffer.input(event)

    def output(self) -> Optional[UpperEvent]:
        """"""
        event = self._buffer.output()

        if not event.has_data:
            return None

        self._current_time = event.time
        if event.step != self._last_step:
            self._last_step = event.step
            self._last_step_change = self._current_time

        if event.button_pressed != self._button_pressed:
            if event.button_pressed:
                self._last_button_down = self._current_time
            else:
                self._last_button_up = self._current_time

            self._button_pressed = event.button_pressed

        pb_state = frontend_pb.RotaryEncoder(
            step=self._last_step,
            last_step_change=self._last_step_change,
            button_pressed=self._button_pressed,
            last_button_down=self._last_button_down,
            last_button_up=self._last_button_up)

        return pb_state
