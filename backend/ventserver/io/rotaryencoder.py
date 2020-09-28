"""" Support for Rotary Encoder communication """

import attr
import RPi.GPIO as GPIO     # type: ignore


@attr.s(auto_attribs=True)
class RotaryEncoderProps:
    """Rotary encoder communication properties."""
    mode = GPIO.BCM
    a_quad_pin: int = 18 # clk
    b_quad_pin: int = 17 # dt
    button_pin: int = 27 # sw
