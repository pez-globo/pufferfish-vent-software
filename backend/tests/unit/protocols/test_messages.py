"""Test the functionality of protocols.messages classes."""

from typing import Any

import betterproto

import pytest as pt  # type: ignore

from ventserver.protocols import application
from ventserver.protocols import exceptions
from ventserver.protocols import messages
from ventserver.protocols.protobuf import mcu_pb as pb


example_messages_good = [
    (1, pb.Alarms(alarm_one=True)),
    (2, pb.SensorMeasurements(paw=20)),
    (3, pb.CycleMeasurements(ve=300)),
    (4, pb.Parameters(fio2=80)),
    (5, pb.ParametersRequest(fio2=60)),
    (6, pb.Ping(id=256)),
    (7, pb.Announcement(announcement=b'hello!')),
]


example_message_bodies_bad = [
    b'',
    b'\02' + b'\31\14',
]


example_message_payloads_bad = [
    b'hello!',
    pb.VentilationMode.pc_ac,
]


@pt.mark.parametrize('type_code,payload', example_messages_good)
def test_msg_parse(type_code: int, payload: betterproto.Message) -> None:
    """Test Message parsing from body."""
    message = messages.Message()
    message.parse(
        bytes([type_code]) + bytes(payload), application.MCU_MESSAGE_CLASSES
    )
    assert message.type == type_code
    assert message.payload == payload


@pt.mark.parametrize('type_code,payload', example_messages_good)
def test_msg_parse_invalid_types(
        type_code: int, payload: betterproto.Message
) -> None:
    """Test Message parsing from body with a nonexistent type code."""
    message = messages.Message()
    with pt.raises(exceptions.ProtocolDataError):
        message.parse(
            bytes([0]) + bytes(payload), application.MCU_MESSAGE_CLASSES
        )
    try:
        message.parse(
            bytes([(type_code + 1) % len(application.MCU_MESSAGE_CLASSES) + 1])
            + bytes(payload), application.MCU_MESSAGE_CLASSES
        )
        assert message.payload != payload
    except exceptions.ProtocolDataError:
        pass


@pt.mark.parametrize('body', example_message_bodies_bad)
def test_msg_parse_invalid_bodies(body: bytes) -> None:
    """Test Message parsing from malformed body."""
    message = messages.Message()
    with pt.raises(exceptions.ProtocolDataError):
        message.parse(body, application.MCU_MESSAGE_CLASSES)


@pt.mark.parametrize('type_code,payload', example_messages_good)
def test_msg_generate(type_code: int, payload: betterproto.Message) -> None:
    """Test Message body generation."""
    message = messages.Message()
    message.type = type_code
    message.payload = payload
    assert message.body == bytes([type_code]) + bytes(payload)


@pt.mark.parametrize('type_code,payload', example_messages_good)
def test_msg_update(type_code: int, payload: betterproto.Message) -> None:
    """Test Message body generation."""
    message = messages.Message()
    message.payload = payload
    message.update_from_payload(application.MCU_MESSAGE_TYPES)
    assert message.type == type_code
    assert message.body == bytes([type_code]) + bytes(payload)


@pt.mark.parametrize('payload', example_message_payloads_bad)
def test_msg_generate_invalid(payload: Any) -> None:
    """Test Message body generation with invalid paylod."""
    message = messages.Message()
    message.payload = payload
    with pt.raises(exceptions.ProtocolDataError):
        message.update_from_payload(application.MCU_MESSAGE_TYPES)


@pt.mark.parametrize('type_code,payload', example_messages_good)
def test_msg_roundtrip(type_code: int, payload: betterproto.Message) -> None:
    """Test Message serialize/deserialize roundtrip."""
    generate_message = messages.Message()
    generate_message.type = type_code
    generate_message.payload = payload
    body = generate_message.body
    generate_message.parse(body, application.MCU_MESSAGE_CLASSES)
    assert generate_message.type == type_code
    assert generate_message.payload == payload
    parse_message = messages.Message()
    parse_message.parse(body, application.MCU_MESSAGE_CLASSES)
    assert parse_message.type == type_code
    assert parse_message.payload == payload
    assert parse_message.body == body


@pt.mark.parametrize('type_code,payload', example_messages_good)
def test_msg_rx(type_code: int, payload: betterproto.Message) -> None:
    """Test MessageReceiver behavior with specific examples."""
    receiver = messages.MessageReceiver(
        message_classes=application.MCU_MESSAGE_CLASSES
    )
    assert receiver.output() is None
    receiver.input(bytes([type_code]) + bytes(payload))
    assert receiver.output() == payload


@pt.mark.parametrize('_,payload', example_messages_good)
def test_msg_rx_invalid_types(_: int, payload: betterproto.Message) -> None:
    """Test MessageReceiver behavior with invalid typecode."""
    receiver = messages.MessageReceiver(
        message_classes=application.MCU_MESSAGE_CLASSES
    )
    assert receiver.output() is None
    receiver.input(bytes([0]) + bytes(payload))
    with pt.raises(exceptions.ProtocolDataError):
        receiver.output()


@pt.mark.parametrize('body', example_message_bodies_bad)
def test_msg_rx_invalid_bodies(body: Any) -> None:
    """Test MessageReceiver behavior with invalid body."""
    receiver = messages.MessageReceiver(
        message_classes=application.MCU_MESSAGE_CLASSES
    )
    assert receiver.output() is None
    receiver.input(body)
    with pt.raises(exceptions.ProtocolDataError):
        receiver.output()


@pt.mark.parametrize('type_code,payload', example_messages_good)
def test_msg_tx(type_code: int, payload: betterproto.Message) -> None:
    """Test MessageSender behavior with specific examples."""
    sender = messages.MessageSender(message_types=application.MCU_MESSAGE_TYPES)
    assert sender.output() is None
    sender.input(payload)
    assert sender.output() == bytes([type_code]) + bytes(payload)


@pt.mark.parametrize('payload', example_message_payloads_bad)
def test_msg_tx_invalid(payload: Any) -> None:
    """Test MessageSender behavior with invalid payload."""
    sender = messages.MessageSender(message_types=application.MCU_MESSAGE_TYPES)
    sender.input(payload)
    with pt.raises(exceptions.ProtocolDataError):
        sender.output()
