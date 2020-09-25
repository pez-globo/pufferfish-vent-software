"""Test the functionality of protocols.rotary_encoder classes."""

from typing import List, Tuple

import pytest as pt  # type: ignore

from ventserver.protocols import rotary_encoder
from ventserver.protocols.protobuf import frontend_pb as pb


re_examples = [
    (pb.RotaryEncoder(step=0, last_step_change=1600876180.591,
                      button_pressed=True, last_button_down=1600876182.923,
                      last_button_up=1600876182.821
                      ), 1600876182.923, True, 0),
    (pb.RotaryEncoder(step=0, last_step_change=1600876180.591,
                      button_pressed=False, last_button_down=1600876182.923,
                      last_button_up=1600876184.242
                      ), 1600876184.242, False, 0),
    (pb.RotaryEncoder(step=2, last_step_change=1600876184.850,
                      button_pressed=False, last_button_down=1600876184.141,
                      last_button_up=1600876184.242
                      ), 1600876184.850, False, 2),
    (pb.RotaryEncoder(step=3, last_step_change=1600876185.458,
                      button_pressed=False, last_button_down=1600876184.141,
                      last_button_up=1600876184.242
                      ), 1600876185.458, False, 3),
    (pb.RotaryEncoder(step=8, last_step_change=1600876185.559,
                      button_pressed=False, last_button_down=1600876184.141,
                      last_button_up=1600876184.242
                      ), 1600876185.559, False, 8),
    (pb.RotaryEncoder(step=14, last_step_change=1600876186.268,
                      button_pressed=False, last_button_down=1600876184.141,
                      last_button_up=1600876184.242
                      ), 1600876186.268, False, 14),
    (pb.RotaryEncoder(step=14, last_step_change=1600876186.268,
                      button_pressed=True, last_button_down=1600876186.878,
                      last_button_up=1600876184.242
                      ), 1600876186.878, True, 14),
    (pb.RotaryEncoder(step=0, last_step_change=1600876186.979,
                      button_pressed=False, last_button_down=1600876186.878,
                      last_button_up=1600876186.979
                      ), 1600876186.979, False, 0)
]


@pt.mark.parametrize('message_sequence', [re_examples])
def test_re_button_dial_time_change(
        message_sequence: List[Tuple[pb.RotaryEncoder, float, bool, int]]
) -> None:
    """Tests to check time changes for dial and button."""

    receiver = rotary_encoder.ReceiveFilter()
    last_step = 0
    for message, current_time, pressed, step in message_sequence:
        receive_event = rotary_encoder.ReceiveEvent(
            time=current_time,
            re_data=(step, pressed)
        )
        receiver.input(receive_event)
        output = receiver.output()
        assert output is not None
        if pressed:
            assert output.last_button_down == message.last_button_down
        else:
            assert output.last_button_up == message.last_button_up
        if last_step != step:
            assert output.last_step_change == message.last_step_change
        last_step = step
