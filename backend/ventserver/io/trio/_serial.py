"""Trio I/O serial driver."""

import logging
import os
from typing import Optional

import attr

import serial  # type: ignore
from serial import serialutil

import trio
from trio import lowlevel

from ventserver.io import _serial
from ventserver.io.trio import endpoints


@attr.s
class Driver(endpoints.IOEndpoint[bytes, bytes]):
    """Implements a serial I/O driver."""

    _logger = logging.getLogger('.'.join((__name__, 'Driver')))

    props: _serial.SerialProps = attr.ib(factory=_serial.SerialProps)
    _connection: Optional[serial.Serial] = attr.ib(default=None, repr=False)
    _stream: Optional[lowlevel.FdStream] = attr.ib(default=None, repr=False)

    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Open connection to the serial device.

        The nursery argument is ignored for serial connections.
        """
        if self._stream is not None or self._connection is not None:
            raise RuntimeError(
                'Cannot open a serial connection which is already open!'
            )
        if not self.props.port:
            self.props.port = _serial.find_port(self.props.port_filter)
            self._logger.info('Connecting to: %s', self.props.port)
        try:
            self._connection = serial.Serial(
                port=self.props.port,
                baudrate=self.props.baud_rate,
                timeout=self.props.read_timeout,
                write_timeout=self.props.write_timeout
            )
            self._stream = lowlevel.FdStream(os.dup(self._connection.fileno()))
            await trio.sleep(self.props.open_delay)
        except serialutil.SerialException:
            raise IOError(
                'Cannot open a serial connection to {}!'.format(self.props.port)
            )

    @property
    def is_open(self) -> bool:
        """Return whether or not the serial device connection is open."""
        return bool(self._stream)

    async def close(self) -> None:
        """Close the connection to the Serial device."""
        if self._stream is not None:
            await self._stream.aclose()
            self._stream = None
        if self._connection is not None:
            self._connection.close()
            self._connection = None

    async def send(self, data: Optional[bytes]) -> None:
        """Send data over the serial connection."""
        if self._stream is None:
            raise RuntimeError(
                'Cannot send data without an open serial connection!'
            )
        if data is None:
            return

        try:
            await self._stream.send_all(data)
        except (trio.BrokenResourceError, trio.ClosedResourceError):
            await self.close()
            raise BrokenPipeError(
                'Serial connection lost in write!', self.props.port
            )
        if data:
            self._logger.debug('Wrote: %s', data)

    async def receive(self) -> bytes:
        """Receive data from the serial connection."""
        if self._stream is None:
            raise RuntimeError(
                'Cannot receive data without an open serial connection!'
            )

        read = b''
        try:
            while True:
                read = await self._stream.receive_some(
                    max_bytes=self.props.message_length
                )
                if read:
                    break
                await lowlevel.wait_readable(self._stream.fileno())
        except (trio.BrokenResourceError, trio.ClosedResourceError):
            await self.close()
            raise BrokenPipeError(
                'Serial connection lost in read!', self.props.port
            )
        if read:
            self._logger.debug('Read: %s', read)
        return read
