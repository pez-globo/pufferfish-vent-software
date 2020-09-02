"""Trio I/O WebSocket driver."""

import functools
import logging
from typing import Awaitable, Callable, Optional

import attr

import trio

import trio_websocket  # type: ignore

from ventserver.io import websocket
from ventserver.io.trio import endpoints


@attr.s
class Server:
    """Implements a WebSocket I/O server."""

    _logger = logging.getLogger('.'.join((__name__, 'Server')))

    _request_handler: Callable[
        [trio_websocket.WebSocketRequest], Awaitable[None]
    ] = attr.ib(repr=False)
    props: websocket.WebSocketProps = attr.ib(factory=websocket.WebSocketProps)
    _running: bool = attr.ib(default=False, init=False)

    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Start the WebSocket server."""
        if self._running:
            raise RuntimeError('WebSocket Server is already running!')

        self._logger.info(
            'Starting: %s:%d', self.props.host, self.props.port
        )
        self._running = True
        if nursery is None:
            async with trio.open_nursery() as new_nursery:
                await self._start_server(new_nursery)
        else:
            await self._start_server(nursery)

    async def _start_server(self, nursery: trio.Nursery) -> None:
        """Actually start the WebSocket server."""
        await nursery.start(
            functools.partial(
                trio_websocket.serve_websocket,
                message_queue_size=self.props.message_queue_size,
                max_message_size=self.props.max_message_size,
                connect_timeout=self.props.connect_timeout,
                disconnect_timeout=self.props.disconnect_timeout
            ),
            self._request_handler, self.props.host, self.props.port,
            self.props.ssl_context
        )

    @property
    def is_open(self) -> bool:
        """Return whether the server is running."""
        return self._running


@attr.s
class SingleConnectionService:
    """Implements a WebSocket I/O server which only services one connection."""

    _logger = logging.getLogger('.'.join((__name__, 'SingleConnectionService')))

    _connection_handler: Callable[
        [trio_websocket.WebSocketConnection], Awaitable[None]
    ] = attr.ib(repr=False)
    props: websocket.WebSocketProps = attr.ib(
        factory=websocket.WebSocketProps, repr=False
    )
    server: Server = attr.ib()
    _connection: Optional[trio_websocket.WebSocketConnection] = attr.ib(
        default=None, init=False, repr=False
    )

    @server.default
    def init_server(self) -> Server:
        """Initialize the server."""
        return Server(request_handler=self._request_handler, props=self.props)

    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Start the WebSocket server."""
        await self.server.open(nursery)

    @property
    def is_open(self) -> bool:
        """Return whether the service is running."""
        return self.server.is_open

    async def _request_handler(
            self, request: trio_websocket.WebSocketRequest
    ) -> None:
        """Handle incoming connection requests."""
        self._logger.info('Received new connection request')
        if self._connection is not None:
            self._logger.info('Ignoring new connection request')
            await request.reject(503, body=(
                b'The backend is already servicing a WebSocket connection and '
                b'cannot accept another WebSocket connection request.'
            ))
            return
        self._connection = await request.accept()
        self._logger.info('Accepted connection request')
        try:
            await self._connection_handler(self._connection)
        except (AttributeError, trio_websocket.ConnectionClosed):
            self._logger.warning('Connection was closed')
            self._connection = None
            return
        except trio.Cancelled:
            with trio.move_on_after(
                    self.props.graceful_close_timeout
            ) as cancel_scope:
                cancel_scope.shield = True
                self._logger.info('Closing connection...')
                await self._connection.aclose(
                    reason='Server decided to close the connection.'
                )
                self._logger.info('Closed connection')
                raise
        self._connection = None


@attr.s
class Driver(endpoints.IOEndpoint[bytes, bytes]):
    """Implements a WebSocket I/O driver."""

    _logger = logging.getLogger('.'.join((__name__, 'Driver')))

    props: websocket.WebSocketProps = attr.ib(
        factory=websocket.WebSocketProps, repr=False
    )
    _server: SingleConnectionService = attr.ib()
    _connection: Optional[trio_websocket.WebSocketConnection] = attr.ib(
        default=None, init=False, repr=False
    )
    _connected: trio.Event = attr.ib(factory=trio.Event, repr=False)
    _nursery: Optional[trio.Nursery] = attr.ib(
        default=None, init=False, repr=False
    )

    @_server.default
    def init_server(self) -> SingleConnectionService:
        """Initialize the server."""
        return SingleConnectionService(self._connection_handler, self.props)

    async def wait_connected(self) -> None:
        """Wait until a connection is established."""
        await self._connected.wait()

    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Open WebSocket connection.

        If nursery is not provided, a new nursery will be created and open()
        will only return after the server stops. You probably want to provide
        a nursery.
        """
        if self._connection is not None:
            raise RuntimeError(
                'Cannot open a WebSocket connection which is already open!'
            )
        self._connected = trio.Event()
        if not self._server.is_open:
            await self._server.open(nursery=nursery)
        await self.wait_connected()

    async def _connection_handler(
            self, connection: trio_websocket.WebSocketConnection
    ) -> None:
        """Handle a new connection."""
        self._connection = connection
        self._connected.set()
        while True:
            try:
                await self._connection.ping()
                await trio.sleep(self.props.ping_interval)
            except ValueError:
                self._logger.warning(
                    'Payload of WebSocket ping is identical to that of another '
                    'in-flight ping'
                )

    @property
    def is_open(self) -> bool:
        """Return whether or not the WebSocket connection is open."""
        return self._connected.is_set()

    async def close(self) -> None:
        """Close the WebSocket connection."""
        self._connected = trio.Event()
        if (not self._server.is_open) or (self._connection is None):
            return

        self._logger.info('Closed connection')
        self._connection = None

    async def send(self, data: Optional[bytes]) -> None:
        """Send data over the serial connection."""
        if self._connection is None:
            raise RuntimeError(
                'Cannot send data without an open WebSocket connection!'
            )
        if data is None:
            return

        try:
            await self._connection.send_message(data)
        except trio_websocket.ConnectionClosed:
            raise BrokenPipeError('WebSocket connection lost in write!')
        self._logger.debug('Sent: %s', data)

    async def receive(self) -> bytes:
        """Receive data from the serial connection."""
        if self._connection is None:
            raise RuntimeError(
                'Cannot receive data without an open WebSocket connection!'
            )

        try:
            message: bytes = await self._connection.get_message()
            self._logger.debug('Received: %s', message)
            return message
        except trio_websocket.ConnectionClosed:
            raise BrokenPipeError('WebSocket connection lost in write!')
