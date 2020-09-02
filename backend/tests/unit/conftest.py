"""Define reusable fixtures."""

import pytest as pt  # type: ignore

from ventserver.io.websocket import WebSocketProps


@pt.fixture()
def websocket_props() -> WebSocketProps:
    """Reserve a websocket host and port."""
    return WebSocketProps()
