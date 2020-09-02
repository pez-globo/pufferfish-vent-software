"""Trio WebSocket client."""

import logging

import trio

import trio_websocket  # type: ignore


logger = logging.getLogger()
handler = logging.StreamHandler()
formatter = logging.Formatter(
    '%(asctime)s %(name)-12s %(levelname)-8s %(message)s'
)
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(logging.INFO)


async def main() -> None:
    """Print all received messages to console."""
    logger.info('Opening connection...')
    while True:
        try:
            async with trio_websocket.open_websocket(
                    '127.0.0.1', 8000, '/', use_ssl=False
            ) as connection:
                logger.info('Opened connection!')
                while True:
                    logger.info('Sending ping message...')
                    await connection.send_message(b'ping!')
                    message = await connection.get_message()
                    logger.info('Received message: %s', message)
        except trio_websocket.HandshakeError:
            await trio.sleep(1)
        except trio_websocket.ConnectionClosed as exc:
            logger.info(
                'Connection closed! Attempting to reconnect...', exc_info=exc
            )
            await trio.sleep(1)


if __name__ == '__main__':
    try:
        trio.run(main)
    except KeyboardInterrupt:
        logger.info('Quitting!')
