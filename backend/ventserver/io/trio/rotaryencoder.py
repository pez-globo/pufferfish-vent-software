"""Trio I/O Rotary Encoder driver."""

import logging
from typing import Optional, Tuple
import time
import attr

import trio
import RPi.GPIO as GPIO

from ventserver.io import rotaryencoder
from ventserver.io.trio import endpoints


@attr.s
class RotaryEncoderState:
    """"""
    clk_state: int = attr.ib(default=None, repr=False)
    clk_last_state: int = attr.ib(default=None, repr=False)
    dt_state: int = attr.ib(default=None, repr=False)
    rotation_counter: int = attr.ib(default=0, repr=False)
    button_pressed: bool = attr.ib(default=False, repr=False)
    last_pressed: int = attr.ib(default=None, repr=False)
    

@attr.s
class Driver(endpoints.IOEndpoint[bytes, Tuple[bool, int]]):
    """Implements driver for reading rotary encoder inputs."""
    _logger = logging.getLogger('.'.join((__name__, 'RotaryEncoderDriver')))

    _props: rotaryencoder.RotaryEncoderProps = attr.ib(
        factory=rotaryencoder.RotaryEncoderProps
    )
    _data_available: trio.Event = attr.ib(factory=trio.Event)
    _state: int = attr.ib(factory=RotaryEncoderState)
    _debounce_time: int = attr.ib(default=15) # debounce time in ms 


    def rotation_direction(self, clk_pin: int) -> None:
        """Rotary encoder callback function for dail turn event."""
        self._state.dt_state = GPIO.input(self._props.dt_pin)
        if self._state.clk_state != self._state.dt_last_state:
            self._state.clk_state = GPIO.input(clk_pin)
            if self._state.dt_state != self._state.clk_state:
                self._state.rotation_counts -= 1
            else:
                self._state.rotation_counts += 1

            self._connected.set()


    def button_press_log(self, button_pin):
        """Rotary encoder callback function for button press event."""
        if GPIO.input(button_pin):
            self._state.button_pressed = False
            self._connected.set()
        else:
            self._state.button_pressed = True
            self._state.rotation_counts = 0
            self._connected.set()


    @property
    def is_available(self) -> bool:
        """Return whether or not the rotary encoder is connected."""
        return self._data_available.is_set()


    async def open(self, nursery:Optional[trio.Nursery] = None) -> None:
        """"""
        if self.is_open:
            raise(exceptions.Protocol)

        try:
            GPIO.setmode(GPIO.BCM)
            GPIO.setup(self._props.clk_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.setup(self._props.dt_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.setup(self._props.button_pin,
                       GPIO.IN, pull_up_down=GPIO.PUD_UP
                       )
            GPIO.add_event_detect(
                self._props.clk_pin,
                GPIO.RISING,
                callback=self.rotation_direction
            )
            GPIO.add_event_detect(
                self._props.button_pin,
                GPIO.BOTH,
                callback=self.button_press_log,
                bouncetime=self._debounce_time
            )
        except Exception as err:
            raise IOError(err)
        self._state.clk_last_state = GPIO.input(self._props.clk_pin)



    async def close(self) -> None:
        """"""
        try:
            GPIO.cleanup([self._state.clk_pin, self._state.dt_pin])
        except Exception:
            raise()


    async def receive(self) -> Tuple:
        """"""
        if not self.is_open:
            raise()

        await self._connected.wait()
        self._connected = trio.Event()
        return (self._state.button_pressed, self._state.rotation_counts)


    async def send(self) -> None:
        """"""
        pass
