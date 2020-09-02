"""Trio WebSocket client."""

import logging

import trio

import trio_websocket  # type: ignore

from ventserver.io.trio import websocket


logger = logging.getLogger()
handler = logging.StreamHandler()
formatter = logging.Formatter(
    '%(asctime)s %(name)-12s %(levelname)-8s %(message)s'
)
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(logging.INFO)


async def announce_server(request: trio_websocket.WebSocketRequest) -> None:
    """Send an announcement every second to evey client connected."""
    logger.info('Received new connection request!')
    connection = await request.accept()
    logger.info('Accepted new connection!')
    try:
        while True:
            logger.info('Sending announcement...')
            await connection.send_message(
                'This is an announcement from the mock WebSocket server!'
            )
            await trio.sleep(1.0)
    except trio_websocket.ConnectionClosed:
        logger.info('Connection closed!')
    except trio.Cancelled:
        with trio.move_on_after(1) as cancel_scope:
            cancel_scope.shield = True
            logger.info('Closing connection...')
            await connection.aclose(reason='Keyboard interrupt')
            logger.info('Closed connection!')
            raise


async def main() -> None:
    """Print all received messages to console."""
    server = websocket.Server(request_handler=announce_server)
    logger.info('Serving...')
    await server.open()


if __name__ == '__main__':
    try:
        trio.run(main)
    except KeyboardInterrupt:
        logger.info('Quitting!')
