"""Tests ventserver.simulation.alarms"""

import typing
from typing import Mapping, Optional, Type
# from math import isclose
import pytest as pt  # type: ignore
# from pytest import approx
import hypothesis as hp
import hypothesis.strategies as st
import betterproto

from ventserver.simulation import alarms
from ventserver.protocols.application import lists
from ventserver.protocols.protobuf import mcu_pb as pb

example_hfnc: Mapping[Type[betterproto.Message],
                      Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(ventilating=True, mode=pb.VentilationMode.hfnc),
    pb.SensorMeasurements: pb.SensorMeasurements(spo2=90, fio2=20),
    pb.AlarmLimits: pb.AlarmLimits(spo2=pb.Range(lower=92, upper=100),
                                   fio2=pb.Range(lower=21, upper=100)),
    pb.ActiveLogEvents: pb.ActiveLogEvents()
}

example_hfnc_bad: Mapping[Type[betterproto.Message],
                          Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, mode=pb.VentilationMode.hfnc),
    pb.SensorMeasurements: pb.SensorMeasurements(spo2=90, fio2=20),
    pb.AlarmLimits: pb.AlarmLimits(spo2=pb.Range(lower=92, upper=100),
                                   fio2=pb.Range(lower=21, upper=100)),
    pb.ActiveLogEvents: pb.ActiveLogEvents(id=0)
}

example_pcac_min: Mapping[Type[betterproto.Message],
                          Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(mode=pb.VentilationMode.pc_ac,
                                 ventilating=True),
    pb.SensorMeasurements: pb.SensorMeasurements(spo2=30, fio2=10),
    pb.AlarmLimits: pb.AlarmLimits(spo2=pb.Range(lower=50, upper=100),
                                   fio2=pb.Range(lower=21, upper=100)),
    pb.ActiveLogEvents: pb.ActiveLogEvents()
}

example_pcac_max: Mapping[Type[betterproto.Message],
                          Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(mode=pb.VentilationMode.pc_ac,
                                 ventilating=True),
    pb.SensorMeasurements: pb.SensorMeasurements(spo2=102, fio2=102),
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

state_hfnc_bad = [example_hfnc_bad]
state_hfnc = [example_hfnc]
state_pcac_min = [example_pcac_min]
state_pcac_max = [example_pcac_max]
state_bad = [example_bad]

# Simulators

synchronizer = lists.SendSynchronizer(
    segment_type=pb.NextLogEvents,
    max_len=10, max_segment_len=5
)


@pt.mark.parametrize('state', state_bad)
def test_pcac_service_invalid_mode(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]],
) -> None:
    '''
    Scenario: The Service class correctly handles alarms
    '''
    # Given: A PCAC Service class object is constructed with default parameters
    test_service = alarms.Service()
    # When: The ventilation mode in parameters class is not equal to either pcac or hfnc
    parameters = typing.cast(
        pb.Parameters, state[pb.Parameters])
    alarm_limits = typing.cast(
        pb.AlarmLimits, state[pb.AlarmLimits])
    sensor_measurments = typing.cast(
        pb.SensorMeasurements, state[pb.SensorMeasurements])
    active_log_events = typing.cast(
        pb.ActiveLogEvents, state[pb.ActiveLogEvents])

    test_service.transform(parameters, alarm_limits,
                           sensor_measurments, active_log_events, synchronizer)
    # Then: The alarms are not updated for any parameters
    assert test_service._manager.active_alarm_ids.get(pb.LogEventCode) is None


@pt.mark.parametrize('state', state_hfnc_bad)
def test_pcac_service_invalid_ventilating(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]],
) -> None:
    '''
    # Scenario: The Service class correctly handles alarms
    '''
    # Given: A PCAC Service class object is constructed with default parameters
    test_service = alarms.Service()
    # When: The ventilating value in parameter class is false
    parameters = typing.cast(
        pb.Parameters, state[pb.Parameters])
    alarm_limits = typing.cast(
        pb.AlarmLimits, state[pb.AlarmLimits])
    sensor_measurments = typing.cast(
        pb.SensorMeasurements, state[pb.SensorMeasurements])
    active_log_events = typing.cast(
        pb.ActiveLogEvents, state[pb.ActiveLogEvents])

    test_service.transform(parameters, alarm_limits,
                           sensor_measurments, active_log_events, synchronizer)
    # Then: All Active alarms are deactivated
    assert test_service._manager.active_alarm_ids.get(pb.LogEventCode) is None


