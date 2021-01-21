"""Tests ventserver.simulation.alarm_limits"""
import typing
from typing import Mapping, Optional, Type
# from math import isclose
import pytest as pt  # type: ignore
# from pytest import approx
import betterproto

from ventserver.simulation import alarm_limits
from ventserver.protocols.protobuf import mcu_pb as pb

example_bad: Mapping[Type[betterproto.Message],
                     Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, mode=pb.VentilationMode.vc_ac),
    pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=40, upper=95),
                                                 spo2=pb.Range(lower=94, upper=100)),
    pb.AlarmLimits: pb.AlarmLimits(),
}

example_hfnc: Mapping[Type[betterproto.Message],
                      Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, ie=20, mode=pb.VentilationMode.hfnc),
    pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=40, upper=95),
                                                 spo2=pb.Range(lower=94, upper=100)),
    pb.AlarmLimits: pb.AlarmLimits(fio2=pb.Range(lower=21, upper=100),
                                   spo2=pb.Range(lower=0, upper=100)),
}

example_hfnc_min: Mapping[Type[betterproto.Message],
                          Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, ie=20, mode=pb.VentilationMode.hfnc),
    pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=10, upper=100),
                                                 spo2=pb.Range(lower=-2, upper=50)),
    pb.AlarmLimits: pb.AlarmLimits(fio2=pb.Range(lower=21, upper=100),
                                   spo2=pb.Range(lower=0, upper=100)),
}

example_hfnc_max: Mapping[Type[betterproto.Message],
                          Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, ie=20, mode=pb.VentilationMode.hfnc),
    pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=10, upper=120),
                                                 spo2=pb.Range(lower=10, upper=110)),
    pb.AlarmLimits: pb.AlarmLimits(fio2=pb.Range(lower=21, upper=100),
                                   spo2=pb.Range(lower=0, upper=100)),
}

example_pcac: Mapping[Type[betterproto.Message],
                      Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, ie=20, mode=pb.VentilationMode.pc_ac),
    pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=60, upper=80),
                                                 spo2=pb.Range(lower=94, upper=100)),
    pb.AlarmLimits: pb.AlarmLimits(fio2=pb.Range(lower=21, upper=100),
                                   spo2=pb.Range(lower=0, upper=100)),
}

example_pcac_min: Mapping[Type[betterproto.Message],
                          Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, ie=20, mode=pb.VentilationMode.pc_ac),
    pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=20, upper=100),
                                                 spo2=pb.Range(lower=-1, upper=100)),
    pb.AlarmLimits: pb.AlarmLimits(fio2=pb.Range(lower=21, upper=100),
                                   spo2=pb.Range(lower=0, upper=100))
}

example_pcac_max: Mapping[Type[betterproto.Message],
                          Optional[betterproto.Message]] = {
    pb.Parameters: pb.Parameters(rr=30, ie=20, mode=pb.VentilationMode.pc_ac),
    pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=30, upper=102),
                                                 spo2=pb.Range(lower=0, upper=102)),
    pb.AlarmLimits: pb.AlarmLimits(fio2=pb.Range(lower=21, upper=100),
                                   spo2=pb.Range(lower=0, upper=100))
}

state_bad = [example_bad]
state_hfnc = [example_hfnc]
state_bad_hfnc_min = [example_hfnc_min]
state_bad_hfnc_max = [example_hfnc_max]
state_pcac = [example_pcac]
state_bad_pcac_min = [example_pcac_min]
state_bad_pcac_max = [example_pcac_max]


# Services

@pt.mark.parametrize('state', state_bad)
def test_inactive_service(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]]) -> None:
    '''
    Scenario: Alarm Limit services behave properly
    '''
    # Given: A PCAC Alarm Limit service
    test_alarm_limits = alarm_limits.Services()
    # When: parameter service is not equal to pc_ac or hfnc
    test_alarm_limits.transform(state)

    # Then: Alarm Limits are not updated as per Alarm Limit requests
    response = typing.cast(pb.AlarmLimits,
                           state.get(pb.AlarmLimits))

    assert response.fio2 == pb.Range(lower=0, upper=0)


