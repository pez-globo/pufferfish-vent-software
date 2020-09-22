"""Test functunality of protocols.crcelements classes"""

from typing import List, Tuple
import hypothesis as hp
import hypothesis.strategies as st

import pytest as pt  # type: ignore

from ventserver.protocols import crcelements
from ventserver.protocols import exceptions

example_crc_good = [
    (
        b'\xf1aw\xd2', b'\x00\x00', b'\xf1\x61\x77\xd2\x00\x00'
    ),
    (
        b'\x98\xdb\xe3U', b'\x01\x05\x01\x02\x03\x04\x05',
        b'\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05'
    )
]
example_crc_bad = [
    (
        b'\x124Vx', b'\x03\x04\x00\xed\x30\x00',
        b'\x12\x34\x56\x78\x03\x04\x00\xed\x30\x00'
    ),
    (
        b'\x03\x12#E', b'\x01\x05\x09\x08\x0a\x02\x0a',
        b'\x03\x12\x23\x45\x01\x05\x09\x08\x0a\x02\x0a'
    )
]
example_crc = example_crc_good + example_crc_bad

# CRCElement

@hp.given(crc=st.binary(min_size=0x00000000, max_size=0xffffffffff))
@hp.example(crc=b'\x00\x00\x00\x00')
@hp.example(crc=b'\xff\xff\xff\xff')
@hp.example(crc=b'\xff\xff\xff\xff' + b'\x00\x00\x00\x01')
def test_crcelement_init_crc(crc: bytes) -> None:
    """Test CRCElement crc attr initialization."""
    if (b'\x00\x00\x00\x00' <= crc <= b'\xff\xff\xff\xff') and (len(crc) == 4):
        crcelements.CRCElement(crc=crc)
    else:
        with pt.raises(ValueError):
            crcelements.CRCElement(crc=crc)


@pt.mark.parametrize('payload,expected', [
    # From the catalogue of parameterized CRC algorithms
    (b'123456789', b'\xe3\x06\x92\x83'),
    # Calculated using the Sunshine Online CRC Calculator
    (b'', b'\x00\x00\x00\x00'),
    (b'\x00', b'\x52\x7d\x53\x51'),
    (b'\x01', b'\xa0\x16\xd0\x52')
])
def test_crcelement_compute_crc(
        payload: bytes, expected: bytes
) -> None:
    """Test CRCElement CRC-32C computation."""
    crc_element = crcelements.CRCElement()
    crc_element.payload = payload
    assert crc_element._compute_crc() == expected   # pylint: disable=protected-access

# disabling pylint for longer name
@pt.mark.parametrize('payload', ['123456789', 123456789, 0x75bcd15])
def test_crcelement_invalid_crc_input(payload: bytes) -> None:  # pylint: disable=invalid-name
    """Test CRCElement CRC-32C invalid input."""
    crc_element = crcelements.CRCElement()
    crc_element.payload = payload
    with pt.raises(exceptions.ProtocolDataError):
        _ = crc_element._compute_crc()  # pylint: disable=protected-access


@pt.mark.parametrize('crc,payload,_', example_crc_good)
def test_crcelement_integrity_pass(
        crc: bytes, payload: bytes, _: bytes
) -> None:
    """Test CRCElement data integrity pass check."""
    crc_element = crcelements.CRCElement()
    crc_element.crc = crc
    crc_element.payload = payload
    try:
        crc_element.check_integrity()
    except exceptions.ProtocolDataError as err:
        pt.fail("Integrity check failed: {0}".format(err))


@pt.mark.parametrize('crc,payload,_', example_crc_bad)
def test_crcelement_integrity_fail(
        crc: bytes, payload: bytes, _: bytes
) -> None:
    """Test CRCElement data integrity fail check."""
    crc_element = crcelements.CRCElement()
    crc_element.crc = crc
    crc_element.payload = payload
    with pt.raises(exceptions.ProtocolDataError):
        crc_element.check_integrity()


@pt.mark.parametrize('crc,payload,body', example_crc)
def test_crcelement_parse(
        crc: bytes, payload: bytes, body: bytes
) -> None:
    """Test CRCElement parsing from body."""
    crc_element = crcelements.CRCElement()
    crc_element.parse(body)
    assert crc_element.crc == crc
    assert crc_element.payload == payload


# disabling pylint for longer name
@hp.given(
    crc=st.binary(min_size=4, max_size=4),
    payload=st.binary(max_size=252)
)
@hp.example(crc=b'\x00\x00\x00\x00', payload=b'')
@hp.example(
    crc=b'\xff\xff\xff\xff',
    payload=bytes(list(range(252)))     # pylint: disable=invalid-name
)
def test_crcelement_parse_invariants(
        crc: bytes, payload: bytes
) -> None:
    """Test CRCElement parse invariants."""
    crc_element = crcelements.CRCElement()
    crc_element.parse(crc + payload)
    assert crc_element.crc == crc
    assert crc_element.payload == payload


@hp.given(body=st.binary(max_size=3))
def test_crcelement_parse_invalids(body: bytes)-> None:
    """Test CRCElement parse invalids."""
    crc_element = crcelements.CRCElement()
    with pt.raises(exceptions.ProtocolDataError):
        crc_element.parse(body)


@pt.mark.parametrize('crc,payload,body', example_crc_good)
def test_crcelement_generate(
        crc: bytes, payload: bytes, body: bytes
) -> None:
    """Test CRCElement generation of body."""
    crc_element = crcelements.CRCElement()
    crc_element.crc = crc
    crc_element.payload = payload
    assert crc_element.get_body() == body


