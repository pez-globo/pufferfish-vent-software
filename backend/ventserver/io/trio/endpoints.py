"""Asynchronous I/O endpoint interfaces."""

import abc
import logging
from typing import Optional, TypeVar

import trio

from ventserver.io.asynchronous import endpoints


# Typing


_SendData = TypeVar('_SendData')
_ReceiveData = TypeVar('_ReceiveData')


# I/O Endpoints


class IOEndpoint(endpoints.IOEndpoint[_SendData, _ReceiveData]):
    """Base class for sending data to and receiving data from I/O endpoints."""

    _logger = logging.getLogger('.'.join((__name__, 'IOEndpoint')))

    @abc.abstractmethod
    async def open(self, nursery: Optional[trio.Nursery] = None) -> None:
        """Open the I/O connection.

        Nursery can be used by some I/O drivers to set the nursery used for
        spawning child tasks and listening for connections.
        """

    async def persistently_open(
            self, nursery: Optional[trio.Nursery] = None,
            retry_interval: float = 0
    ) -> None:
        """Try persistently to open the I/O connection.

        Note: retry_interval=0 means that this coroutine will just hit an
        async checkpoint after every failed connection, to give an opportunity
        for other Trio tasks to run.
        """
        retrying_connection = False
        while True:
            try:
                await self.open(nursery=nursery)
                self._logger.info('I/O connected: %s', self)
                return
            except IOError:
                if not retrying_connection:
                    if retry_interval:
                        self._logger.info(
                            'I/O connection not found, trying again in %s s: '
                            '%s', retry_interval, self
                        )
                    else:
                        self._logger.info(
                            'I/O connection not found, trying again: %s', self
                        )
                await trio.sleep(retry_interval)
                retrying_connection = True
