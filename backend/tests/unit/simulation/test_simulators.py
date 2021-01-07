"""Tests ventserver.io.trio.serials"""

import typing
from typing import Mapping, Optional, Type
from math import isclose
import pytest as pt # type: ignore
import hypothesis as hp
import hypothesis.strategies as st
import betterproto

from ventserver.simulation import simulators
from ventserver.protocols.application import lists
from ventserver.protocols.protobuf import mcu_pb as pb

example_hfnc: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(mode=pb.VentilationMode.hfnc, ventilating=True),
        pb.SensorMeasurements: pb.SensorMeasurements(time=10,
        fio2=30, flow=50, spo2=92),
        pb.CycleMeasurements: pb.CycleMeasurements(time=10, rr=20, ve=300),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=50, fio2=60,
        mode=pb.VentilationMode.hfnc, pip=20, flow=40)
}

example_pcac: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=5, ie=10,
        mode=pb.VentilationMode.pc_ac, ventilating=True),
        pb.SensorMeasurements: pb.SensorMeasurements(flow=20),
        pb.CycleMeasurements: pb.CycleMeasurements(time=10, ve=300)
}

example_bad_mode: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, ie=20,
        mode=pb.VentilationMode.niv, ventilating=True),
        pb.SensorMeasurements: pb.SensorMeasurements(fio2=50),
        pb.CycleMeasurements: pb.CycleMeasurements(rr=10),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=50, fio2=60, pip=30)
}

example_bad_hfnc: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, ie=20,
        mode=pb.VentilationMode.hfnc, ventilating=False),
        pb.SensorMeasurements: pb.SensorMeasurements(fio2=50),
        pb.CycleMeasurements: pb.CycleMeasurements(rr=10),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=50, fio2=60, pip=30)
}

example_bad_pcac: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, ie=20,
        mode=pb.VentilationMode.pc_ac, ventilating=False),
        pb.SensorMeasurements: pb.SensorMeasurements(fio2=50),
        pb.CycleMeasurements: pb.CycleMeasurements(rr=10),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=50, fio2=60, pip=30)
}

state_bad_mode = [example_bad_mode]
state_hfnc = [example_hfnc]
state_bad_hfnc = [example_bad_hfnc]
state_pcac = [example_pcac]
state_bad_pcac = [example_bad_pcac]


synchronizer = lists.SendSynchronizer(
    segment_type=pb.NextLogEvents,
    max_len=10, max_segment_len=5
)

# PCAC SIMULATOR

@hp.given(example_time=st.floats(min_value=0, max_value=10, allow_nan=False))
@pt.mark.parametrize('state', state_bad_mode)
def test_pcac_invalid_mode(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]],
    example_time: float) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.pc_ac).initial_time = 0

    # When: ventilation mode is not equal to pc_ac
    service.transform(example_time, state)

    sensor_measurements = typing.cast(pb.SensorMeasurements,
        state.get(pb.SensorMeasurements))

    # Then: sensor measurements time is unchanged
    assert sensor_measurements.time == 0

@hp.given(example_time=st.floats(min_value=0, max_value=10, allow_nan=False))
@pt.mark.parametrize('state', state_bad_pcac)
def test_pcac_not_ventilating(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]],
    example_time: float) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.pc_ac).initial_time = 0
    # When: ventilating mode is false
    service.transform(example_time, state)

    sensor_measurements = typing.cast(pb.SensorMeasurements,
        state.get(pb.SensorMeasurements))

    # Then: sensor measurements time is unchanged
    assert sensor_measurements.time == 0

@pt.mark.parametrize('state', state_pcac)
def test_pcac_sensors_time_step(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.pc_ac).initial_time = 0
    # When: time_step function returns 0
    service.transform(0, state)

    sensor_measurements = typing.cast(pb.SensorMeasurements,
    state.get(pb.SensorMeasurements))

    # Then: sensor measurments flow is unchanged
    assert isclose(sensor_measurements.flow, 20, abs_tol=0.002)

@pt.mark.parametrize('state', state_pcac)
def test_pcac_update_sensors_insp(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.pc_ac).initial_time = 0
    # When: ventilator is in inspiratory phase
    service.transform(25.0, state)

    sensor_measurements = typing.cast(pb.SensorMeasurements,
        state.get(pb.SensorMeasurements))

    cycle_measurments = typing.cast(pb.CycleMeasurements,
        state.get(pb.CycleMeasurements))

    # Then: Sensor measurments are updated in inspiratory phase
    assert isclose(sensor_measurements.flow, 120, abs_tol=0.0002)
    assert cycle_measurments.rr != 0
    assert cycle_measurments.pip != 0
    assert cycle_measurments.peep != 0
    assert sensor_measurements.fio2 != 20
    assert sensor_measurements.flow != 20 # 20 is the input value we passed
    assert sensor_measurements.fio2 != 0

# HFNC simulator

@pt.mark.parametrize('state', state_bad_mode)
def test_hfnc_invalid_mode(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.hfnc).initial_time = 0
    # When: ventilation mode is not equal to hfnc
    service.transform(10, state)

    sensor_measurements = typing.cast(pb.SensorMeasurements,
        state.get(pb.SensorMeasurements))

    # Then: sensor measurements time is unchanged
    assert sensor_measurements.time == 0

@pt.mark.parametrize('state', state_bad_hfnc)
def test_hfnc_not_ventilating(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.hfnc).initial_time = 0
    # When: ventilating mode is false
    service.transform(12, state)

    sensor_measurements = typing.cast(pb.SensorMeasurements,
        state.get(pb.SensorMeasurements))

    # Then: sensor measurements time is unchanged
    assert sensor_measurements.time == 0

@pt.mark.parametrize('state', state_hfnc)
def test_hfnc_sensors_time_step(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.hfnc).initial_time = 0
    # When: time_step function returns 0
    service.transform(0, state)

    sensor_measurements = typing.cast(pb.SensorMeasurements,
        state.get(pb.SensorMeasurements))

    # Then: sensor measurements time is unchanged
    assert sensor_measurements.time == 0

@pt.mark.parametrize('state', state_hfnc)
def test_service_active(
    state: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator service
    service = simulators.Services()
    service._services.get(pb.VentilationMode.hfnc).initial_time = 0
    # WHen: parameter service is equal to HFNC
    service.transform(20, state)
    # Then: Sensor measurements fio2, flow, spo2 are updated
    sensor_measurements = typing.cast(pb.SensorMeasurements,
        state.get(pb.SensorMeasurements))

    assert sensor_measurements.fio2 != 30 # 30 is the input value
    assert sensor_measurements.flow != 20 # 20 is the input value
    assert sensor_measurements.spo2 != 92 # 92 is the input value
