"""" Support for Rotary Encoder communication """

from typing import Optional

import attr
import RPi.GPIO as GPIO


@attr.s(auto_attribs=True)
class RotaryEncoderProps:
    mode = GPIO.BCM 
    a_quad_pin: int = 18 # clk
    b_quad_pin: int = 17 # dt
    button_pin: int = 27 # sw
