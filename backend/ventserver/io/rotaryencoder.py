"""" Support for Rotary Encoder communication """

from typing import Optional

import attr
import RPi.GPIO as GPIO


@attr.s(auto_attribs=True)
class RotaryEncoderProps:
    mode = GPIO.BCM 
    clk_pin: int = 18 # A_COM
    dt_pin: int = 17 # B_COM
    button_pin: int = 27
