"""Trio I/O Rotary Encoder driver."""

import logging
from typing import Optional
import time
import attr

import trio
import RPi.GPIO as GPIO

from ventserver.io import rotaryencoder
from ventserver.io.trio import endpoints
from ventserver.protocols.protobuf import frontend_pb


@attr.s
class RotaryEncoderState:
    """"""
    clk_state: int = attr.ib(default=None, repr=False)
    clk_last_state: int = attr.ib(default=None, repr=False)
    dt_state: int = attr.ib(default=None, repr=False)
    rotation_counter: int = attr.ib(default=0, repr=False)
    angle: int = attr.ib(default=0, repr=False)
    button_pressed: bool = attr.ib(default=False, repr=False)
    button_press_time: str = attr.ib(default=None, repr=False)
    button_release_time: str = attr.ib(default=None, repr=False)
    last_angle_change: str = attr.ib(default=None, repr=False)


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
        """Rotary encoder callback function"""
        self._state.clk_state = GPIO.input(self._props.clk_pin)
        if self._state.clk_state != self._state.clk_last_state:
            self._state.dt_state = GPIO.input(self._props.dt_pin)
            if self._state.dt_state != self._state.clk_state:
                self._state.rotation_counter += 1
            else:
                self._state.rotation_counter -= 1
        self._state.last_angle_change = int(1000 * time.time())
        self._state.angle = ((self._state.rotation_counter * 6) % 360)


    def button_press_log(self, channel):
        """"""
        if GPIO.input(self._props.button_pin):
            self._state.button_release_time = int(1000 * time.time())
            self._state.button_pressed = False
            time.sleep(0.01)
        else:
            self._state.button_press_time = int(1000 * time.time())
            self._state.button_pressed = True


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
        GPIO.cleanup()


    async def receive(self) -> bytes:
        """"""
        pb_state = frontend_pb.RotaryEncoder(
            angle=self._state.angle,
            last_angle_change=self._state.last_angle_change,
            last_button_down=self._state.button_press_time,
            last_button_up=self._state.button_release_time,
            button_pressed=self._state.button_pressed
            )
        return pb_state


    async def send(self) -> None:
        """"""
        pass


async def main():
    """"""
    driver = RotaryEncoderDriver()
    await driver.open()
    try:
        async for each in driver.receive_all():
            print("received:", each)
            await trio.sleep(0.10)
    except Exception as err:
        print(err)
    await driver.close()

if __name__ == "__main__":

    trio.run(main)
