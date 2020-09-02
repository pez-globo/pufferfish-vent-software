"""Trio I/O with sans-I/O protocol demo, running on real I/O."""

import logging

import trio

from ventserver.io.trio import _serial


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
    serial = _serial.Driver()

    async with serial:  # pylint: disable=not-async-context-manager
        # await trio.sleep(1.0)  # delay before send is given by serial.open()
        while True:
            await trio.sleep(1.0)
            logger.info('Sending ping...')
            await serial.send(b'ping!\0')
            logger.info('Received: %s', await serial.receive())


if __name__ == '__main__':
    try:
        trio.run(main)
    except KeyboardInterrupt:
        logger.info('Quitting!')
