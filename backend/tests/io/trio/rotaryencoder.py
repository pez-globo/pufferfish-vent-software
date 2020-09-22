"""Test Rotary Encoder connection and low-level driver."""
from ventserver.protocols import rotary_encoder
from ventserver.io.trio import rotaryencoder
import trio

async def main():
    """"""
    driver = rotaryencoder.RotaryEncoderDriver()
    protocol = rotary_encoder.ReceiveFilter()
    await driver.open()
    try:
        async for each in driver.receive_all():
            print("received:", each)
            protocol.input(each)
            out = protocol.output()
            print("received:",out)
            await trio.sleep(0.10)
    except Exception as err:
        print(err)
    await driver.close()

if __name__ == "__main__":

    trio.run(main)

