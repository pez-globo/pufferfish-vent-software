"""Tests ventserver.simulation.parameters"""
import typing
from typing import Mapping, Optional, Type
# from math import isclose
import pytest as pt # type: ignore
# from pytest import approx
import betterproto

from ventserver.simulation import parameters
from ventserver.protocols.protobuf import mcu_pb as pb

example_hfnc: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(time=10, fio2=80,
        rr=5, ie=10, pip=20, mode=pb.VentilationMode.hfnc),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=50, fio2=40,
        mode=pb.VentilationMode.hfnc, pip=20, flow=80),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=21, upper=100)),
        pb.AlarmLimits: pb.AlarmLimits(rr=pb.Range(lower=40, upper=50))
}

example_bad_hfnc: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, ie=20),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=50, flow=81, mode=pb.VentilationMode.hfnc, pip=30),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(),
        pb.AlarmLimits: pb.AlarmLimits()
}

example_pcac: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(mode=pb.VentilationMode.pc_ac, rr=5),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=20, fio2=60, mode=pb.VentilationMode.pc_ac, pip=30),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(fio2=pb.Range(lower=21, upper=100)),
        pb.AlarmLimits: pb.AlarmLimits(rr=pb.Range(lower=40, upper=50)),
        pb.ActiveLogEvents: pb.ActiveLogEvents(id=1)
}

example_bad_pcac: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, ie=20),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=50, fio2=20, mode=pb.VentilationMode.pc_ac, pip=30),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(),
        pb.AlarmLimits: pb.AlarmLimits()
}

example_bad: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, ie=20, mode=pb.VentilationMode.pc_simv),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=50, fio2=20, mode=pb.VentilationMode.niv, pip=30),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(),
        pb.AlarmLimits: pb.AlarmLimits()
}

state_bad = [example_bad]
state_hfnc = [example_hfnc]
state_bad_hfnc = [example_bad_hfnc]
state_pcac = [example_pcac]
state_bad_pcac = [example_bad_pcac]

# Services

# PC_AC Service

@pt.mark.parametrize('state', state_bad)
def test_pcac_mode_invalid(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    '''
    Scenario: parameters services behaves properly
    '''
    # Given: A PCAC parameter service
    limits = parameters.Services()

    # When: parameter mode is not equal to pc_ac
    limits.transform(state)

    # Then: parameters are not updated as per the parameter requests
    response = typing.cast(pb.Parameters, state.get(pb.Parameters))
    assert response.rr == 30 # No change in parameters

@pt.mark.parametrize('state', state_pcac)
def test_pcac_mode_active(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    '''
    Scenario: parameters services behaves properly
    '''

    # Given: A PCAC parameter service
    limits = parameters.Services()

    # When: parameter mode is equal to pc_ac
    limits.transform(state)

    # Then: parameters are updated as per the parameter requests

    response = typing.cast(pb.Parameters, state.get(pb.Parameters))
    assert response.rr != 20
    assert response.rr == 10 # same as parameters request
    assert response.ie == 20 # same as parameters request
    assert response.pip == 30 # same as parameters request
    assert response.fio2 == 60

@pt.mark.parametrize('state', state_bad_pcac)
def test_pcac_transform_fio2(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    '''
    Scenario: parameters services behaves properly
    '''
    # Given: A PCAC parameter service
    limits = parameters.Services()

    # When: parameter request fio2 is less than fio2_min
    limits.transform(state)

    # Then: parameters are not updated as per the parameter requests
    response = typing.cast(pb.Parameters, state.get(pb.Parameters))
    assert response.fio2 != 20 # as fio2 is less than fio2_min

# HFNC Service

@pt.mark.parametrize('state', state_bad)
def test_hfnc_mode_invalid(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    '''
    Scenario: parameters services behaves properly
    '''

    # Given: A HFNC parameter service
    limits = parameters.Services()

    # When: parameter mode is not equal to HFNC
    limits.transform(state)

    # Then: parameters are not updated as per the parameter requests
    response = typing.cast(pb.Parameters, state.get(pb.Parameters))
    assert response.rr == 30 # No change in parameters

@pt.mark.parametrize('state', state_hfnc)
def test_hfnc_mode_active(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    '''
    Scenario: parameters services behaves properly
    '''

    # Given: A HFNC parameter service
    limits = parameters.Services()

    # When: parameter mode is equal to HFNC
    limits.transform(state)
    response = typing.cast(pb.Parameters, state.get(pb.Parameters))
    assert response.flow == 80 # same as parameters request
    assert response.fio2 == 40 # same as parameters request

    hfnc_service = parameters.HFNC()
    assert hfnc_service.mode_active(response) is True

@pt.mark.parametrize('state', state_bad_hfnc)
def test_hfnc_transform_fio2(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    '''
    Scenario: parameters services behaves properly
    '''

    # Given: A HFNC parameter service
    limits = parameters.Services()

    # When: parameter request flow is greater than flow_max
    limits.transform(state)

    # Then: parameters are not updated as per the parameter requests
    response = typing.cast(pb.Parameters, state.get(pb.Parameters))
    assert response.flow != 81 # as flow is greater than flow_max
