"""Trio I/O with sans-I/O protocol, running application."""

import logging
from typing import Type, List
import functools

import trio
import betterproto

from ventserver.integration import _trio
from ventserver.io.trio import _serial
from ventserver.io.trio import channels
from ventserver.io.trio import websocket
from ventserver.io.trio import fileio
from ventserver.io.trio import rotaryencoder
from ventserver.io.subprocess import frozen_frontend
from ventserver.protocols import server
from ventserver.protocols import exceptions
from ventserver.protocols.protobuf import mcu_pb


logger = logging.getLogger()
handler = logging.StreamHandler()
formatter = logging.Formatter(
    '%(asctime)s %(name)-12s %(levelname)-8s %(message)s'
)
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(logging.INFO)


async def main() -> None:
    """Set up wiring between subsystems and process until completion."""
    # pylint: disable=duplicate-code
    # Sans-I/O Protocols
    protocol = server.Protocol()

    # I/O Endpoints
    serial_endpoint = _serial.Driver()
    websocket_endpoint = websocket.Driver()
    filehandler = fileio.Handler()

    rotary_encoder = None
    try:
        rotary_encoder = rotaryencoder.Driver()
        try:
            await rotary_encoder.open()
        except exceptions.ProtocolError as err:
            exception = (
                "Unable to connect the rotary encoder, please check the "
                "serial connection. Check if the pigpiod service is running: %s"
            )
            rotary_encoder = None
            logger.error(exception, err)
    except NameError:
        rotary_encoder = None
        logger.warning('Running without rotary encoder support!')

    # Server Receive Outputs
    channel: channels.TrioChannel[
        server.ReceiveOutputEvent
    ] = channels.TrioChannel()

    # Initialize state with defaults
    all_states = protocol.receive.backend.all_states
    for state in all_states:
        if state is mcu_pb.ParametersRequest:
            all_states[state] = mcu_pb.ParametersRequest(
                mode=mcu_pb.VentilationMode.hfnc, ventilating=False,
                rr=30, fio2=60, flow=6
            )
        else:
            all_states[state] = state()

    # Load state from file
    states: List[Type[betterproto.Message]] = [
        mcu_pb.Parameters, mcu_pb.CycleMeasurements,
        mcu_pb.SensorMeasurements, mcu_pb.ParametersRequest
    ]
    await _trio.load_file_states(
        states, protocol, filehandler
    )

    # Turn off ventilation
    parameters_request = all_states[mcu_pb.ParametersRequest]
    if parameters_request is not None:
        parameters_request.ventilating = False

    try:
        async with channel.push_endpoint:
            async with trio.open_nursery() as nursery:
                nursery.start_soon(
                    # mypy only supports <= 5 args with trio-typing
                    functools.partial(_trio.process_all,
                                      channel=channel,
                                      push_endpoint=channel.push_endpoint),
                    protocol, serial_endpoint,
                    websocket_endpoint, rotary_encoder
                )

                while True:
                    receive_output = await channel.output()
                    await _trio.process_protocol_send(
                        receive_output.server_send, protocol,
                        serial_endpoint, websocket_endpoint,
                        filehandler
                    )

                    if receive_output.frontend_delayed:
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
