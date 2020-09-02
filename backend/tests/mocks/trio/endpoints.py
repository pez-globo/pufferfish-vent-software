"""Mock trio implementations of I/O endpoints."""

import abc
import logging
from typing import Optional, TypeVar

import attr

from cobs import cobs  # type: ignore

import trio

from ventserver.io.trio import endpoints


# Typing


S = TypeVar('S')  # pylint: disable=C0103


# Mocks


@attr.s
class IOEndpoint(endpoints.IOEndpoint[S, S]):
    """Base class for sending data to and receiving data from I/O endpoints."""

    _logger = logging.getLogger('.'.join((__name__, 'IOEndpoint')))

    _open: bool = attr.ib(default=False, repr=False)

    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Open the I/O connection."""
        await trio.sleep(5)
        self._open = True
        self._logger.info('I/O connected: %s', self)

    async def persistently_open(
            self, nursery: Optional[trio.Nursery] = None,
            retry_interval: float = 0
    ) -> None:
        """Try persistently to open the I/O connection."""
        await self.open()

    @property
    def is_open(self) -> bool:
        """Return whether or not the I/O connection is open."""
        return self._open

    async def close(self) -> None:
        """Close the I/O connection."""
        self._open = False

    @abc.abstractmethod
    async def send(self, data: Optional[S]) -> None:
        """Send data over the endpoint."""


@attr.s
class SerialDriver(IOEndpoint[bytes]):
    """Synchronous mock for serial I/O subsystem."""

    _logger = logging.getLogger('.'.join((__name__, 'SerialDriver')))

    exc_interval: int = attr.ib(default=5, repr=False)
    exc_countdown: int = attr.ib(default=5, repr=False)

    async def send(self, data: Optional[bytes]) -> None:
        """Send data over the serial connection."""
        if not self._open:
            raise BrokenPipeError('Cannot write on a broken serial connection!')
        if data is None:
            return
        await trio.sleep(0.5)
        if data:
            self._logger.debug('Wrote: %s', data)

        self.exc_countdown -= 1
        if not self.exc_countdown:
            self.exc_countdown = self.exc_interval
            self._open = False
            self._logger.info('About to cause some chaos!')

    async def receive(self) -> bytes:
        """Receive data from the serial connection."""
        if not self._open:
            raise BrokenPipeError('Cannot read on a broken serial connection!')
        await trio.sleep(0.5)
        read: bytes = (
            cobs.encode(b'new serial rx!') + b'\0'
            + cobs.encode(b'another new serial rx!') + b'\0'
        )
        if read:
            self._logger.debug('Read: %s', read)
        return read


@attr.s
class WebSocketDriver(IOEndpoint[bytes]):
    """Synchronous mock for WebSocket I/O subsystem."""

    _logger = logging.getLogger('.'.join((__name__, 'WebSocketDriver')))

    exc_interval: int = attr.ib(default=10, repr=False)
    exc_countdown: int = attr.ib(default=10, repr=False)

    async def send(self, data: Optional[bytes]) -> None:
        """Send data over the WebSocket connection."""
        if data is None:
            return
        await trio.sleep(0.5)
        self._logger.debug('Sent: %s', data)

    async def receive(self) -> bytes:
        """Receive data from the WebSocket connection."""
        self.exc_countdown -= 1
        if not self.exc_countdown:
            self.exc_countdown = self.exc_interval
            raise BrokenPipeError('WebSocket Mock caused some chaos!')

        await trio.sleep(0.5)
        output = b'new websocket rx'
        self._logger.debug('Received: %s', output)
        return output
