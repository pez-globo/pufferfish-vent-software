"""Test the functionality of protocols.backend classes."""

import pytest as pt  # type: ignore
import hypothesis as hp
import hypothesis.strategies as st
import betterproto

from ventserver.protocols import backend
from ventserver.protocols.protobuf import mcu_pb as pb

example_good = [
    (pb.SensorMeasurements(paw=20)),
    (pb.CycleMeasurements(ve=300)),
    (pb.Parameters(fio2=80)),
    (pb.ParametersRequest(fio2=60)),
    (pb.AlarmLimits(fio2=pb.Range(lower=21, upper=100))),
    (pb.AlarmLimitsRequest(fio2=pb.Range(lower=21, upper=100))),
    (pb.Ping(id=256)),
    (pb.Announcement(announcement=b'hello!')),
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
    Scenario: Backend receive event behaves properly
    """
    # Given: A ReceiveEvent object
    # When: the object is initialised without data
    receive_event = backend.ReceiveEvent()
    # Then: has_data function returns false
    assert receive_event.has_data() is False
    # And When: time is given as input
    receive_event = backend.ReceiveEvent(time=example_time)
    # Then: has_data function returns true
    assert receive_event.has_data() is True


@hp.given(time=st.floats(min_value=0, max_value=10, allow_nan=False))
@pt.mark.parametrize('payload', example_good)
def test_receive_mcu_event_has_data(
        time: float, payload: betterproto.Message) -> None:
    """
    Scenario: Backend receive event behaves properly
    """
    # Given: A ReceiveEvent object
    # When: mcu event is given as input
    receive_event = backend.ReceiveEvent(time, payload)
    # Then: has_data function returns true
    assert receive_event.has_data() is True
    # And When: frontend data is given as input
    receive_event = backend.ReceiveEvent(time, payload, payload)
    # Then: has_data function returns true
    assert receive_event.has_data() is True


@pt.mark.parametrize('payload', example_good)
def test_output_event_received(
        payload: betterproto.Message) -> None:
    """
    Scenario: The backend's output event behaves properly
    """
    # Given: A Output event object
    output_event = backend.OutputEvent()
    # When: no data is received
    # Then: has_data() is false
    assert output_event.has_data() is False

    # And When: A output event is recieved
    output_event = backend.OutputEvent(payload)
    # Then: event has data
    assert output_event.has_data() is True


@pt.mark.parametrize('message', example_announcement)
def test_announcement_has_data(
    message: bytes
) -> None:
    """
    Scenario: The backend's output event behaves properly
    """
    # Given: a announcement object
    # When: backend recieves announcement strings as input
    announcement = backend.Announcement(message)
    # Then: has_data function returns true
    assert announcement.has_data() is True

# FIlters

# Receive filter


@hp.given(time=st.floats(min_value=0, max_value=10, allow_nan=False))
@pt.mark.parametrize('payload', example_good)
def test_receive_filter_mcu_input(
        time: float, payload: betterproto.Message) -> None:
    """
    Scenario: Receive filter receives input event
    """
    # Given: mcu receive event
    receive_filter = backend.ReceiveFilter()
    receive_event = backend.ReceiveEvent(time, payload)
    # When: mcu receive event is passed as input to receive filter
    receive_filter.input(receive_event)
    # Then: output of receive filter is not None
    assert receive_filter.output() is not None


@hp.given(time=st.floats(min_value=0, max_value=10, allow_nan=False))
@pt.mark.parametrize('mcu', example_good)
def test_filter_frontend_input(
        time: float, mcu: betterproto.Message) -> None:
    """
    Scenario: Receive filter receives input event
    """
    # Given: frontend receive event
    receive_filter = backend.ReceiveFilter()
    receive_event = backend.ReceiveEvent(time, mcu, mcu)
    # When: frontend receive event is passed as input to receive filter
    receive_filter.input(receive_event)
    receive_filter.input(receive_event)
    # Then: output of receive filter is not None
    assert receive_filter.output() is not None


@hp.given(time=st.floats(min_value=0, max_value=10, allow_nan=False))
@pt.mark.parametrize('payload', example_good)
def test_receive_filter_output(
        time: float, payload: betterproto.Message) -> None:
    """
    Scenario: Receive filter receives input event
    """
    # Given: a `ReceiveFilter` object which has not yet received any events
    receive_filter = backend.ReceiveFilter()
    # When: a ReceiveEvent with no data is passed as input to the receive filter
    output_event = backend.ReceiveEvent()
    receive_filter.input(output_event)
    # Then: the output of the receive filter is None
    assert receive_filter.output() is None

    # Given: file object receive event
    output_event = backend.ReceiveEvent(time, payload, payload, payload)
    # When: file receive event is passed as input to receive filter
    receive_filter.input(output_event)
    # Then: output of receive filter is not None
    assert receive_filter.output() is not None

# Send filter


@pt.mark.parametrize('message', example_announcement)
def test_send_filter_output(
        message: bytes) -> None:
    """
    Scenario : Send Filter behaves properly
    """
    # Given: An event of type announcement
    announcement = backend.Announcement(message)
    # When: announcement event is passed as input to send filter
    send_filter = backend.SendFilter()
    send_filter.input(announcement)
    # Then: output of send filter is Not None
    assert send_filter.output() is not None


@pt.mark.parametrize('payload', example_good)
def test_send_filter_input(
        payload: betterproto.Message) -> None:
    """
    Scenario : an input is given to send filter
    """
    # Given: An Event of type SendEvent with no data
    output_event = backend.OutputEvent()
    # When: empty send event is passed as input to send filter
    send_filter = backend.SendFilter()
    send_filter.input(output_event)
    # Then: output of send filter is None
    assert send_filter.output() is None

    # Given: An Event of type SendEvent with data
    output_event = backend.OutputEvent(payload)
    # When: Given event is passed as input to send filter
    send_filter.input(output_event)
    # Then: output of send filter is not None
    assert send_filter.output is not None

# Protocols


@pt.mark.parametrize('payload', example_good)
def test_get_mcu_send(
        payload: betterproto.Message) -> None:
    """
    Scenario: get_mcu_send function behaves properly
    """
    # Given: The get_mcu_send function
    output_event = backend.OutputEvent(payload)
    # When: OutputEvent object containing data in mcu_send field is passed
    output = backend.get_mcu_send(output_event)
    # Then:  output of the function is not None
    assert output is not None
    assert output == payload


@pt.mark.parametrize('payload', example_good)
def test_get_frontend_send(
        payload: betterproto.Message) -> None:
    """
    Scenario: get_frontend_send function behaves properly
    """
    # Given: The get_frontend_send function
    output_event = backend.OutputEvent(payload, payload)
    # When:
    # OutputEvent object containing data in frontend_send field is passed
    output = backend.get_frontend_send(output_event)
    # Then: output of the function is not None
    assert output is not None
    assert output == payload


@pt.mark.parametrize('payload', example_good)
def test_get_file_send(
        payload: betterproto.Message) -> None:
    """
    Scenario: get_file_send function behaves properly
    """
    # Given: The get_file_send function
    output_event = backend.OutputEvent(payload, payload, payload)
    # When:
    # OutputEvent object containing data in file_send field is passed
    output = backend.get_file_send(output_event)
    # then: output of the function is not None
    assert output is not None
    assert output == payload
