import logging
import typing
from typing import Optional, Type, Tuple
import time

import attr

from ventserver.protocols import exceptions
from ventserver.sansio import protocols
from ventserver.sansio import channels
from ventserver.protocols.protobuf import frontend_pb


LowerEvent = Tuple[bool, int]
UpperEvent = frontend_pb.RotaryEncoder


@attr.s
class ReceiveFilter(protocols.Filter[LowerEvent, UpperEvent]):
    """Filter which passes input data in an event class."""

    _logger = logging.getLogger('.'.join((__name__, 'ReceiveFilter')))
    _buffer: channels.DequeChannel[LowerEvent] = attr.ib(
        factory=channels.DequeChannel
    )
    _current_time: int = attr.ib(default=None, repr=False)
    _last_button_down: int = attr.ib(default=0, repr=False)
    _last_button_up: int = attr.ib(default=0, repr=False)
    _last_step_change: int = attr.ib(default=0, repr=False)
    _last_steps: int = attr.ib(default=0, repr=False)
    _button_pressed: bool = attr.ib(default=False, repr=False)

    def input(self, event: Optional[LowerEvent]) -> None:
        """"""
        if event is not None:
            self._buffer.input(event)

    def output(self) -> Optional[UpperEvent]:
        """"""
        event = self._buffer.output()

        self._current_time = int(1000 * time.time()) 
        if event is None:
            return None

        if event[1] != self._last_steps:
            self._last_steps = event[1]
            self._last_step_change = self._current_time

        if event[0] != self._button_pressed:
            if event[0]:
                self._last_button_down = self._current_time
            else:
                self._last_button_up = self._current_time

            self._button_pressed = event[0]

        pb_state = frontend_pb.RotaryEncoder(
            steps=self._last_steps,
            last_step_change=self._last_step_change,
            button_pressed=self._button_pressed,
            last_button_down=self._last_button_down,
            last_button_up=self._last_button_up)

        return pb_state
