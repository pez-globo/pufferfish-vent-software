"""Test the functionality of protocols.server classes."""

from typing import Tuple
import pytest as pt # type: ignore
import hypothesis as hp
import hypothesis.strategies as st
import betterproto

from ventserver.protocols import server,backend,file
from ventserver.protocols.protobuf import mcu_pb as pb

example_messages_good = [
    (43.000230,pb.Alarms(alarm_one=True)),
    (35.065809,pb.SensorMeasurements(paw=20)),
    (58.719322,pb.CycleMeasurements(ve=300)),
    (58.719322,pb.Parameters(fio2=80)),
    (58.719322,pb.ParametersRequest(fio2=60)),
    (50.268176,pb.Ping(id=256)),
    (00.268176,pb.Announcement(announcement=b'hello!')),
]

example_event = [
    (
        43.000230,
        b'U\xa3\x13\xae\x04\x08\xdd\xe1\xce\xce\xcd.\x10\x06\x1d\x00\x00' +
        b'\xc0@%\x00\x00\xf0A-\x00\x00HC5\x00\x00\xf0A=\x00\x00\x00@E\x00' +
        b'\x00pBM\x00\x00\x08B', (0, True), 'Parameters'
    ),
    (
        35.065809,
        b'\xacO\xb2\x8c\x03\x08\x89\x99\xcf\xce\xcd.\x15\x00\x00zC\x1d' +
        b'\x00\x00\xc8A%\x00\x00pA-\x00\x00 A5\x00\x00\xc0@=\x00\x00pA',
        (32, False), 'CycleMeasurements'
    ),
    (
        58.719322,
        b'\xff\r\xca<\x05\x08\x90\xf5\xce\xce\xcd.\x10\x06\x1d\x00\x00' +
        b'\xe0@%\x00\x00\xa0A-\x00\x00\x96C5\x00\x00\xa0B=\x00\x00@@E' +
        b'\x00\x00HBM\x00\x00\xa0A',(29, True), 'ParametersRequest'
    ),
    (
        58.719322, b'/3Z\xf6\x01\x08\xe9\xc0\xe1\xce\xcd.\x10\x01',
        (55, True), 'Alarms'
    ),
    (
        58.719322,
        b'%W\xb29\x02\x08\xe9\xc0\xe1\xce\xcd.\x10\n\x1d\x00\x00\xa0A%' +
        b'\x00\x00\xc8A-\x00\x00HB5\x00\x00HB=\x00\x00\xc2B',
        (45, True), 'SensorMeasurements'
    )
]

#  Server
@hp.given(example_time=st.times())
@hp.example(example_time='13:34:52.681276')
@hp.example(example_time='13:52:52.681276')
@hp.example(example_time='01:52:52.681276')
@hp.example(example_time='16:37:41.745141')
@hp.example(example_time='16:37:00.745141')
@hp.example(example_time='16:37:00.002910')
def test_frontend_connection_event(example_time: float) -> None:
    """
    Scenario: check if server connects to frontend
    """
    # Given: Frontendconnection event object
    # When: event is passed at some time
    frontend_connection = server.FrontendConnectionEvent(example_time, True)
    # Then: frontend connection has data
    assert frontend_connection.has_data() is True

@pt.mark.parametrize('example_time, data, rotary, state_type', example_event)
def test_receive_event(
    example_time:float, data: bytes,
    rotary: Tuple[int, bool],
    state_type: str) -> None:
    """
    Scenario: check if receive event has data
    """
    # Given: A ReceiveRvent object
    # When: test data is given to it
    filedata = file.StateData(
        data=data,
        state_type=state_type
    )
    receive_event = server.ReceiveEvent(
        example_time, data, data, rotary, filedata)
    print(receive_event)
    # Then: receive event has data
    assert receive_event.has_data() is True

@pt.mark.parametrize('_,payload', example_messages_good)
def test_receive_output_event(
    _: float, payload: betterproto.Message) -> None:
    """
    Scenario: check if receive output event has data
    """
    # Given: A backend output event object
    backend_output_event = backend.OutputEvent(payload)
    # When: backend output event is passed to ReceiveOutputEvent
    server_output = server.ReceiveOutputEvent(backend_output_event, False)
    # Then: server output has data
    assert server_output.has_data() is True

