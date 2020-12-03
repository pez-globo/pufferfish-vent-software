"""Test the functionality of protocols.backend classes."""

import pytest as pt # type: ignore
import hypothesis as hp
import hypothesis.strategies as st
import betterproto

from ventserver.protocols import backend
from ventserver.protocols.protobuf import mcu_pb as pb

example_good = [
    (43.000230,pb.Alarms(alarm_one=True)),
    (35.065809,pb.SensorMeasurements(paw=20)),
    (58.719322,pb.CycleMeasurements(ve=300)),
    (58.719322,pb.Parameters(fio2=80)),
    (58.719322,pb.ParametersRequest(fio2=60)),
    (50.268176,pb.Ping(id=256)),
    (00.268176,pb.Announcement(announcement=b'hello!')),
]

example_frontend = [
    (43.000230,pb.Alarms(alarm_one=True),pb.Alarms(time=1234)),
    (35.065809,pb.SensorMeasurements(paw=20), pb.SensorMeasurements(flow=10)),
    (58.719322,pb.CycleMeasurements(ve=300), pb.CycleMeasurements(pip=20)),
    (58.719322,pb.Parameters(fio2=80), pb.Parameters(rr=10)),
    (58.719322,pb.ParametersRequest(fio2=60), pb.ParametersRequest(fio2=20)),
    (50.268176,pb.Ping(id=256),pb.Ping(id=255)),
    (00.268176,pb.Announcement(announcement=b'hello!'),
     pb.Announcement(announcement=b'hi')),
]

example_announcement = [
    (b'hello'),
    (b'hey'),
    (b'hi')
]

# Events

@hp.given(example_time=st.times())
def test_receive_event_has_data(example_time: float) -> None:
    """
    Scenario: Backend Receive Event behaves properly
    """
    # Given: A ReceiveEvent object
    receive_event = backend.ReceiveEvent()
    # When: no data is received
    # Then: has_data() is false
    assert receive_event.has_data() is False
    # When: A time event is recieved
    receive_event = backend.ReceiveEvent(time=example_time)
    # Then: has_data() is True
    assert receive_event.has_data() is True

@pt.mark.parametrize('time,payload', example_good)
def test_receive_mcu_event_has_data(
    time:float, payload: betterproto.Message) -> None:
    """
    Scenario: Backend Receive Event behaves properly
    """
    # Given: A Receive event object
    receive_event = backend.ReceiveEvent()
    # When: no data is received
    # Then: has_data() is false
    assert receive_event.has_data() is False
    # When: A serial event is recieved
    receive_event = backend.ReceiveEvent(time, payload)
    # Then: has_data is true
    assert receive_event.has_data() is True

@pt.mark.parametrize('_,payload', example_good)
def test_output_event_received(
    _: float, payload: betterproto.Message) -> None:
    """
    Scenario: backend recieves output event
    """
    # Given: A Output event object
    output_event = backend.OutputEvent()
    # When: no data is received
    # Then: has_data() is false
    assert output_event.has_data() is False

    # When: A output event is recieved
    output_event = backend.OutputEvent(payload)
    # Then: event has data
    assert output_event.has_data() is True

@pt.mark.parametrize('message', example_announcement)
def test_announcement_has_data(
    message: bytes
    ) -> None:
    """
    Scenario: backend sends announcement message
    """
    # Given: a announcement object
    announcement = backend.Announcement(message)
    # When: backend sends input event
    # Then: event has data
    assert announcement.has_data() is True

# FIlters

# Receive filter
@pt.mark.parametrize('time,payload', example_good)
def test_receive_filter_mcu_input(
    time: float, payload: betterproto.Message) -> None:
    """
    Scenario: Receive filter receives input event
    """
    # Given: a receive filter object
    receive_filter = backend.ReceiveFilter()
    # When: a input event is given to backend receive filter
    receive_event = backend.ReceiveEvent(time, payload)
    # Then: input has data
    receive_filter.input(receive_event)
    assert receive_event is not None
    assert receive_event.has_data() is True
    assert receive_event.mcu_receive is not None

