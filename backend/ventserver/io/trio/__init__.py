"""Modules for trio I/O endpoint drivers and execution synchronizers.

Modules:
    endpoints: abstract interface for I/O endpoint drivers.
    _serial: I/O endpoint driver for serial connections.
    websocket: I/O endpoint driver for WebSocket server connections to clients.
    channels: support for trio MemoryChannels in a Filter-like interface.

"""
