"""Tests ventserver.io.trio.serials"""

import typing
from typing import Mapping, Optional, Type
from math import isclose
import pytest as pt # type: ignore
from pytest import approx
import betterproto

from ventserver import simulation
from ventserver.protocols.application import lists
from ventserver.protocols.protobuf import mcu_pb as pb

MESSAGE_CLASSES_hfnc: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(time=10, fio2=80,
        rr=5, ie=10, pip=20, mode=pb.VentilationMode.hfnc),
        pb.SensorMeasurements: pb.SensorMeasurements(time=10,
        fio2=30, flow=50, paw=20),
        pb.CycleMeasurements: pb.CycleMeasurements(time=10, rr=20, ve=300),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=50, fio2=60,
        mode=pb.VentilationMode.hfnc, pip=20, flow=40),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(rr_min=10),
        pb.AlarmLimits: pb.AlarmLimits(rr_max=100, spo2_max=90, spo2_min=10)
}

MESSAGE_CLASSES_pcac: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(mode=pb.VentilationMode.pc_ac, rr=5),
        pb.SensorMeasurements: pb.SensorMeasurements(time=10, flow=20),
        pb.CycleMeasurements: pb.CycleMeasurements(time=10, ve=300),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=20, fio2=60, mode=pb.VentilationMode.pc_ac, pip=30),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(rr_min=10),
        pb.AlarmLimits: pb.AlarmLimits(rr_max=100, spo2_max=90, spo2_min=10)
}

MESSAGE_CLASSES_bad: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]] = {
        pb.Parameters: pb.Parameters(rr=30, ie=20),
        pb.SensorMeasurements: pb.SensorMeasurements(fio2=50),
        pb.CycleMeasurements: pb.CycleMeasurements(rr=10),
        pb.ParametersRequest: pb.ParametersRequest(time=10, rr=10,
        ie=50, fio2=60, mode=pb.VentilationMode.niv, pip=30),
        pb.AlarmLimitsRequest: pb.AlarmLimitsRequest(),
        pb.AlarmLimits: pb.AlarmLimits()
}

state_hfnc = [MESSAGE_CLASSES_hfnc]
state_pcac = [MESSAGE_CLASSES_pcac]
state_bad = [MESSAGE_CLASSES_bad]

synchronizer = lists.SendSynchronizer(
    segment_type=pb.NextLogEvents,
    max_len=10, max_segment_len=5
)