@pt.mark.parametrize('time,mcu,frontend', example_frontend)
def test_filter_frontend_input(
    time: float, mcu: betterproto.Message,
    frontend: betterproto.Message) -> None:
    """
    Scenario: Receive filter receives input event
    """
    # Given: a receive filter object
    receive_filter = backend.ReceiveFilter()
    # When: a input event is given to backend receive filter
    receive_event = backend.ReceiveEvent(time, mcu, frontend)
    # Then: input has data
    receive_filter.input(receive_event)
    assert receive_event is not None
    assert receive_event.has_data() is True
    assert receive_event.frontend_receive is not None

@pt.mark.parametrize('time,payload', example_good)
def test_receive_filter_output(
    time: float, payload: betterproto.Message) -> None:
    """
    Scenario: Receive filter receives input event
    """
    # Given: a receive filter object
    receive_filter = backend.ReceiveFilter()
    # When: none event is given to backend receive filter
    output_event = backend.ReceiveEvent()
    receive_filter.input(output_event)
    assert receive_filter.output() is None

    # When: a input event is given to backend receive filter
    output_event = backend.ReceiveEvent(time, payload, payload, payload)
    receive_filter.input(output_event)
    # Then: output has data
    assert receive_filter.output() is not None

# Send filter
@pt.mark.parametrize('message', example_announcement)
def test_send_filter_output(
    message: bytes) -> None:
    """
    Scenario : an input is given to send filter
    """
    # Given: An announcement object
    announcement = backend.Announcement(message)
    send_filter = backend.SendFilter()
    # When: input is given
    send_filter.input(announcement)
    # Then: output event is not none
    assert send_filter.output() is not None

@pt.mark.parametrize('_,payload', example_good)
def test_send_filter_input(
    _: float, payload: betterproto.Message) -> None:
    """
    Scenario : an input is given to send filter
    """
    # Given: An output event object
    output_event = backend.OutputEvent()
    # When: no input is given
    send_filter = backend.SendFilter()
    send_filter.input(output_event)
    # Then: output event is none
    assert send_filter.output() is None

    # When: valid input is given
    output_event = backend.OutputEvent(payload)
    send_filter.input(output_event)
    # Then: output has data
    assert send_filter.output is not None

# Protocols
@pt.mark.parametrize('_,payload', example_good)
def test_get_mcu_send(
    _: float, payload: betterproto.Message) -> None:
    """
    Scenario: get_mcu_send function behaves properly
    """
    # Given: A output event
    # When: mcu output event is None
    # Then: output is none
    output_none = backend.OutputEvent()
    output = backend.get_mcu_send(output_none)
    assert output is None

    # When: mcu output event is passed
    # Then: output event has data
    output_event = backend.OutputEvent(payload)
    output = backend.get_mcu_send(output_event)
    assert output is not None
    assert output == payload

@pt.mark.parametrize('_,payload', example_good)
def test_get_frontend_send(
    _: float, payload: betterproto.Message) -> None:
    """
    Scenario: get_frontend_send function behaves properly
    """
    # Given: A output event
    # When: frontend output event is None
    # Then: output is none
    output_none = backend.OutputEvent()
    output = backend.get_frontend_send(output_none)
    assert output is None

    # When: frontend output event is passed
    # Then: frontend output event has data
    output_event = backend.OutputEvent(payload, payload)
    output = backend.get_frontend_send(output_event)
    assert output is not None
    assert output == payload

@pt.mark.parametrize('_,payload', example_good)
def test_get_file_send(
    _: float, payload: betterproto.Message) -> None:
    """
    Scenario: get_file_send function behaves properly
    """
    # Given: A output event
    # When: file output event is passed
    # Then: output is none
    output_none = backend.OutputEvent()
    output = backend.get_frontend_send(output_none)
    assert output is None

    # When: file output event is passed
    # then: output event has data
    output_event = backend.OutputEvent(payload, payload, payload)
    output = backend.get_file_send(output_event)
    assert output is not None
    assert output == payload
