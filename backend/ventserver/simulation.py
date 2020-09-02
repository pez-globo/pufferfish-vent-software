"""Trio I/O with sans-I/O protocol, running application."""

import random
import time
import typing
from typing import Mapping, Optional, Type

import trio

from ventserver.integration import _trio
from ventserver.io.trio import channels
from ventserver.io.trio import websocket
from ventserver.protocols import application
from ventserver.protocols import server
from ventserver.protocols.protobuf import mcu_pb


async def simulate_states(
        all_states: Mapping[
            Type[application.PBMessage], Optional[application.PBMessage]
        ]
) -> None:
    """Simulate evolution of all states."""
    current_time = time.time() * 1000
    initial_time = current_time
    previous_time = current_time
    time_step = current_time - previous_time
    cycle_start_time = 0.0
    sensor_update_interval = 2.0  # ms
    cycle_period = 2000.0  # ms
    insp_period = 1000.0  # ms
    insp_responsiveness = 0.05  # ms
    exp_responsiveness = 0.05  # ms
    insp_init_flow_rate = 120  # L / min
    exp_init_flow_rate = -120  # L / min
    insp_flow_responsiveness = 0.02  # ms
    exp_flow_responsiveness = 0.02  # ms
    fio2_responsiveness = 0.01  # ms
    while True:
        sensor_measurements = typing.cast(
            mcu_pb.SensorMeasurements, all_states[mcu_pb.SensorMeasurements]
        )
        cycle_measurements = typing.cast(
            mcu_pb.CycleMeasurements, all_states[mcu_pb.CycleMeasurements]
        )
        parameters = typing.cast(
            mcu_pb.Parameters, all_states[mcu_pb.Parameters]
        )
        parameters_request = typing.cast(
            mcu_pb.ParametersRequest, all_states[mcu_pb.ParametersRequest]
        )
        # Parameters
        if parameters_request.rr > 0:
            parameters.rr = parameters_request.rr
        if parameters_request.ie > 0:
            parameters.ie = parameters_request.ie
        if parameters_request.pip > 0:
            parameters.pip = parameters_request.pip
        parameters.peep = parameters_request.peep
        parameters.fio2 = parameters_request.fio2
        # Timing
        await trio.sleep(sensor_update_interval / 1000)
        previous_time = current_time
        current_time = time.time() * 1000 - initial_time
        time_step = current_time - previous_time
        sensor_measurements.time = int(current_time)
        cycle_period = 60000 / parameters.rr
        if current_time - cycle_start_time > cycle_period:
            cycle_start_time = current_time
            sensor_measurements.flow = insp_init_flow_rate
            insp_period = cycle_period / (1 + 1 / parameters.ie)
            # Cycle Measurements
            cycle_measurements.rr = parameters.rr + random.random() - 0.5
            cycle_measurements.peep = parameters.peep + random.random() - 0.5
            cycle_measurements.pip = parameters.pip + random.random() - 0.5
        # Airway
        if current_time - cycle_start_time < insp_period:
            sensor_measurements.paw += (
                parameters.pip - sensor_measurements.paw
            ) * insp_responsiveness / time_step
            sensor_measurements.flow *= (
                1 - insp_flow_responsiveness / time_step
            )
        else:
            sensor_measurements.paw += (
                parameters.peep - sensor_measurements.paw
            ) * exp_responsiveness / time_step
            if sensor_measurements.flow >= 0:
                sensor_measurements.flow = exp_init_flow_rate
            else:
                sensor_measurements.flow *= (
                    1 - exp_flow_responsiveness / time_step
                )
        # FiO2
        sensor_measurements.fio2 += (
            (
                parameters.fio2 - sensor_measurements.fio2
            )
            * fio2_responsiveness / sensor_update_interval
        )
        sensor_measurements.fio2 += 0.005 * random.random()


async def main() -> None:
    """Set up wiring between subsystems and process until completion."""
    # Sans-I/O Protocols
    protocol = server.Protocol()

    # I/O Endpoints
    websocket_endpoint = websocket.Driver()

    # Server Receive Outputs
    channel: channels.TrioChannel[
        server.ReceiveOutputEvent
    ] = channels.TrioChannel()

    # Initialize State
    all_states = protocol.receive.backend.all_states
    all_states[mcu_pb.Parameters] = mcu_pb.Parameters()
    all_states[mcu_pb.CycleMeasurements] = mcu_pb.CycleMeasurements()
    all_states[mcu_pb.SensorMeasurements] = mcu_pb.SensorMeasurements()
    all_states[mcu_pb.ParametersRequest] = mcu_pb.ParametersRequest(
        mode=mcu_pb.VentilationMode(
            support=mcu_pb.SpontaneousSupport.ac,
            cycling=mcu_pb.VentilationCycling.pc
        ),
        pip=30, peep=10, rr=30, ie=1, fio2=60
    )

    try:
        async with channel.push_endpoint:
            async with trio.open_nursery() as nursery:
                nursery.start_soon(
                    _trio.process_all, None, protocol,
                    websocket_endpoint, channel, channel.push_endpoint
                )
                nursery.start_soon(simulate_states, all_states)

                while True:
                    receive_output = await channel.output()
                    await _trio.process_protocol_send(
                        receive_output.server_send, protocol,
                        None, websocket_endpoint
                    )
                nursery.cancel_scope.cancel()
    except trio.EndOfChannel:
        print('Finished, quitting!')


if __name__ == '__main__':
    try:
        trio.run(main)
    except KeyboardInterrupt:
        print('Quitting!')
