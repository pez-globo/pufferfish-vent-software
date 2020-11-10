"""Trio I/O with sans-I/O protocol, running application."""

import logging
import random
import time
import functools
import typing
from typing import Dict, Mapping, Optional, Type, List

import attr

import betterproto
import trio

from ventserver.io.trio import rotaryencoder
from ventserver.integration import _trio
from ventserver.io.trio import channels
from ventserver.io.trio import websocket
from ventserver.io.trio import fileio
from ventserver.io.subprocess import frozen_frontend
from ventserver.protocols import server
from ventserver.protocols import exceptions
from ventserver.protocols.application import lists
from ventserver.protocols.protobuf import mcu_pb


# Configure logging

logger = logging.getLogger()
handler = logging.StreamHandler()
formatter = logging.Formatter(
    '%(asctime)s %(name)-12s %(levelname)-8s %(message)s'
)
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(logging.INFO)


# Simulators

@attr.s
class BreathingCircuitSimulator:
    """Base class for a breathing circuit simulator."""

    SENSOR_UPDATE_INTERVAL = 2

    all_states: Mapping[
        Type[betterproto.Message], Optional[betterproto.Message]
    ] = attr.ib()
    log_events_sender: lists.SendSynchronizer[mcu_pb.LogEvent] = attr.ib()
    current_time: float = attr.ib(default=0)  # ms after initial_time
    previous_time: float = attr.ib(default=0)  # ms after initial_time
    initial_time: float = attr.ib(default=time.time() * 1000)  # ms, Unix time

    # FiO2
    fio2_responsiveness: float = attr.ib(default=0.01)  # ms
    fio2_noise: float = attr.ib(default=0.005)  # % FiO2

    # Log Events
    next_log_event_id: int = attr.ib(default=0)
    active_alarm_ids: Dict[mcu_pb.LogEventCode, int] = attr.ib(default={})

    # State segments

    @property
    def _sensor_measurements(self) -> mcu_pb.SensorMeasurements:
        """Access the SensorMeasurements state segment."""
        return typing.cast(
            mcu_pb.SensorMeasurements,
            self.all_states[mcu_pb.SensorMeasurements]
        )

    @property
    def _cycle_measurements(self) -> mcu_pb.CycleMeasurements:
        """Access the CycleMeasurements state segment."""
        return typing.cast(
            mcu_pb.CycleMeasurements, self.all_states[mcu_pb.CycleMeasurements]
        )

    @property
    def _parameters_request(self) -> mcu_pb.ParametersRequest:
        """Access the ParametersRequest state segment."""
        return typing.cast(
            mcu_pb.ParametersRequest, self.all_states[mcu_pb.ParametersRequest]
        )

    @property
    def _parameters(self) -> mcu_pb.Parameters:
        """Access the Parameters state segment."""
        return typing.cast(
            mcu_pb.Parameters, self.all_states[mcu_pb.Parameters]
        )

    @property
    def _alarm_limits(self) -> mcu_pb.AlarmLimits:
        """Access the AlarmLimits state segment."""
        return typing.cast(
            mcu_pb.AlarmLimits, self.all_states[mcu_pb.AlarmLimits]
        )

    @property
    def _alarm_limits_request(self) -> mcu_pb.AlarmLimitsRequest:
        """Access the AlarmLimitsRequest state segment."""
        return typing.cast(
            mcu_pb.AlarmLimitsRequest,
            self.all_states[mcu_pb.AlarmLimitsRequest]
        )

    @property
    def _active_log_events(self) -> mcu_pb.ActiveLogEvents:
        """Access the ActiveLogEvents state segment."""
        return typing.cast(
            mcu_pb.ActiveLogEvents, self.all_states[mcu_pb.ActiveLogEvents]
        )

    # Timing

    @property
    def _time_step(self) -> float:
        """Compute the time step."""
        return self.current_time - self.previous_time

    # Update methods

    def update_parameters(self) -> None:
        """Update the parameters."""

    def update_alarm_limits(self) -> None:
        """Update the alarm limits."""
        if (
                self._alarm_limits_request.spo2_min >= 0
                and self._alarm_limits_request.spo2_min < 100
        ):
            self._alarm_limits.spo2_min = self._alarm_limits_request.spo2_min
        if (
                self._alarm_limits_request.spo2_max > 0
                and self._alarm_limits_request.spo2_max <= 100
        ):
            self._alarm_limits.spo2_max = self._alarm_limits_request.spo2_max

    def update_clock(self, current_time: float) -> None:
        """Update the internal state for timing."""
        self.previous_time = self.current_time
        self.current_time = current_time * 1000 - self.initial_time

    def update_sensors(self) -> None:
        """Update the simulated sensors."""

    def update_actuators(self) -> None:
        """Update the simulated actuators."""

    def update_alarms(self) -> None:
        """Update the alarms."""
        self._update_alarms_spo2()
        self._update_active_log_event_ids()

    # Alarms

    def _update_active_log_event_ids(self) -> None:
        """Update the list of active log events."""
        self._active_log_events.id = list(self.active_alarm_ids.values())

    def _update_alarms_spo2(self) -> None:
        """Update the SpO2-related alarms."""
        if self._sensor_measurements.spo2 < self._alarm_limits.spo2_min:
            self._activate_alarm(
                mcu_pb.LogEventCode.spo2_too_low,
                self._sensor_measurements.spo2
            )
        else:
            self._deactivate_alarm(mcu_pb.LogEventCode.spo2_too_low)

        if self._sensor_measurements.spo2 > self._alarm_limits.spo2_max:
            self._activate_alarm(
                mcu_pb.LogEventCode.spo2_too_high,
                self._sensor_measurements.spo2
            )
        else:
            self._deactivate_alarm(mcu_pb.LogEventCode.spo2_too_high)

    # FiO2 simulation

    def _update_fio2(self) -> None:
        """Update FiO2 measurements."""
        self._sensor_measurements.fio2 += (
            (
                self._parameters.fio2 - self._sensor_measurements.fio2
            )
            * self.fio2_responsiveness / self.SENSOR_UPDATE_INTERVAL
        )
        self._sensor_measurements.fio2 += self.fio2_noise * random.random()

    # Log Events

    def _activate_alarm(self, code: mcu_pb.LogEventCode, value: float) -> None:
        """Create a new Log Event if it is not active."""
        if code in self.active_alarm_ids:
            return

        log_event = mcu_pb.LogEvent(
            id=self.next_log_event_id, time=int(self.current_time),
            code=code, new_value=value
        )
        self.log_events_sender.input(lists.UpdateEvent(new_element=log_event))
        self.active_alarm_ids[code] = self.next_log_event_id
        self.next_log_event_id += 1

    def _deactivate_alarm(self, code: mcu_pb.LogEventCode) -> None:
        """Dectivate the Log Event if it's active."""
        if code not in self.active_alarm_ids:
            return

        self.active_alarm_ids.pop(code)


