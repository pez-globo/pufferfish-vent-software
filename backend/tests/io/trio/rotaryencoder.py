"""Test Rotary Encoder connection and low-level driver."""
from ventserver.protocols import rotary_encoder
from ventserver.io.trio import rotaryencoder
import trio
import RPi.GPIO as GPIO
import pickle

GPIO.setmode(GPIO.BCM)

async def main():
    """"""
    driver = rotaryencoder.RotaryEncoderDriver()
    protocol = rotary_encoder.ReceiveFilter()
    await driver.open()
    samples = list()
    try:
        async for each in driver.receive_all():
            protocol.input(each)
            out = protocol.output()
            samples.append(out)
            print("received:",out)
            await trio.sleep(0.10)
    except Exception as err:
        print(err)
    finally:
        with open("re_samples.pckl","wb") as fi: 
            fi.write(pickle.dumps(samples))
    await driver.close()

if __name__ == "__main__":

    trio.run(main)