@pt.mark.parametrize('state', state_pcac_min)
def test_pcac_service_alarm_min(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]],
) -> None:
    '''
    # Scenario: The Service class correctly handles pcac alarms for SpO2
    '''
    # Given: a pc_ac alarm service object
    test_service = alarms.Service()
    # When: The sensor measurments value for Spo2 & fio2 is lesser than the minimum alarm_limit for spo2 & fio2
    parameters = typing.cast(
        pb.Parameters, state[pb.Parameters])
    alarm_limits = typing.cast(
        pb.AlarmLimits, state[pb.AlarmLimits])
    sensor_measurments = typing.cast(
        pb.SensorMeasurements, state[pb.SensorMeasurements])
    active_log_events = typing.cast(
        pb.ActiveLogEvents, state[pb.ActiveLogEvents])

    test_service.transform(parameters, alarm_limits,
                           sensor_measurments, active_log_events, synchronizer)

    # Then: The alarm for spo2 & fio2 too low is triggered
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_low) is not None
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.fio2_too_low) is not None


@pt.mark.parametrize('state', state_pcac_max)
def test_pcac_alarm_max(
        state: Mapping[Type[betterproto.Message],
                       Optional[betterproto.Message]],
        example_time: float) -> None:
    '''
    # Scenario: The Service class correctly handles pcac alarms for SpO2
    '''
    # Given: a pc_ac alarm service object
    test_service = alarms.Service()
    # When: The sensor measurments value for Spo2 & fio2 is greater than the maximum alarm_limit for spo2 & fio2
    parameters = typing.cast(
        pb.Parameters, state[pb.Parameters])
    alarm_limits = typing.cast(
        pb.AlarmLimits, state[pb.AlarmLimits])
    sensor_measurments = typing.cast(
        pb.SensorMeasurements, state[pb.SensorMeasurements])
    active_log_events = typing.cast(
        pb.ActiveLogEvents, state[pb.ActiveLogEvents])

    test_service.transform(parameters, alarm_limits,
                           sensor_measurments, active_log_events, synchronizer)
    # Then: The alarm for spo2 & fio2 too high is triggered
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_low) is None
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.fio2_too_high) is not None
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_high) is not None


@hp.given(example_time=st.floats(min_value=0))
@pt.mark.parametrize('state', state_bad)
def test_invalid_mode(
        state: Mapping[Type[betterproto.Message],
                       Optional[betterproto.Message]],
        example_time: float) -> None:
    '''
    Scenario: alarm services behaving properly
    '''
    # Given: a pc_ac alarm service object
    test_service = alarms.Services()
    # When: parameter mode is not equal to pc_ac
    test_service.transform(example_time, state, synchronizer)

    # Then: alarms are not updated for parameters
    assert test_service._manager.active_alarm_ids.get(pb.LogEventCode) is None


@hp.given(example_time=st.floats(min_value=0))
@pt.mark.parametrize('state', state_hfnc_bad)
def test_invalid_ventilating(
        state: Mapping[Type[betterproto.Message],
                       Optional[betterproto.Message]],
        example_time: float) -> None:
    '''
    Scenario: alarm services behaving properly
    '''
    # Given: a pc_ac alarm service object
    test_service = alarms.Services()
    # When: parameter ventilating mode is false
    test_service.transform(example_time, state, synchronizer)

    # Then: alarms are not updated for parameters
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_low) is None


@hp.given(example_time=st.floats(min_value=0))
@pt.mark.parametrize('state', state_pcac_min)
def test_pcac_alarm_min(
        state: Mapping[Type[betterproto.Message],
                       Optional[betterproto.Message]],
        example_time: float) -> None:
    '''
    Scenario: alarm services behaving properly
    '''
    # Given: a pc_ac alarm service object
    test_service = alarms.Services()
    # When: sensor measurments spo2 is less than lower alarm_limit for spo2
    test_service.transform(example_time, state, synchronizer)

    # Then: alarm for spo2 & fio2 too low is triggered
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_low) is not None
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.fio2_too_low) is not None


@hp.given(example_time=st.floats(min_value=0))
@pt.mark.parametrize('state', state_pcac_max)
def test_pcac_alarm_max(
        state: Mapping[Type[betterproto.Message],
                       Optional[betterproto.Message]],
        example_time: float) -> None:
    '''
    Scenario: alarm services behaving properly
    '''
    # Given: a pc_ac alarm service object
    test_service = alarms.Services()
    # When: sensor measurments fio2 is greater than upper alarm_limit for fio2
    test_service.transform(example_time, state, synchronizer)
    # Then: alarm for spo2 & fio2 too high is triggered
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_low) is None
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.fio2_too_high) is not None
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_high) is not None


@hp.given(example_time=st.floats(min_value=0))
@pt.mark.parametrize('state', state_hfnc)
def test_hfnc_alarm_min(
        state: Mapping[Type[betterproto.Message],
                       Optional[betterproto.Message]],
        example_time: float) -> None:
    '''
    Scenario: alarm services behaving properly
    '''
    # Given: a hfnc alarm service object
    test_service = alarms.Services()
    # When: sensor measurments spo2 is less than lower alarm_limit for spo2
    test_service.transform(example_time, state, synchronizer)

    # Then: alarm for spo2 & fio2 too low is triggered
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.spo2_too_low) is not None
    assert test_service._manager.active_alarm_ids.get(
        pb.LogEventCode.fio2_too_low) is not None