@attr.s
class PCACSimulator(BreathingCircuitSimulator):
    """Breathing circuit simulator in PC-AC mode."""

    cycle_start_time: float = attr.ib(default=0)  # ms
    cycle_period: float = attr.ib(default=2000.0)  # ms
    insp_period: float = attr.ib(default=1000.0)  # ms
    insp_responsiveness: float = attr.ib(default=0.05)  # ms
    exp_responsiveness: float = attr.ib(default=0.05)  # ms
    insp_init_flow_rate: float = attr.ib(default=120)  # L / min
    exp_init_flow_rate: float = attr.ib(default=-120)  # L / min
    insp_flow_responsiveness: float = attr.ib(default=0.02)  # ms
    exp_flow_responsiveness: float = attr.ib(default=0.02)  # ms

    def update_parameters(self) -> None:
        """Implement BreathingCircuitSimulator.update_parameters."""
        self._parameters.ventilating = self._parameters_request.ventilating
        self._parameters.mode = self._parameters_request.mode
        if self._parameters.mode != mcu_pb.VentilationMode.pc_ac:
            return

        if self._parameters_request.rr > 0:
            self._parameters.rr = self._parameters_request.rr
        if self._parameters_request.ie > 0:
            self._parameters.ie = self._parameters_request.ie
        if self._parameters_request.pip > 0:
            self._parameters.pip = self._parameters_request.pip
        self._parameters.peep = self._parameters_request.peep
        self._parameters.fio2 = self._parameters_request.fio2

    def update_sensors(self) -> None:
        """Implement BreathingCircuitSimulator.update_sensors."""
        if self._time_step == 0:
            return

        if self._parameters.mode != mcu_pb.VentilationMode.pc_ac:
            return

        self._sensor_measurements.time = int(self.current_time)
        cycle_period = 60000 / self._parameters.rr
        if self.current_time - self.cycle_start_time > cycle_period:
            self._init_cycle(cycle_period)
            self._update_cycle_measurements()

        if self.current_time - self.cycle_start_time < self.insp_period:
            self._update_airway_inspiratory()
        else:
            self._update_airway_expiratory()
        self._update_fio2()

    def _init_cycle(self, cycle_period: float) -> None:
        """Initialize at the start of the cycle."""
        self.cycle_start_time = self.current_time
        self._sensor_measurements.flow = self.insp_init_flow_rate
        self._sensor_measurements.volume = 0
        self.insp_period = cycle_period / (1 + 1 / self._parameters.ie)
        self._sensor_measurements.cycle += 1

    def _update_cycle_measurements(self) -> None:
        """Update cycle measurements."""
        self._cycle_measurements.rr = \
            self._parameters.rr + random.random() - 0.5
        self._cycle_measurements.peep = \
            self._parameters.peep + random.random() - 0.5
        self._cycle_measurements.pip = \
            self._parameters.pip + random.random() - 0.5

    def _update_airway_inspiratory(self) -> None:
        """Update sensor measurements in inspiratory phase."""
        self._sensor_measurements.paw += (
            self._parameters.pip - self._sensor_measurements.paw
        ) * self.insp_responsiveness / self._time_step
        self._sensor_measurements.flow *= (
            1 - self.insp_flow_responsiveness / self._time_step
        )
        self._sensor_measurements.volume += (
            self._sensor_measurements.flow / 60 * self._time_step
        )

    def _update_airway_expiratory(self) -> None:
        """Update sensor measurements in expiratory phase."""
        self._sensor_measurements.paw += (
            self._parameters.peep - self._sensor_measurements.paw
        ) * self.exp_responsiveness / self._time_step
        if self._sensor_measurements.flow >= 0:
            self._sensor_measurements.flow = self.exp_init_flow_rate
        else:
            self._sensor_measurements.flow *= (
                1 - self.exp_flow_responsiveness / self._time_step
            )
        self._sensor_measurements.volume += (
            self._sensor_measurements.flow / 60 * self._time_step
        )


