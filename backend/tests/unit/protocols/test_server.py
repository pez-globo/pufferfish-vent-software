import time

import hypothesis as hp
import hypothesis.strategies as st

from ventserver.protocols import server


@hp.given(example_time=st.times())
def test_frontend_connection_has_data(example_time: float) -> None:
    """Test Frontend Connection."""
    frontend_connection = server.FrontendConnectionEvent(last_connection_time=example_time,
                                                         is_frontend_connected=True)
    assert frontend_connection.has_data() is True

    # TODO: check if has_data handles boolean check for is_frontend_connected correctly
    # frontend_connection = server.FrontendConnectionEvent(last_connection_time=None, is_frontend_connected=None)
    # assert frontend_connection.has_data() is False


@hp.given(example_time=st.times())
def test_receive_events_has_data(example_time: float) -> None:
    """Test Receive Events."""
    receive_event = server.ReceiveEvent()
    assert receive_event.has_data() is False

    receive_event = server.ReceiveEvent(time=example_time)
    assert receive_event.has_data() is True


@hp.given(
    example_bytes=st.binary(max_size=32),
    example_time=st.times(),
)
def test_make_serial_receive(example_bytes: bytes, example_time: float) -> None:
    assert server.make_serial_receive(example_bytes, example_time) == server.ReceiveEvent(serial_receive=example_bytes,
                                                                                          time=example_time)


@hp.given(
    example_bytes=st.binary(max_size=32),
    example_time=st.times(),
)
def test_make_websocket_receive(example_bytes: bytes, example_time: float) -> None:
    assert server.make_websocket_receive(example_bytes, example_time) == server.ReceiveEvent(
        websocket_receive=example_bytes,
        time=example_time)


@hp.given(
    example_int=st.integers(),
    example_bool=st.booleans(),
    example_time=st.times(),
)
def test_make_rotary_encoder_receive(example_int: int, example_time: float, example_bool: bool) -> None:
    assert server.make_rotary_encoder_receive((example_int, example_bool), example_time) == server.ReceiveEvent(
        rotary_encoder_receive=(example_int, example_bool),
        time=example_time)