@pt.mark.parametrize('state', state_pcac)
def test_pcac_service_active(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]]) -> None:
    '''
    Scenario: Alarm Limit services behave properly
    '''
    # Given: A PCAC Alarm Limit service
    test_alarm_limits = alarm_limits.Services()
    # When: parameter service is equal to pc_ac
    test_alarm_limits.transform(state)

    # Then: Alarm Limits are updated as per Alarm Limit requests
    response = typing.cast(pb.AlarmLimits,
                           state.get(pb.AlarmLimits))

    assert response.fio2 == pb.Range(lower=60, upper=80)
    assert response.spo2 == pb.Range(lower=94, upper=100)


@pt.mark.parametrize('state', state_bad_pcac_min)
def test_pcac_request_min(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]]) -> None:
    '''
    Scenario: Alarm Limit services behave properly
    '''
    # Given: A PCAC Alarm Limit service
    test_alarm_limits = alarm_limits.Services()
    # When: Requested alarm limit lower value is lesser than defined lower value
    test_alarm_limits.transform(state)

    # Then: Alarm Limits are not updated as per Alarm Limit requests
    response = typing.cast(pb.AlarmLimits,
                           state.get(pb.AlarmLimits))

    assert response.fio2 == pb.Range(lower=21, upper=100)
    assert response.spo2 == pb.Range(lower=0, upper=100)


@pt.mark.parametrize('state', state_bad_pcac_max)
def test_pcac_request_max(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]]) -> None:
    '''
    Scenario: Alarm Limit services behave properly
    '''
    # Given: A PCAC Alarm Limit service
    test_alarm_limits = alarm_limits.Services()
    # When: Requested alarm limit upper value is greater than defined upper value
    test_alarm_limits.transform(state)

    # Then: Alarm Limits are not updated as per Alarm Limit requests
    response = typing.cast(pb.AlarmLimits,
                           state.get(pb.AlarmLimits))

    assert response.fio2 == pb.Range(lower=21, upper=100)
    assert response.spo2 == pb.Range(lower=0, upper=100)


@pt.mark.parametrize('state', state_hfnc)
def test_hfnc_service_active(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]]) -> None:
    '''
    Scenario: Alarm Limit services behave properly
    '''
    # Given: A HFNC Alarm Limit service
    test_alarm_limits = alarm_limits.Services()
    test_alarm_limits.transform(state)

    # Then: Alarm Limits are updated as per Alarm Limit requests
    response = typing.cast(pb.AlarmLimits,
                           state.get(pb.AlarmLimits))

    assert response.fio2 == pb.Range(lower=40, upper=95)
    assert response.spo2 == pb.Range(lower=94, upper=100)


@pt.mark.parametrize('state', state_bad_hfnc_min)
def test_hfnc_request_min(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]]) -> None:
    '''
    Scenario: Alarm Limit services behave properly
    '''
    # Given: A HFNC Alarm Limit service
    test_alarm_limits = alarm_limits.Services()
    # When: Requested alarm limit lower value is lesser than defined lower value
    test_alarm_limits.transform(state)

    # Then: Alarm Limits are not updated as per Alarm Limit requests
    response = typing.cast(pb.AlarmLimits,
                           state.get(pb.AlarmLimits))

    assert response.fio2 == pb.Range(lower=21, upper=100)
    assert response.spo2 == pb.Range(lower=0, upper=100)


@pt.mark.parametrize('state', state_bad_hfnc_max)
def test_hfnc_request_max(
    state: Mapping[Type[betterproto.Message],
                   Optional[betterproto.Message]]) -> None:
    '''
    Scenario: Alarm Limit services behave properly
    '''
    # Given: A HFNC Alarm Limit service
    test_alarm_limits = alarm_limits.Services()
    # When: Requested alarm limit upper value is greater than defined upper value
    test_alarm_limits.transform(state)

    # Then: Alarm Limits are not updated as per Alarm Limit requests
    response = typing.cast(pb.AlarmLimits,
                           state.get(pb.AlarmLimits))

    assert response.fio2 == pb.Range(lower=21, upper=100)
    assert response.spo2 == pb.Range(lower=0, upper=100)
