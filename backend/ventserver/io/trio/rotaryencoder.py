"""Trio I/O Rotary Encoder driver."""

import logging
from typing import Optional, Tuple

import attr
import pigpio     # type: ignore
import trio

from ventserver.io import rotaryencoder
from ventserver.io.trio import endpoints
from ventserver.protocols import exceptions


# States
B_GPIO1 = 'D'  # dt_gpio is high
B_GPIO0 = 'd'  # dt_gpio is low
A_GPIO1 = 'C'  # clk_gpio is high
A_GPIO0 = 'c'  # clk_gpio is low

# State sequences
SEQUENCE_UP = B_GPIO1 + A_GPIO1 + B_GPIO0 + A_GPIO0
SEQUENCE_DOWN = A_GPIO1 + B_GPIO1 + A_GPIO0 + B_GPIO0

@attr.s
class Driver(endpoints.IOEndpoint[bytes, Tuple[int, bool]]):
    """Implements driver for reading rotary encoder inputs."""

    _logger = logging.getLogger('.'.join((__name__, 'Driver')))

    _props: rotaryencoder.RotaryEncoderProps = attr.ib(
        factory=rotaryencoder.RotaryEncoderProps
    )
    rpi: pigpio.pi = attr.ib(factory=pigpio.pi)
    _connected: bool = attr.ib(default=False)
    _data_available: trio.Event = attr.ib(factory=trio.Event)
    _sequence: str = attr.ib(default='')
    counter: int = attr.ib(default=0, repr=False)
    button_pressed: bool = attr.ib(default=False, repr=False)
    debounce_time: int = attr.ib(default=300)   # debounce time in us
    button_debounce_time: int = attr.ib(default=100)    # debounce time in us
    trio_token: trio.lowlevel.TrioToken = attr.ib(default=None, repr=False)


    def a_quad_rise(self, gpio: int, level: int, tick: int) -> None:
        """Callback for rising A qaudrature pin."""
        _, __, ___ = gpio, level, tick  # Unused args
        self._sequence += A_GPIO0
        if self._sequence == SEQUENCE_UP:
            self.counter += 1
            self._sequence = ''
            trio.from_thread.run_sync(
                self._data_available.set,
                trio_token=self.trio_token
            )

    def a_quad_fall(self, gpio: int, level: int, tick: int) -> None:
        """Callback for falling A qaudrature pin."""
        _, __, ___ = gpio, level, tick  # Unused args
        if len(self._sequence) > 2:
            self._sequence = ''
        self._sequence += A_GPIO1

    def b_quad_rise(self, gpio: int, level: int, tick: int) -> None:
        """Callback for rising B qaudrature pin."""
        _, __, ___ = gpio, level, tick  # Unused args
        self._sequence += B_GPIO0
        if self._sequence == SEQUENCE_DOWN:
            self.counter -= 1
            self._sequence = ''
            trio.from_thread.run_sync(
                self._data_available.set,
                trio_token=self.trio_token
            )

    def b_quad_fall(self, gpio: int, level: int, tick: int) -> None:
        """Callback for falling B qaudrature pin."""
        _, __, ___ = gpio, level, tick  # Unused args
        if len(self._sequence) > 2:
            self._sequence = ''
        self._sequence += B_GPIO1

    def button_rise(self, gpio: int, level: int, tick: int) -> None:
        """Callback for rising button pin."""
        _, __, ___ = gpio, level, tick  # Unused args
        if self.button_pressed:
            self.button_pressed = False
            self.counter = 0
            trio.from_thread.run_sync(
                self._data_available.set,
                trio_token=self.trio_token
            )

    def button_fall(self, gpio: int, level: int, tick: int) -> None:
        """Callback for falling button pin."""
        _, __, ___ = gpio, level, tick  # Unused args
        if not self.button_pressed:
            self.button_pressed = True
            trio.from_thread.run_sync(
                self._data_available.set,
                trio_token=self.trio_token
            )


    @property
    def is_data_available(self) -> bool:
        """Return whether or not new state is available or not."""
        return self._data_available.is_set()

    @property
    def is_open(self) -> bool:
        """Return whether or not the rotary encoder is connected."""
        return self._connected

    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Opens the connection with the rotary encoder.
        Raises:
            ProtocolError:"Rotary Encoder is already connected at A:a_quad,
                B:b_quad GPIO pins."

            IOError
        """

        if self.is_open:
            exception = ("Rotary Encoder is already connected at "
                         f"A:{self._props.a_quad_pin}, "
                         "B:{self._props.b_quad_pin} GPIO pins.")
            raise exceptions.ProtocolError(exception)

        try:

            self.rpi.set_glitch_filter(
                self._props.a_quad_pin,
                self.debounce_time
            )
            self.rpi.callback(
                self._props.a_quad_pin,
                pigpio.FALLING_EDGE,
                self.a_quad_fall
            )
            self.rpi.callback(
                self._props.a_quad_pin,
                pigpio.RISING_EDGE,
                self.a_quad_rise
            )
            self.rpi.set_glitch_filter(
                self._props.b_quad_pin,
                self.debounce_time
            )
            self.rpi.callback(
                self._props.b_quad_pin,
                pigpio.FALLING_EDGE,
                self.b_quad_fall
            )
            self.rpi.callback(
                self._props.b_quad_pin,
                pigpio.RISING_EDGE,
                self.b_quad_rise
            )
            self.rpi.set_pull_up_down(
                self._props.button_pin,
                pigpio.PUD_UP
            )
            self.rpi.set_glitch_filter(
                self._props.button_pin,
                self.button_debounce_time
            )
            self.rpi.callback(
                self._props.button_pin,
                pigpio.FALLING_EDGE,
                self.button_fall
            )
            self.rpi.callback(
                self._props.button_pin,
                pigpio.RISING_EDGE,
                self.button_rise
            )
        except Exception as err:
            raise exceptions.ProtocolError(err)

        self._connected = True
        self.trio_token = trio.lowlevel.current_trio_token()

    async def close(self) -> None:
        """Closes the connection with the rotary encoder.

         Closes the connection by cleaning up the GPIO pins on
         the Raspberry Pi board.

        """
        self._connected = False

    async def receive(self) -> Tuple[int, bool]:
        """Shares current rotation counts and button
        pressed states with the caller.

        Waits for the the rotary encoder to change state and the callback
        function to set the trio.Event.

        Raises:
            ProtocolError: No Rotary Encoder is connected.Try checking
                your RPi Connections.
        """

        if not self.is_open:
            exception = ("No Rotary Encoder is connected. "
                         "Try checking your RPi Connections.")
            raise exceptions.ProtocolError(exception)

        await self._data_available.wait()
        self._data_available = trio.Event()
        return (self.counter, self.button_pressed)

    async def send(self, data: Optional[bytes]) -> None:
        """Defined just to fulfill requirements of the
        IOEndpoint's abstract class."""
        return
