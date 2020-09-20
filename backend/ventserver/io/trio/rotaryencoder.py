"""Trio I/O Rotary Encoder driver."""

import logging
import os
from typing import Optional
import attr

import RPi.GPIO as GPIO
import trio

from ventserver.io import rotaryencoder
from ventserver.io.trio import endpoints


@attr.s
class RotaryEncoderDriver(endpoints.IOEndpoints[bytes,bytes]):
    """Implements driver for reading rotary encoder inputs."""

    _logger = logging.getLogger('.'.join((__name__, 'RotaryEncoderDriver')))

    _props: rotaryencoder.RotaryEncoderProps = attr.ib(
        factory=rotaryencoder.RotaryEncoderProps
    )
    clk_state: int = attr.ib(default=None, repr=False)
    clk_last_state: int = attr.ib(default=None, repr=False)
    dt_state: int = attr.ib(default=None, repr=False)
    rotation_counter: int = attr.ib(default=0, repr=False)
    angle: int = attr.ib(default=None, repr=False)

    def rotation_direction(self, channel) -> None:
        """"""
        self.clk_state = GPIO.input(self._props.clk_pin)
        if self.clk_state != self.clk_last_state:
            self.dt_state = GPIO.input(self._props.dt_pin)
            if self.dt_state != self.clk_state:
                self.rotation_counter += 1
            else:
                self.rotation_counter -= 1
        self.angle = ((self.rotation_counter * 6) % 360)

    async def open(self, nursery:Optional[trio.Nursery] = None) -> None:
        """"""
        try:
            GPIO.setup(self._props.clk_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.setup(self._props.dt_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        except Exception as err:
            raise IOError(err)
        self.clk_last_state = GPIO.input(_prop.clk_pin)

    async def read(self) -> bytes:
        """"""
        await trio.sleep(10)
        GPIO.add_event_detect(self._props.clk_pin, GPIO.RISING, callback=rotation_direction)
        return self.angle
