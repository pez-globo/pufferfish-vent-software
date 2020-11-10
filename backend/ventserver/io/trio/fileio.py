"""Trio I/O WebSocket driver.
'Optional[trio._AsycRawIOBase]'"""
import os
import logging
from typing import Optional

import attr
import trio

from ventserver.io.trio import endpoints
from ventserver.io import fileobject

@attr.s
class Handler(endpoints.IOEndpoint[bytes, bytes]):
    """ Base class for reading and writing data to and fro protobuf files. """
    _logger = logging.getLogger('.'.join((__name__, 'Handler')))

    props: fileobject.FileProps = attr.ib(
        factory=fileobject.FileProps, repr=False
    )
    rootdir: str = attr.ib(
        default=os.path.join(os.getcwd(), "ventserver", "statestore")
    )
    _fileobject: Optional['trio._AsyncBufferedIOBase'] = attr.ib(default=None)
    _connected: trio.Event = attr.ib(factory=trio.Event, repr=False)


    def set_props(self, filename: str, mode: Optional[str] = 'rb') -> None:
        """Sets properties of file to establish I/O connection."""
        if not ".pb" in filename:
            filename += ".pb"
        self.props.filename = filename
        if mode:
            self.props.mode = mode

    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Open File I/O connection.

        The mypy 'type: ignore' is added to trio.open_file() because the second
        argument, mode, needs to be literal to return trio.AsyncBufferedIOBase.
        If not it returns trio._AsyncIOBase, which doesn't have read() and
        write()method in it's class signature."""
        if self._fileobject is not None:
            raise RuntimeError("Cannot open new file instance" +
                               "if one is already open." +
                               "Please close the old file instance."
                              )

        _filepath = os.path.join(self.rootdir, self.props.filedir)
        if not os.path.exists(_filepath):
            self._logger.info("No such directory: %s", _filepath)
            os.mkdir(_filepath)
            self._logger.info("Created directory: %s", _filepath)

        try:
            # raises OSError
            self._fileobject = await trio.open_file(    # type: ignore
                os.path.join(_filepath, self.props.filename),
                str(self.props.mode)
            )
        except OSError as err:
            raise OSError("Handler: {}".format(err)) from err

        self._logger.debug('File %s opened.', self.props.filename)
        self._connected.set()

    @property
    def is_open(self) -> bool:
        """Check if file connection is open."""
        return self._connected.is_set()

    async def close(self) -> None:
        """Close File I/O connection."""
        if (self._fileobject is None) and (not self.is_open):
            return

        assert self._fileobject is not None
        await self._fileobject.aclose()
        self._fileobject = None
        self._logger.debug('File %s closed.', self.props.filename)
        self._connected = trio.Event()

    async def receive(self) -> bytes:
        """Reads data from protobuf file and returns a bytes object."""
        if self._fileobject is None:
            raise RuntimeError("No file object defined to read.")
        # TODO: Recognize and handle async File I/O exceptions
        _data = await self._fileobject.read()
        return _data

    async def send(self, data: Optional[bytes]) -> None:
        """Writes data to the protobuf file."""
        if data is None:
            return
        if self._fileobject is None:
            raise RuntimeError("No file object defined to write.")

        await self._fileobject.write(data)  # raises OSError
