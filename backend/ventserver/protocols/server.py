"""Sans-I/O ventilator backend server protocol."""

from typing import Optional, Union, Tuple
import logging
import time as _time

import attr

from ventserver.protocols import backend
from ventserver.protocols import events
from ventserver.protocols import frontend
from ventserver.protocols import mcu
from ventserver.protocols import file
from ventserver.protocols import rotary_encoder
from ventserver.protocols import exceptions
from ventserver.sansio import channels
from ventserver.sansio import protocols


# Events

@attr.s
class FrontendConnectionEvent(events.Event):
    """Server frontend connection status event."""

    last_connection_time: float = attr.ib(default=None)
    is_frontend_connected: bool = attr.ib(default=False)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return (
            self.last_connection_time is not None
            and self.is_frontend_connected is not None
        )

@attr.s
class ReceiveEvent(events.Event):
    """Server receive input event."""

    time: Optional[float] = attr.ib(default=None)
    serial_receive: Optional[bytes] = attr.ib(default=None)
    websocket_receive: Optional[bytes] = attr.ib(default=None)
    rotary_encoder_receive: Tuple[int, bool] = attr.ib(default=None)
    file_receive: Optional[file.StateData] = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return (
            self.time is not None
            or bool(self.serial_receive)
            or self.websocket_receive is not None
            or self.rotary_encoder_receive is not None
            or self.file_receive is not None
        )


@attr.s
class ReceiveOutputEvent(events.Event):
    """Server receive output/send event."""

    server_send: Optional[backend.OutputEvent] = attr.ib(default=None)
    frontend_delayed: bool = attr.ib(default=False)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return self.server_send is not None and self.server_send.has_data()


SendEvent = Union[backend.Announcement, backend.OutputEvent]


@attr.s
class SendOutputEvent(events.Event):
    """Server send output/send event."""

    serial_send: Optional[bytes] = attr.ib(default=None)
    websocket_send: Optional[bytes] = attr.ib(default=None)
    file_send: Optional[file.StateData] = attr.ib(default=None)

    def has_data(self) -> bool:
        """Return whether the event has data."""
        return (bool(self.serial_send) or
            self.websocket_send is not None or
            self.file_send is not None)


def make_serial_receive(
        serial_receive: bytes,
        time: float
    ) -> ReceiveEvent:
    """Make a ReceiveEvent from serial receive data."""
    return ReceiveEvent(serial_receive=serial_receive, time=time)


def make_websocket_receive(
        ws_receive: bytes,
        time: float
    ) -> ReceiveEvent:
    """Make a ReceiveEvent from websocket receive data."""
    return ReceiveEvent(websocket_receive=ws_receive, time=time)

def make_rotary_encoder_receive(
        re_receive: Tuple[int, bool],
        time: float
    ) -> ReceiveEvent:
    """Make a ReceiveEvent from rotary encoder receive data."""
    return ReceiveEvent(rotary_encoder_receive=re_receive, time=time)


# Frontend kill props

@attr.s
class FrontendKillProps():
    """Variables used to implement frozen frontend kill logic."""

    # fe = frontend
    last_fe_event: float = attr.ib(default=0)
    fe_connected: bool = attr.ib(default=False)
    fe_connection_time: float = attr.ib(default=0)
    last_fe_kill: float = attr.ib(factory=_time.time)


# Filters


