"""Trio I/O Rotary Encoder driver."""

import logging
import os
from typing import Optional
import attr
import functools

import RPi.GPIO as GPIO
import trio

from ventserver.io import rotaryencoder
from ventserver.io.trio import endpoints


def rotation_direction(dt_pin: int, clk_pin: int, 
                       clk_state: int, dt_state: int,
                       clk_last_state: int, angle: int,
                       rotation_counter: int
) -> None:
    """Rotary encoder callback function"""
    clk_state = GPIO.input(clk_pin)
    if clk_state != clk_last_state:
        dt_state = GPIO.input(dt_pin)
        if dt_state != clk_state:
            rotation_counter += 1
        else:
            rotation_counter -= 1
    angle = ((rotation_counter * 6) % 360)
    print(angle)


@attr.s
class RotaryEncoderDriver(endpoints.IOEndpoint[bytes, bytes]):
    """Implements driver for reading rotary encoder inputs."""

    _logger = logging.getLogger('.'.join((__name__, 'RotaryEncoderDriver')))

    _props: rotaryencoder.RotaryEncoderProps = attr.ib(
        factory=rotaryencoder.RotaryEncoderProps
    )
    _connected: trio.Event = attr.ib(factory=trio.Event, repr=False)
    clk_state: int = attr.ib(default=None, repr=False)
    clk_last_state: int = attr.ib(default=None, repr=False)
    dt_state: int = attr.ib(default=None, repr=False)
    rotation_counter: int = attr.ib(default=0, repr=False)
    angle: int = attr.ib(default=None, repr=False)

    
    @property
    def is_open(self) -> bool:
        """Return whether or not the rotary encoder is connected."""
        return self._connected.is_set()

    async def open(self, nursery:Optional[trio.Nursery] = None) -> None:
        """"""
        self._connected = trio.Event()
        try:
            GPIO.setmode(GPIO.BCM)
            GPIO.setup(self._props.clk_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.setup(self._props.dt_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            self._connected.set()
        except Exception as err:
            raise IOError(err)
        self.clk_last_state = GPIO.input(self._props.clk_pin)
        await self._connected.wait()
        self._connected.set()

    async def close(self) -> None:
        """"""
        self._connected = trio.Event()
        GPIO.cleanup()

    async def receive(self) -> bytes:
        """"""
        call_back = functools.partial(
                        rotation_direction,
                        clk_pin=self._props.clk_pin, clk_state=self.clk_state,
                        dt_state=self.dt_state,
                        clk_last_state=self.clk_last_state,
                        rotation_counter=self.rotation_counter,
                        angle=self.angle
        )
        GPIO.add_event_detect(
            self._props.dt_pin,
            GPIO.RISING,
            callback=call_back
        )
        return self.angle
    
    async def send(self) -> None:
        pass


async def main():
    driver = RotaryEncoderDriver()
    await driver.open()
    angle = await driver.receive()
    print(f"{angle} {driver.angle}")
    await trio.sleep(12)

if __name__ == "__main__":

    trio.run(main)
