"""Trio I/O Rotary Encoder driver."""

import logging
import os
from typing import Optional
import attr
import functools
from datetime import datetime

import RPi.GPIO as GPIO
import trio

from ventserver.io import rotaryencoder
from ventserver.io.trio import endpoints



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
    angle: int = attr.ib(default=0, repr=False)
    button_press_time: str = attr.ib(default=None, repr=False)
    
    def rotation_direction(self, dt_pin: int) -> None:
        """Rotary encoder callback function"""
        self.clk_state = GPIO.input(self._props.clk_pin)
        if self.clk_state != self.clk_last_state:
            self.dt_state = GPIO.input(self._props.dt_pin)
            if self.dt_state != self.clk_state:
                self.rotation_counter += 1
            else:
                self.rotation_counter -= 1
        self.angle = ((self.rotation_counter * 6) % 360)
    
        
    def switch_press_log(self, channel):
        if GPIO.input(27):
            dateTimeObj = datetime.now()
            self.button_press_time = dateTimeObj.strftime("%d - %b - %Y - %H - %M - %S")
            print("button_pressed_at : ", self.button_press_time)

    
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
            GPIO.setup(27, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.add_event_detect(
            self._props.dt_pin,
            GPIO.RISING,
            callback=self.rotation_direction
            )
            GPIO.add_event_detect(
            27,
            GPIO.RISING,
            callback=self.switch_press_log,
            bouncetime=10
            )
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
        return (self.angle, self.button_press_time)
    
    async def send(self) -> None:
        pass


async def main():
    driver = RotaryEncoderDriver()
    await driver.open()
    try:
        async for each in driver.receive_all():
            print("received:",each)
            await trio.sleep(0.10)
    except Exception as err:
        print(err)
    await driver.close()

if __name__ == "__main__":

    trio.run(main)
