"""Trio I/O Rotary Encoder driver."""

import logging
from typing import Optional, Tuple
import attr

import trio
try:
    import RPi.GPIO as GPIO     # type: ignore
except RuntimeError:
    logging.getLogger().warning('Running without RPi.GPIO!')

from ventserver.io import rotaryencoder
from ventserver.io.trio import endpoints
from ventserver.protocols import exceptions


@attr.s
class RotaryEncoderState:
    """Holds state values for communication and computation."""

    a_quad_state: int = attr.ib(default=None, repr=False)
    a_quad_last_state: int = attr.ib(default=None, repr=False)
    b_quad_state: int = attr.ib(default=None, repr=False)
    rotation_counts: int = attr.ib(default=0, repr=False)
    button_pressed: bool = attr.ib(default=False, repr=False)
    last_pressed: int = attr.ib(default=None, repr=False)
    debounce_time: int = attr.ib(default=10) # debounce time in ms


@attr.s
class Driver(endpoints.IOEndpoint[bytes, Tuple[int, bool]]):
    """Implements driver for reading rotary encoder inputs."""

    _logger = logging.getLogger('.'.join((__name__, 'Driver')))

    _props: rotaryencoder.RotaryEncoderProps = attr.ib(
        factory=rotaryencoder.RotaryEncoderProps
    )
    _connected: bool = attr.ib(default=False)
    _data_available: trio.Event = attr.ib(factory=trio.Event)
    _state: RotaryEncoderState = attr.ib(factory=RotaryEncoderState)
    trio_token: trio.lowlevel.TrioToken = attr.ib(default=None, repr=False)

    def rotation_direction(self, b_quad_pin: int) -> None:
        """Rotary encoder callback function for dail turn event."""

        self._state.a_quad_state = GPIO.input(self._props.a_quad_pin)
        if self._state.a_quad_state != self._state.a_quad_last_state:
            self._state.b_quad_state = GPIO.input(b_quad_pin)
            if self._state.b_quad_state != self._state.a_quad_state:
                self._state.rotation_counts -= 1
            else:
                self._state.rotation_counts += 1

            self._state.a_quad_last_state = self._state.a_quad_state
            trio.from_thread.run_sync(
                self._data_available.set,
                trio_token=self.trio_token
            )


    def button_press_log(self, button_pin: int) -> None:
        """Rotary encoder callback function for button press event."""

        if GPIO.input(button_pin):
            self._state.button_pressed = False
            self._state.rotation_counts = 0
            trio.from_thread.run_sync(
                self._data_available.set,
                trio_token=self.trio_token
            )
        else:
            self._state.button_pressed = True
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
            GPIO.setmode(GPIO.getmode())
            GPIO.setup(
                self._props.a_quad_pin,
                GPIO.IN, pull_up_down=GPIO.PUD_UP
            )
            GPIO.setup(
                self._props.b_quad_pin,
                GPIO.IN, pull_up_down=GPIO.PUD_UP
            )
            GPIO.setup(self._props.button_pin,
                       GPIO.IN, pull_up_down=GPIO.PUD_UP
                       )
            GPIO.add_event_detect(
                self._props.b_quad_pin,
                GPIO.RISING,
                callback=self.rotation_direction
            )
            GPIO.add_event_detect(
                self._props.button_pin,
                GPIO.BOTH,
                callback=self.button_press_log,
                bouncetime=self._state.debounce_time
            )
        except Exception as err:
            raise IOError(err)

        self._connected = True
        self._state.a_quad_last_state = GPIO.input(self._props.a_quad_pin)
        self.trio_token = trio.lowlevel.current_trio_token()


    async def close(self) -> None:
        """Closes the connection with the rotary encoder.

         Closes the connection by cleaning up the GPIO pins on
         the Raspberry Pi board.

        """
        GPIO.cleanup([self._props.a_quad_pin, self._props.b_quad_pin])
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
        return (self._state.rotation_counts, self._state.button_pressed)


    async def send(self, data: Optional[bytes]) -> None:
        """Defined just to fulfill requirements of the
        IOEndpoint's abstract class."""
        return