@attr.s
class HFNCSimulator(BreathingCircuitSimulator):
    """Breathing circuit simulator in HFNC mode."""

    cycle_start_time: float = attr.ib(default=0)  # ms
    flow_responsiveness: float = attr.ib(default=0.01)  # ms
    flow_noise: float = 0.05  # L/min
    spo2_fio2_scale: float = attr.ib(default=2.5)  # % SpO2 / % FiO2
    spo2_responsiveness: float = attr.ib(default=0.0005)  # ms
    spo2_noise: float = 0.1  # % SpO2
    rr_noise: float = 5  # b/min

    def update_parameters(self) -> None:
        """Implement BreathingCircuitSimulator.update_parameters."""
        self._parameters.ventilating = self._parameters_request.ventilating
        self._parameters.mode = self._parameters_request.mode
        if self._parameters.mode != mcu_pb.VentilationMode.hfnc:
            return

        if 0 <= self._parameters_request.flow <= 80:
            self._parameters.flow = self._parameters_request.flow
        if 21 <= self._parameters_request.fio2 <= 100:
            self._parameters.fio2 = self._parameters_request.fio2
        if self._parameters_request.rr > 0:
            self._parameters.rr = self._parameters_request.rr

    def update_sensors(self) -> None:
        """Implement BreathingCircuitSimulator.update_sensors."""
        if self._time_step == 0:
            return

        if self._parameters.mode != mcu_pb.VentilationMode.hfnc:
            return

        self._sensor_measurements.time = int(self.current_time)
        cycle_period = 60000 / self._parameters.rr
        if self.current_time - self.cycle_start_time > cycle_period:
            self._init_cycle()
            self._update_cycle_measurements()

        self._update_flow()
        self._update_fio2()
        self._update_spo2()

    def _update_flow(self) -> None:
        """Update flow measurements."""
        self._sensor_measurements.flow += (
            (
                self._parameters.flow - self._sensor_measurements.flow
            )
            * self.flow_responsiveness / self.SENSOR_UPDATE_INTERVAL
        )
        self._sensor_measurements.flow += \
            self.flow_noise * (random.random() - 0.5)

    def _update_spo2(self) -> None:
        """Update SpO2 measurements."""
        self._sensor_measurements.spo2 += (
            (
                self.spo2_fio2_scale * self._sensor_measurements.fio2
                - self._sensor_measurements.spo2
            )
            * self.spo2_responsiveness / self.SENSOR_UPDATE_INTERVAL
        )
        self._sensor_measurements.spo2 += \
            self.spo2_noise * (random.random() - 0.5)
        self._sensor_measurements.spo2 = min(
            self._sensor_measurements.spo2, 100
        )
        self._sensor_measurements.spo2 = max(
            self._sensor_measurements.spo2, 21
        )

    def _init_cycle(self) -> None:
        """Initialize at the start of the cycle."""
        self.cycle_start_time = self.current_time

    def _update_cycle_measurements(self) -> None:
        """Update cycle measurements."""
        self._cycle_measurements.rr = \
            self._parameters.rr + self.rr_noise * (random.random() - 0.5)


