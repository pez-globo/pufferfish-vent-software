import hypothesis as hp
import hypothesis.strategies as st

from ventserver.protocols import exceptions
from ventserver.protocols.backend import OutputEvent
from ventserver.protocols import backend
from typing import Dict, Optional, Type
import betterproto
from ventserver.protocols import frontend

data = Dict[Type[betterproto.Message], Optional[betterproto.Message]]


@hp.given(example_time=st.times())
def test_receive_event_has_data(example_time: float) -> None:
    receive_event = backend.ReceiveEvent()
    assert receive_event.has_data() is False

    receive_event = backend.ReceiveEvent(time=example_time)
    assert receive_event.has_data() is True


@hp.given(example_time=st.times())
def test_output_event_has_data(example_time: float) -> None:
    output_event = backend.OutputEvent()
    assert output_event.has_data() is False

    output_event = backend.OutputEvent(mcu_send=betterproto.Message())
    assert output_event.has_data() is True


def test_announcement_has_data() -> None:
    # TODO: check if this is right, or message value nin announcement needs to be checked
    announcement = backend.Announcement()
    assert announcement.has_data() is True


def test_init_all_states() -> None:
    receive_filter = backend.ReceiveFilter()
    assert receive_filter.init_all_states() == {value_type: None for value_type in frontend.MESSAGE_CLASSES.values()}


def test_get_file_send() -> None:
    output_event = backend.OutputEvent()
    assert backend.get_file_send(output_event) == output_event.file_send


def test_get_frontend_send() -> None:
    output_event = backend.OutputEvent()
    assert backend.get_frontend_send(output_event) == output_event.frontend_send


def test_get_mcu_send() -> None:
    output_event = backend.OutputEvent()
    assert backend.get_mcu_send(output_event) == output_event.mcu_send


def test_output() -> None:
    receive_filter = backend.ReceiveFilter()
    # mcu_send = None
    # try:
    #     mcu_send = receive_filter._mcu_state_synchronizer.output()
    # except exceptions.ProtocolDataError:
    #     receive_filter._logger.exception('MCU State Synchronizer:')
    # frontend_send = None
    # try:
    #     frontend_send = receive_filter._frontend_state_synchronizer.output()
    # except exceptions.ProtocolDataError:
    #     receive_filter._logger.exception('Frontend State Synchronizer:')
    # file_send = None
    # try:
    #     file_send = receive_filter._file_state_synchronizer.output()
    # except exceptions.ProtocolDataError:
    #     receive_filter._logger.exception('File State Synchronizer:')

    # TODO: check how to test synchronisation in unit test
    assert receive_filter.output() is None