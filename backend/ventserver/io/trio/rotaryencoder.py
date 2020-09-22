"""Trio I/O Rotary Encoder driver."""

import logging
from typing import Optional
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
    angle: int = attr.ib(default=0, repr=False)
    button_pressed: bool = attr.ib(default=False, repr=False)
    

@attr.s
class RotaryEncoderDriver(endpoints.IOEndpoint[bytes, frontend_pb.RotaryEncoder]):
    """Implements driver for reading rotary encoder inputs."""
    _logger = logging.getLogger('.'.join((__name__, 'RotaryEncoderDriver')))

    _props: rotaryencoder.RotaryEncoderProps = attr.ib(
        factory=rotaryencoder.RotaryEncoderProps
    )
    _connected: trio.Event = attr.ib(factory=trio.Event)
    _state = attr.ib(factory=RotaryEncoderState)


    def rotation_direction(self, dt_pin: int) -> None:
        """Rotary encoder callback function for dail turn event."""
        self._state.clk_state = GPIO.input(self._props.clk_pin)
        if self._state.clk_state != self._state.clk_last_state:
            self._state.dt_state = GPIO.input(dt_pin)
            if self._state.dt_state != self._state.clk_state:
                self._state.rotation_counter += 1
            else:
                self._state.rotation_counter -= 1
        self._state.last_angle_change = int(1000 * time.time())
        self._state.angle = ((self._state.rotation_counter * 6) % 360)


    def button_press_log(self, button_pin):
        """Rotary encoder callback function for button press event."""
        if GPIO.input(button_pin):
            self._state.button_pressed = False
            time.sleep(0.01)
        else:
            self._state.button_pressed = True


    @property
    def is_open(self) -> bool:
        """Return whether or not the rotary encoder is connected."""
        return self._connected.is_set()


    async def open(self, nursery:Optional[trio.Nursery] = None) -> None:
        """"""
        if self.is_open:
            raise(exceptions.Protocol)

        self._connected = trio.Event()
        try:
            GPIO.setmode(GPIO.BCM)
            GPIO.setup(self._props.clk_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.setup(self._props.dt_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.setup(self._props.button_pin,
                       GPIO.IN, pull_up_down=GPIO.PUD_UP
                       )
            GPIO.add_event_detect(
                self._props.dt_pin,
                GPIO.RISING,
                callback=self.rotation_direction
            )
            GPIO.add_event_detect(
                self._props.button_pin,
                GPIO.BOTH,
                callback=self.button_press_log
            )
            self._connected.set()
        except Exception as err:
            raise IOError(err)
        self._state.clk_last_state = GPIO.input(self._props.clk_pin)
        await self._connected.wait()
        self._connected.set()


    async def close(self) -> None:
        """"""
        self._connected = trio.Event()
        try:
            GPIO.cleanup()
        except Exception:
            raise()


    async def receive(self) -> bytes:
        """"""
        if not self.is_open:
            raise()

        return (self._state.button_pressed, self._state.angle)


    async def send(self) -> None:
        """"""
        pass
