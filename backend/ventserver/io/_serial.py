"""Generic support for serial connections."""

from typing import Optional

import attr

from serial.tools import list_ports  # type: ignore


def find_port(device_filter: str) -> str:
    """Find a port based on the given filter."""
    for port in list_ports.comports():
        if device_filter in port.description:
            return str(port.device)
    try:
        next_port: str = next(list_ports.grep(device_filter))
        return next_port
    except StopIteration:
        pass

    raise IOError(f'No {device_filter} ports found.')


@attr.s(auto_attribs=True)
class SerialProps:
    """Defines driver properties for serial devices."""

    port: str = ''
    port_filter: str = ''
    baud_rate: int = 115200
    message_length: int = 4096
    message_delimiter: bytes = b'\0'
    open_delay: float = 1.0
    read_timeout: Optional[float] = None
    write_timeout: Optional[float] = None