@pt.mark.parametrize('crc,payload,body', example_crc_good)
def test_crcelement_roundtrip(
        crc: bytes, payload: bytes, body: bytes
) -> None:
    """Test CRCElement generate/parse roundtrip."""
    generate_crc_element = crcelements.CRCElement()
    generate_crc_element.crc = crc
    generate_crc_element.payload = payload
    generate_crc_element.parse(generate_crc_element.get_body())
    assert generate_crc_element.crc == crc
    assert generate_crc_element.payload == payload
    parse_crc_element = crcelements.CRCElement()
    parse_crc_element.parse(generate_crc_element.get_body())
    assert parse_crc_element.crc == crc
    assert parse_crc_element.payload == payload
    assert parse_crc_element.get_body() == body


# CRCReceiver


@pt.mark.parametrize('_,payload,body', example_crc_good)
def test_crcelements_rx_indiv(
        _: bytes, payload: bytes, body: bytes
) -> None:
    """Test CRCReceiver with specific individual samples."""
    receiver = crcelements.CRCReceiver()
    assert receiver.output() is None
    receiver.input(body)
    assert receiver.output() == payload


def input_body_seq(
        receiver: crcelements.CRCReceiver,
        crc_sequence: List[Tuple[bytes, bytes, bytes]]
) -> None:
    """Add sequence of crc bodies as input to a receiver."""
    for (_, __, body) in crc_sequence:
        receiver.input(body)


def output_body_seq(
        receiver: crcelements.CRCReceiver,
        crc_sequence: List[Tuple[bytes, bytes, bytes]]
) -> None:
    """Get the sequence of crc payloads from output of receiver."""
    for (_, payload, __) in crc_sequence:
        assert receiver.output() == payload


@hp.given(crc_sequence=st.lists(
    st.sampled_from(example_crc_good), min_size=1, max_size=6
))
def test_crcelements_rx_seq(
        crc_sequence: List[Tuple[bytes, bytes, bytes]]
) -> None:
    """Test CRCReceiver sequential behaviour."""
    receiver = crcelements.CRCReceiver()
    assert receiver.output() is None
    input_body_seq(receiver, crc_sequence)
    output_body_seq(receiver, crc_sequence)
    assert receiver.output() is None
    input_body_seq(receiver, crc_sequence)
    output_body_seq(receiver, crc_sequence)
    assert receiver.output() is None


# disabling pylint for longer name
@pt.mark.parametrize('body', [
    bytes(list(range(256))) + b'\x01',
    b'\x04\x08\x01',
    bytes(list(range(256))) + bytes(list(range(256))),
    b'\x00'
])      # pylint: disable=invalid-name
def test_crcelements_rx_invalid_indiv(body: bytes) -> None:
    """Test CRCReceiver with individual invalid inputs."""
    receiver = crcelements.CRCReceiver()
    receiver.input(body)
    with pt.raises(exceptions.ProtocolDataError):
        _ = receiver.output()


@pt.mark.parametrize('_,__,body', example_crc_bad)
def test_crcelements_rx_invalid_crc(
        _: bytes, __: bytes, body: bytes
) -> None:
    """Test CRCReceiver with invalid crc input."""
    receiver = crcelements.CRCReceiver()
    receiver.input(body)
    with pt.raises(exceptions.ProtocolDataError):
        ___ = receiver.output()


# CRCSender


@pt.mark.parametrize('_,payload,body', example_crc_good)
def test_crcelements_tx_indiv(
        _: bytes, payload: bytes, body: bytes
) -> None:
    """Test CRCSender with specific individual samples."""
    sender = crcelements.CRCSender()
    assert sender.output() is None
    sender.input(payload)
    assert sender.output() == body


def input_payload_seq(
        sender: crcelements.CRCSender,
        crc_sequence: List[Tuple[bytes, bytes, bytes]]
) -> None:
    """Add sequence of crc payloads as input to a sender."""
    for (_, payload, __) in crc_sequence:
        sender.input(payload)


def output_payload_seq(
        sender: crcelements.CRCSender,
        crc_sequence: List[Tuple[bytes, bytes, bytes]]
) -> None:
    """Get the sequence of crc bodies from output of receiver."""
    for (_, __, body) in crc_sequence:
        assert sender.output() == body


@hp.given(crc_sequence=st.lists(
    st.sampled_from(example_crc_good), min_size=1, max_size=6
))
def test_crcelements_tx_seq(
        crc_sequence: List[Tuple[bytes, bytes, bytes]]
) -> None:
    """Test CRCSender sequential behaviour."""
    sender = crcelements.CRCSender()
    assert sender.output() is None
    input_payload_seq(sender, crc_sequence)
    output_payload_seq(sender, crc_sequence)
    assert sender.output() is None
    input_payload_seq(sender, crc_sequence)
    output_payload_seq(sender, crc_sequence)
    assert sender.output() is None


@pt.mark.parametrize('payload', ['123456789', 123456789, 0x75bcd15])
def test_crcelements_tx_invalid_indiv(payload: bytes) -> None:  # pylint: disable=invalid-name
    """Test CRCSender with individual invalid inputs."""
    sender = crcelements.CRCSender()
    sender.input(payload)
    with pt.raises(exceptions.ProtocolDataError):
        _ = sender.output()
