"""Tests ventserver.simulation.alarms"""

import typing
from typing import Mapping, Optional, Type
# from math import isclose
import pytest as pt # type: ignore
# from pytest import approx
import hypothesis as hp
import hypothesis.strategies as st
import betterproto

from ventserver.simulation import alarms
from ventserver.protocols.application import lists
from ventserver.protocols.protobuf import mcu_pb as pb

example_hfnc: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(time=10, fio2=80,
        rr=5, ie=10, pip=20, mode=pb.VentilationMode.hfnc),
        pb.SensorMeasurements: pb.SensorMeasurements(time=10,
        fio2=30, flow=50, paw=20),
        pb.AlarmLimits: pb.AlarmLimits(spo2=pb.Range(lower=0, upper=100),
        fio2=pb.Range(lower=21, upper=100)),
        pb.ActiveLogEvents: pb.ActiveLogEvents(id=1)
}

example_pcac: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(mode=pb.VentilationMode.pc_ac,
        ventilating=True),
        pb.SensorMeasurements: pb.SensorMeasurements(spo2=30),
        pb.AlarmLimits: pb.AlarmLimits(spo2=pb.Range(lower=50, upper=100),
        fio2=pb.Range(lower=21, upper=100)),
        pb.ActiveLogEvents: pb.ActiveLogEvents()
}

example_pcac1: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(mode=pb.VentilationMode.pc_ac,
        ventilating=True),
        pb.SensorMeasurements: pb.SensorMeasurements(spo2=102),
        pb.AlarmLimits: pb.AlarmLimits(spo2=pb.Range(lower=95, upper=100),
        fio2=pb.Range(lower=21, upper=100)),
        pb.ActiveLogEvents: pb.ActiveLogEvents()
}

example_bad: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, mode=pb.VentilationMode.vc_ac),
        pb.SensorMeasurements: pb.SensorMeasurements(time=10,
        fio2=30, flow=50, paw=20),
        pb.AlarmLimits: pb.AlarmLimits(spo2=pb.Range(lower=0, upper=100),
        fio2=pb.Range(lower=21, upper=100)),
        pb.ActiveLogEvents: pb.ActiveLogEvents(id=0)
}

state_hfnc = [example_hfnc]
state_pcac = [example_pcac]
state_pcac1 = [example_pcac1]
state_bad = [example_bad]

# Simulators

synchronizer = lists.SendSynchronizer(
    segment_type=pb.NextLogEvents,
    max_len=10, max_segment_len=5
)

@hp.given(example_time=st.floats(min_value=0, max_value=5, allow_nan=False))
@pt.mark.parametrize('state', state_pcac)
def test_pcac_alarm(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]],
    example_time: float) -> None:

    test_service = alarms.Services()
    test_service.transform(example_time, state, synchronizer)

    active_log_id = typing.cast(pb.ActiveLogEvents,
     state.get(pb.ActiveLogEvents))

    assert active_log_id.id == [0, 1]

@hp.given(example_time=st.floats(min_value=0, max_value=5, allow_nan=False))
@pt.mark.parametrize('state', state_pcac1)
def test_pcac_alarm_max(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]],
    example_time: float) -> None:

    test_service = alarms.Services()
    test_service.transform(example_time, state, synchronizer)

    active_log_id = typing.cast(pb.ActiveLogEvents,
     state.get(pb.ActiveLogEvents))

    assert active_log_id.id == [1, 0]