@attr.s
class ReceiveFilter(protocols.Filter[ReceiveEvent, ReceiveOutputEvent]):
    """Filter which transforms receive bytes into high-level events."""
    _logger = logging.getLogger('.'.join((__name__, 'ReceiveFilter')))

    _buffer: channels.DequeChannel[
                        Union[ReceiveEvent, FrontendConnectionEvent]
            ] = attr.ib(factory=channels.DequeChannel)

    current_time: float = attr.ib(default=0)
    _kill_props: FrontendKillProps = attr.ib(factory=FrontendKillProps)

    _mcu: mcu.ReceiveFilter = attr.ib(factory=mcu.ReceiveFilter)
    _frontend: frontend.ReceiveFilter = attr.ib(factory=frontend.ReceiveFilter)
    _rotary_encoder: rotary_encoder.ReceiveFilter = attr.ib(
        factory=rotary_encoder.ReceiveFilter
    )
    _backend: backend.ReceiveFilter = attr.ib(factory=backend.ReceiveFilter)
    _file: file.ReceiveFilter = attr.ib(
        factory=file.ReceiveFilter
    )


    def input(self, event: Optional[
                            Union[ReceiveEvent, FrontendConnectionEvent
                            ]
    ]) -> None:
        """Handle input events."""
        if event is None or not event.has_data():
            return

        self._buffer.input(event)

    def output(self) -> Optional[ReceiveOutputEvent]:
        """Emit the next output event."""
        self._process_buffer()
        any_updated = False

        # Process mcu output
        any_updated = self._process_mcu() or any_updated
        # Process frontend output
        any_updated = self._process_frontend() or any_updated
        # Process rotary encoder output
        any_updated = self._process_rotary_encoder() or any_updated
        # Process file output
        try:
            any_updated = self._process_file() or any_updated
        except exceptions.ProtocolDataError as err:
            self._logger.error(err)
        # Process time
        if not any_updated:
            self._backend.input(backend.ReceiveEvent(time=self.current_time))

        # Process backend output
        # Consume any outputs as long as the backend is indicating that it still
        # has receive data to process, even if it has no data to output
        while True:
            backend_output = self._backend.output()
            if backend_output is None:
                break
            if backend_output.has_data():
                break
        any_updated = any_updated or backend_output is not None
        if not any_updated:
            return None

        # Kill frontend process if it stops responding.
        # The frontend service will automatically restart the frontend process.
        delayed = False
        if int(self.current_time - self._kill_props.last_fe_event) > 1:
            if int(self.current_time - self._kill_props.last_fe_kill) > 2:
                connection_duration = int(
                    self.current_time - self._kill_props.fe_connection_time
                )
                if self._kill_props.fe_connected and connection_duration > 2:
                    self._kill_props.last_fe_kill = self.current_time
                    delayed = True


        output = ReceiveOutputEvent(
            server_send=backend_output, frontend_delayed=delayed
        )
        return output

    def _process_buffer(self) -> None:
        """Process the next event in the input buffer."""
        event = self._buffer.output()
        if event is None:
            return

        if isinstance(event, FrontendConnectionEvent):
            self._kill_props.fe_connection_time = event.last_connection_time
            self._kill_props.fe_connected = event.is_frontend_connected
            return

        if event.time is not None:
            self.current_time = event.time
        self._mcu.input(event.serial_receive)
        self._frontend.input(event.websocket_receive)
        self._rotary_encoder.input(
            rotary_encoder.ReceiveEvent(
                time=self.current_time,
                re_data=event.rotary_encoder_receive
            )
        )
        self._file.input(event.file_receive)


    def _process_mcu(self) -> bool:
        """Process the next event from the mcu protocol."""
        mcu_output = self._mcu.output()
        if mcu_output is None:
            return False

        self._backend.input(backend.ReceiveEvent(
            time=self.current_time, mcu_receive=mcu_output,
            frontend_receive=None
        ))
        return True

    def _process_frontend(self) -> bool:
        """Process the next event from the frontend protocol."""
        frontend_output = self._frontend.output()
        if frontend_output is None:
            return False
        self._backend.input(backend.ReceiveEvent(
            time=self.current_time, mcu_receive=None,
            frontend_receive=frontend_output
        ))
        self._kill_props.last_fe_event = self.current_time
        return True

    def _process_rotary_encoder(self) -> bool:
        """Process the next event from the rotary encoder."""
        rotary_encoder_output = self._rotary_encoder.output()
        if rotary_encoder_output is None:
            return False
        self._backend.input(backend.ReceiveEvent(
            time=self.current_time, mcu_receive=None,
            frontend_receive=rotary_encoder_output
        ))
        return True

    def _process_file(self) -> bool:
        """Process the next event from the file."""
        file_output = self._file.output() # throws ProtocolDataError
        if file_output is None:
            return False
        self._backend.input(backend.ReceiveEvent(
            time=self.current_time, mcu_receive=None,
            frontend_receive=None, file_receive=file_output
        ))
        return True


    def input_serial(self, serial_receive: bytes) -> None:
        """Input a ReceiveEvent corresponding to serial data.

        This is just a convenience function intended for writing unit tests
        more concisely.
        """
        self.input(make_serial_receive(serial_receive, self.current_time))

    def input_websocket(self, websocket: bytes) -> None:
        """Input a ReceiveEvent corresponding to websocket data.

        This is just a convenience function intended for writing unit tests
        more concisely.
        """
        self.input(make_websocket_receive(websocket, self.current_time))

    @property
    def backend(self) -> backend.ReceiveFilter:
        """Return the backend receiver."""
        return self._backend

    @property
    def file(self) -> file.ReceiveFilter:
        """Return the file receiver"""
        return self._file



@attr.s
class SendFilter(protocols.Filter[SendEvent, SendOutputEvent]):
    """Filter which transforms high-level events into send bytes."""

    _buffer: channels.DequeChannel[SendEvent] = attr.ib(
        factory=channels.DequeChannel
    )
    _backend: backend.SendFilter = attr.ib(factory=backend.SendFilter)
    _mcu: mcu.SendFilter = attr.ib(factory=mcu.SendFilter)
    _frontend: frontend.SendFilter = attr.ib(factory=frontend.SendFilter)
    _file: file.SendFilter = attr.ib(factory=file.SendFilter)

    def input(self, event: Optional[SendEvent]) -> None:
        """Handle input events."""
        if event is None or not event.has_data():
            return

        self._buffer.input(event)

    def output(self) -> Optional[SendOutputEvent]:
        """Emit the next output event."""
        any_updated = False
        self._process_buffer()
        backend_output = self._backend.output()
        any_updated = (backend_output is not None) or any_updated

        self._mcu.input(backend.get_mcu_send(backend_output))
        mcu_output = self._mcu.output()
        any_updated = (mcu_output is not None) or any_updated

        self._frontend.input(backend.get_frontend_send(backend_output))
        frontend_output = self._frontend.output()
        any_updated = (frontend_output is not None) or any_updated

        self._file.input(backend.get_file_send(backend_output))
        file_output = self._file.output()
        any_updated = (file_output is not None) or any_updated

        if not any_updated:
            return None

        output = SendOutputEvent(
            serial_send=mcu_output, websocket_send=frontend_output,
            file_send=file_output
        )
        return output

    def _process_buffer(self) -> None:
        """Process the next event in the input buffer."""
        try:
            event = self._buffer.output()
            self._backend.input(event)
        except IndexError:
            pass

    @property
    def file(self) -> file.SendFilter:
        """Return file sendfilter"""
        return self._file


# Protocols


@attr.s
class Protocol(protocols.Protocol[
        ReceiveEvent, ReceiveOutputEvent, SendEvent, SendOutputEvent
]):
    """Backend communication protocol."""

    _receive: ReceiveFilter = attr.ib(factory=ReceiveFilter)
    _send: SendFilter = attr.ib(factory=SendFilter)

    @property
    def receive(self) -> ReceiveFilter:
        """Return a Filter interface for receive events."""
        return self._receive

    @property
    def send(self) -> SendFilter:
        """Return a Filter interface for send events."""
        return self._send
