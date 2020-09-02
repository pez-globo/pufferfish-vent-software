"""Async protocol interfaces."""

import abc
from typing import AsyncGenerator, Generic, Optional, TypeVar


# Filters


_InputEvent = TypeVar('_InputEvent')
_OutputEvent = TypeVar('_OutputEvent')


class Filter(Generic[_InputEvent, _OutputEvent], abc.ABC):
    """Interface class for filters in a pipe-and-filter architecture."""

    @abc.abstractmethod
    async def input(self, data: Optional[_InputEvent]) -> None:
        """Push a data item onto the channel.

        Finishes only when the data item can be pushed onto the channel.
        """

    @abc.abstractmethod
    async def output(self) -> _OutputEvent:
        """Pull a data item from the channel.

        Finishes only when a data item can be pulled from the channel.
        """

    async def output_all(self) -> AsyncGenerator[_OutputEvent, None]:
        """Generate all outputs as they become ready for emission.

        Never finishes.
        """
        while True:
            yield await self.output()