@pt.mark.parametrize('message', state_bad)
def test_pcac_invalid_mode(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    pcac_circuit = simulation.PCACSimulator(message, synchronizer, 0, 0, 0)
    # When: ventilation mode is not equal to pc_ac
    pcac_circuit.update_parameters()

    output = pcac_circuit.all_states.get(pb.Parameters)
    parameters = typing.cast(pb.Parameters, output)

    # Then: Parameter values are not updated
    assert parameters.rr == 30 # same as initial value
    assert parameters.ie == 20 # same as initial value

@pt.mark.parametrize('message', state_pcac)
def test_pcac_update_parameters(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    pcac_circuit = simulation.PCACSimulator(message, synchronizer, 0, 0, 0)
    pcac_circuit.update_clock(20)
    # When: parameters are updated as per the paramater requests at 20ms
    pcac_circuit.update_parameters()
    output = pcac_circuit.all_states.get(pb.Parameters)
    parameters = typing.cast(pb.Parameters, output)

    # Then: parameters values are equal to parameters request values
    assert parameters.rr != 20
    assert parameters.rr == 10 # same as parameters request
    assert parameters.ie == 20 # same as parameters request
    assert parameters.pip == 30 # same as parameters request

@pt.mark.parametrize('message', state_pcac)
def test_pcac_sensors_time_step(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    pcac_circuit = simulation.PCACSimulator(message, synchronizer)
    # When: time_step is 0
    pcac_circuit.update_sensors()

    # Then: pcac simulator cycle period is not updated
    assert pcac_circuit.cycle_period == 2000

@pt.mark.parametrize('message', state_bad)
def test_pcac_invalid_mode_sensors(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    pcac_circuit = simulation.PCACSimulator(message, synchronizer, 15, 0, 0)
    pcac_circuit.update_clock(15)
    # When: ventilation mode is not equal to pc_ac
    pcac_circuit.update_sensors()

    # Then: cycle start time is not equal to current time
    assert pcac_circuit.cycle_start_time != 15000

@pt.mark.parametrize('message', state_pcac)
def test_pcac_update_sensors_cycle(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    pcac_circuit = simulation.PCACSimulator(message, synchronizer, 15, 0, 0)
    pcac_circuit.update_clock(15)
    # When: current time - cycle_start time is greater than cycle period
    pcac_circuit.update_sensors()
    output = pcac_circuit.all_states.get(pb.SensorMeasurements)
    sensor_measurments = typing.cast(pb.SensorMeasurements, output)
    cycle_measurments = typing.cast(pb.CycleMeasurements,
        pcac_circuit.all_states.get(pb.CycleMeasurements))

    # Then: sensor measurments and cycle measurments are updated accordingly
    assert pcac_circuit.cycle_start_time == 15000
    assert isclose(sensor_measurments.flow, 120, abs_tol=0.0002)
    assert sensor_measurments.cycle == 1
    assert cycle_measurments.rr != 0
    assert cycle_measurments.pip != 0
    assert cycle_measurments.peep != 0

@pt.mark.parametrize('message', state_pcac)
def test_pcac_update_sensors_insp(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    pcac_circuit = simulation.PCACSimulator(message, synchronizer, 30, 0, 0)
    # When: ventilator is in inspiratory phase
    pcac_circuit.update_sensors()
    output = pcac_circuit.all_states.get(pb.SensorMeasurements)
    sensor_measurments = typing.cast(pb.SensorMeasurements, output)

    # Then: Sensor measurments are updated in inspiratory phase
    assert sensor_measurments.fio2 != 20
    assert sensor_measurments.paw != 0 # 0 is the value we passed
    assert sensor_measurments.flow != 20 # 20 is the initial value we passed


@pt.mark.parametrize('message', state_pcac)
def test_update_sensors_exp(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    pcac_circuit = simulation.PCACSimulator(message, synchronizer, 0, 0, 0)
    pcac_circuit.update_clock(2)
    # When: ventilator is in expiratory phase
    pcac_circuit.update_sensors()
    output = pcac_circuit.all_states.get(pb.SensorMeasurements)
    sensor_measurments = typing.cast(pb.SensorMeasurements, output)

    # Then: sensor measurments are updated in expiratory phase
    assert sensor_measurments.flow == approx(-120)
    assert sensor_measurments.paw != 0 # 0 is the value we passed

# HFNC simulator

@pt.mark.parametrize('message', state_bad)
def test_hfnc_invalid_mode(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator object
    hfnc_circuit = simulation.HFNCSimulator(message, synchronizer, 0, 0, 0)
    # When: ventilation mode is not equal to hfnc
    hfnc_circuit.update_parameters()

    output = hfnc_circuit.all_states.get(pb.Parameters)
    parameters = typing.cast(pb.Parameters, output)

    # Then: Parameter values are not updated
    assert parameters.rr == 30 # same as initial value
    assert parameters.ie == 20 # same as initial value

@pt.mark.parametrize('message', state_hfnc)
def test_hfnc_update_parameters(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator object
    hfnc_circuit = simulation.HFNCSimulator(message, synchronizer, 50, 0, 0)
    # WHen: parameters are updated as per the paramater requests at 50ms
    hfnc_circuit.update_parameters()
    # Then: Parameters are updated properly
    output = hfnc_circuit.all_states.get(pb.Parameters)
    parameters = typing.cast(pb.Parameters, output)

    assert parameters.rr == 50 # same as parameters request
    assert parameters.fio2 == 60 # same as parameters request
    assert parameters.flow == 40 # same as parameters request

@pt.mark.parametrize('message', state_hfnc)
def test_hfnc_sensors_time_step(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator object
    hfnc_circuit = simulation.HFNCSimulator(message, synchronizer)
    # When: Time step is 0
    hfnc_circuit.update_sensors()

    output = hfnc_circuit.all_states.get(pb.SensorMeasurements)
    sensor_measurments = typing.cast(pb.SensorMeasurements, output)

    # Then: Sensor measurments fio2 is unchanged
    assert sensor_measurments.fio2 == 30

@pt.mark.parametrize('message', state_bad)
def test_hfnc_invalid_mode_sensors(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: PCAC simulator behaves properly
    """
    # Given: A PCAC simulator object
    hfnc_circuit = simulation.HFNCSimulator(message, synchronizer, 0, 0, 0)
    hfnc_circuit.update_clock(60)
    # When: ventilation mode is not equal to pc_ac
    hfnc_circuit.update_sensors()

    output = hfnc_circuit.all_states.get(pb.SensorMeasurements)
    sensor_measurments = typing.cast(pb.SensorMeasurements, output)

    # Then: Sensor measurments time is not equal to current time
    assert sensor_measurments.fio2 == 50 # 50 is the initial value

@pt.mark.parametrize('message', state_hfnc)
def test_hfnc_update_sensors(
    message: Mapping[Type[betterproto.Message],
    Optional[betterproto.Message]]) -> None:
    """
    Scenario: HFNC simulator behaves properly
    """
    # Given: A HFNC simulator object
    hfnc_circuit = simulation.HFNCSimulator(message, synchronizer, 0, 0, 0)
    hfnc_circuit.update_clock(60)
    # WHen: Current time - cycle start time is greater than cycle period
    hfnc_circuit.update_sensors()
    # Then: current time and cycle measurment values are updated
    output = hfnc_circuit.all_states.get(pb.CycleMeasurements)
    cycle_measurments = typing.cast(pb.CycleMeasurements, output)
    sensor_measurments = typing.cast(pb.SensorMeasurements,
        hfnc_circuit.all_states.get(pb.SensorMeasurements))

    assert cycle_measurments.rr != 20 # 20 is the initial value
    assert sensor_measurments.fio2 != 30 # 30 is the initial value
    assert sensor_measurments.flow != 20 # 20 is the initial value
