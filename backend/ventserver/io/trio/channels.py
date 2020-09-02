"""Trio-backed channel implementations."""

import math
from typing import Optional, Tuple, TypeVar

import attr

import trio

from ventserver.io.asynchronous import protocols


_Data = TypeVar('_Data')
TrioChannelPair = Tuple[trio.MemorySendChannel, trio.MemoryReceiveChannel]


@attr.s
class TrioChannel(protocols.Filter[_Data, _Data]):
    """A channel backed by a deque."""

    maxlen: Optional[int] = attr.ib(default=0)
    _trio_endpoints: TrioChannelPair = attr.ib()
    push_endpoint = attr.ib(init=False, repr=False)
    pull_endpoint = attr.ib(init=False, repr=False)

    @maxlen.validator
    def _check_maxlen(  # pylint: disable=no-self-use
            self, _: 'attr.Attribute[Optional[int]]', value: int
    ) -> None:
        """Validate the attr input for maxlen.

        Raises:
            ValueError: attr init value is a negative number.

        """
        if value is not None and value < 0:
            raise ValueError(
                'Max Length cannot be negative: {}'.format(value)
            )

    @_trio_endpoints.default
    def init_endpoints(self) -> TrioChannelPair:
        """Initialize the pusher."""
        if self.maxlen is None:
            maxlen = math.inf
        else:
            maxlen = self.maxlen
        return trio.open_memory_channel(max_buffer_size=maxlen)

    @push_endpoint.default
    def init_push_endpoint(self) -> 'trio.MemorySendChannel[_Data]':
        """Initialize the push endpoint."""
        return self._trio_endpoints[0]

    @pull_endpoint.default
    def init_pull_endpoint(self) -> 'trio.MemoryReceiveChannel[_Data]':
        """Initialize the pull endpoint."""
        return self._trio_endpoints[1]

    async def input(self, data: Optional[_Data]) -> None:
        """Push a data item onto the channel.

        Finishes only when the data item can be pushed onto the channel.
        """
        if data is None:
            return

        await self.push_endpoint.send(data)

    async def output(self) -> _Data:
        """Pull a data item from the channel.

        Finishes only when a data item can be pulled from the channel.
        """
        output: _Data = await self.pull_endpoint.receive()
        return output
