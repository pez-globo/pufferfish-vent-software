"""Generic support for WebSocket connections."""

import ssl
from typing import Optional

import attr


@attr.s(auto_attribs=True)
class WebSocketProps:
    """Defines driver properties for WebSocket connections."""

    host: str = '127.0.0.1'
    port: int = 8000
    ssl_context: Optional[ssl.SSLContext] = None
    message_queue_size: int = 1
    max_message_size: int = 1048576
    connect_timeout: float = 1.0
    disconnect_timeout: float = 1.0
    ping_interval: float = 1.0
    graceful_close_timeout: float = 0.1