@hp.given(data=st.binary(min_size=0x00000000, max_size=0xffffffffff))
@hp.example(data=b'\x00\x00\x00\x00')
@hp.example(data=b'\xff\xff\xff\xff')
def test_send_output_event_has_data(data: bytes) -> None:
    """
    Scenario: check if server output event has data
    """
    # Given: A SendOutputEvent object
    # When: data is written to it
    send_output_event = server.SendOutputEvent(data,data)
    # Then: server output event has data
    assert send_output_event.has_data() is True
    assert send_output_event.websocket_send is not None

@hp.given(data=st.binary(
    min_size=0x00000000, max_size=0xffffffffff),
    example_time=st.times())
@hp.example(data=b'\x00\x00\x00\x00',example_time='13:34:52.681276')
@hp.example(data=b'\xff\xff\xff\xff',example_time='13:52:52.681276')
# @hp.example(example_time='01:52:52.681276')
def test_serial_receive(data: bytes, example_time: float) -> None:
    """
    Scenario: Check if make serial receive gives receive event object
    """
    # Given: example data and times
    # When: data is written to it
    serial_receive = server.make_serial_receive(data, example_time)
    # Then: receive event is not None
    assert serial_receive is not None
    assert isinstance(serial_receive, server.ReceiveEvent)

@hp.given(data=st.binary(
    min_size=0x00000000, max_size=0xffffffffff),
    example_time=st.times())
@hp.example(data=b'\x00\x00\x00\x00',example_time='13:34:52.681276')
@hp.example(data=b'\x12\x23\x12\x45',example_time='13:34:52.681276')
@hp.example(data=b'\xff\xff\xff\xff',example_time='13:52:52.681276')
# @hp.example(example_time='01:52:52.681276')
def test_websocket_receive(data: bytes, example_time: float) -> None:
    """
    Scenario: websocket receive behaves properly
    """
    # Given: example data and time
    # When: data is written to it
    websocket_receive = server.make_websocket_receive(data, example_time)
    # Then: receive event is not None
    assert websocket_receive is not None
    assert isinstance(websocket_receive, server.ReceiveEvent)

@hp.given(data=st.tuples(st.integers(), st.booleans()),
    example_time=st.times())
@hp.example(data=(1, True), example_time='13:34:52.681276')
def test_rotary_encoder_receive(
    data: Tuple[int, bool],
    example_time: float) -> None:
    """
    Scenario: rotary encoder receive behaves properly
    """
    # Given: example data and time
    # When: data is written to it
    rotary_encoder_receive = \
        server.make_rotary_encoder_receive(data, example_time)
    # Then: output is not none
    assert rotary_encoder_receive is not None
    assert isinstance(rotary_encoder_receive, server.ReceiveEvent)

# Filters

@pt.mark.parametrize('example_time, data, rotary, state_type', example_event)
def test_receive_filter_output(
    example_time:float, data: bytes,
    rotary: Tuple[int, bool],
    state_type: str) -> None:
    """
    Scenario: Receive Filter behaves properly
    """
    # Given: A receive event object or FrontEndConnection event object
    filedata = file.StateData(
        data=data,
        state_type=state_type
    )
    receive_event = server.ReceiveEvent(
        example_time, data, data, rotary, filedata)
    # When: No event is passed to the filter
    recieve_filter = server.ReceiveFilter()
    # When: event is passed to the filter
    recieve_filter.input(receive_event)
    output = recieve_filter.output()

    # Then: output is of type ReceiveOutputvent
    assert output is not None
    assert isinstance(output, server.ReceiveOutputEvent)

@pt.mark.parametrize('_,payload', example_messages_good)
def test_send_filter(
    _: float, payload: betterproto.Message) -> None:
    """
    Scenario: Send Filter behaves properly
    """
    # Given: Send Filter object
    server_send_filter = server.SendFilter()
    # When: backend output event object is given to it
    backend_output_event = backend.OutputEvent(payload)
    server_send_filter.input(backend_output_event)

    # Then: output is of type SendOutputEvent
    output = server_send_filter.output()

    assert output is not None
    assert isinstance(output, server.SendOutputEvent)