async def simulate_states(
        all_states: Mapping[
            Type[betterproto.Message], Optional[betterproto.Message]
        ],
        log_events_sender: lists.SendSynchronizer[mcu_pb.LogEvent]
) -> None:
    """Simulate evolution of all states."""
    simulators = {
        mcu_pb.VentilationMode.pc_ac: PCACSimulator(
            all_states=all_states,
            log_events_sender=log_events_sender
        ),
        mcu_pb.VentilationMode.hfnc: HFNCSimulator(
            all_states=all_states,
            log_events_sender=log_events_sender
        )
    }

    while True:
        # Mode
        parameters = typing.cast(
            mcu_pb.Parameters, all_states[mcu_pb.Parameters]
        )
        if parameters.mode not in simulators:
            continue

        simulator = simulators[parameters.mode]
        # Parameters & Settings
        simulator.update_parameters()
        simulator.update_alarm_limits()
        # Timing
        await trio.sleep(simulator.SENSOR_UPDATE_INTERVAL / 1000)
        simulator.update_clock(time.time())
        if parameters.ventilating:
            simulator.update_sensors()
            simulator.update_actuators()
            simulator.update_alarms()


async def main() -> None:
    """Set up wiring between subsystems and process until completion."""
    # pylint: disable=duplicate-code
    # Sans-I/O Protocols
    protocol = server.Protocol()

    # I/O Endpoints
    websocket_endpoint = websocket.Driver()

    rotary_encoder = None
    try:
        rotary_encoder = rotaryencoder.Driver()
        await rotary_encoder.open()
    except exceptions.ProtocolError as err:
        exception = (
            "Unable to connect the rotary encoder, please check the "
            "serial connection. Check if the pigpiod service is running: %s"
        )
        rotary_encoder = None
        logger.error(exception, err)

    # I/O File
    filehandler = fileio.Handler()

    # Server Receive Outputs
    channel: channels.TrioChannel[
        server.ReceiveOutputEvent
    ] = channels.TrioChannel()

    # Initialize States
    states: List[Type[betterproto.Message]] = [
        mcu_pb.Parameters, mcu_pb.CycleMeasurements,
        mcu_pb.SensorMeasurements, mcu_pb.ParametersRequest
    ]

    all_states = protocol.receive.backend.all_states
    for state in all_states:
        if state is mcu_pb.ParametersRequest:
            all_states[state] = mcu_pb.ParametersRequest(
                mode=mcu_pb.VentilationMode.hfnc, ventilating=False,
                rr=30, fio2=60, flow=6
            )
        elif state is mcu_pb.AlarmLimitsRequest:
            all_states[state] = mcu_pb.AlarmLimitsRequest(
                spo2_min=85, spo2_max=95
            )
        else:
            all_states[state] = state()

    await _trio.load_file_states(
        states, protocol, filehandler
    )
    try:
        async with channel.push_endpoint:
            async with trio.open_nursery() as nursery:
                nursery.start_soon(
                    functools.partial(
                        _trio.process_all, channel=channel,
                        push_endpoint=channel.push_endpoint
                    ),
                    protocol, None, websocket_endpoint, rotary_encoder
                )
                nursery.start_soon(
                    simulate_states, all_states,
                    protocol.receive.backend.log_events_sender
                )

                while True:
                    receive_output = await channel.output()
                    await _trio.process_protocol_send(
                        receive_output.server_send, protocol,
                        None, websocket_endpoint, filehandler
                    )

                    if receive_output.frontend_delayed:
                        print("calling")
                        nursery.start_soon(
                            frozen_frontend.kill_frozen_frontend
                        )

                nursery.cancel_scope.cancel()
    except trio.EndOfChannel:
        logger.info('Finished, quitting!')


if __name__ == '__main__':
    try:
        trio.run(main)
    except KeyboardInterrupt:
        logger.info('Quitting!')
