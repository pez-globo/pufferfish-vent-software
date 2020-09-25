/// FrameReceiver.cpp
/// This file has methods to take input from FrameBuffer and output the
/// frame on availability.

// Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Pufferfish/Driver/Serial/Nonin/FrameReceiver.h"

namespace Pufferfish::Driver::Serial::Nonin {

/**
 * validateStartOfFrame function is called on beginning to get the first frame
 * and on there is a loss of bytes or noise in the bytes of frame received.
 * Called based on start_of_frame_status_ private variable
 */
bool validate_start_of_frame(const Frame &new_frame) {
  /* Check for the byte 1 is 01 and 1st bit of byte 2 is 0x81 for the start of
   * frame */
  static const uint8_t mask_start_of_packet = 0x81;
  if (new_frame[0] == 0x01 && (new_frame[1] & mask_start_of_packet) == mask_start_of_packet) {
    /* Checksum validation */
    if (((new_frame[0] + new_frame[1] + new_frame[2] + new_frame[3]) % (UINT8_MAX + 1)) ==
        new_frame[4]) {
      return true;
    }
  }
  return false;
}

/**
 * validateFrame function is called to validated the every frame for Status byte
 * and Checksum.
 */
FrameReceiver::FrameInputStatus validate_frame(const Frame &new_frame) {
  static const uint8_t mask_start_of_frame = 0x80;
  /* Check for the byte 1 is 01 and 1st bit of byte 2 is 0x80 for status byte */
  if (new_frame[0] == 0x01 && (new_frame[1] & mask_start_of_frame) == mask_start_of_frame) {
    /* Checksum validation */
    if (((new_frame[0] + new_frame[1] + new_frame[2] + new_frame[3]) % (UINT8_MAX + 1)) ==
        new_frame[4]) {
      /* Return the start of packet status as available */
      return FrameReceiver::FrameInputStatus::available;
    }
  }

  /* return the frame status as not available */
  return FrameReceiver::FrameInputStatus::framing_error;
}

FrameReceiver::FrameInputStatus FrameReceiver::update_frame_buffer(uint8_t new_byte) {
  Frame frame_buffer;

  /* Input the new byte received and check for frame availability */
  if (frame_buf_.input(new_byte) == BufferStatus::partial) {
    /* return false on frame is not available */
    return FrameInputStatus::waiting;
  }

  /* On frame available update the frameBuffer with new frame available */
  if (frame_buf_.output(frame_buffer) != BufferStatus::ok) {
    /* return false on frame is not available */
    return FrameInputStatus::not_available;
  }

  /* On start_of_frame_status_ false Validate the start of frame */
  if (!start_of_frame_status_) {
    /* Validate the start of frame in the beginning of reading sensor data and
       on there is loss of bytes in a frame or noise occurred in recived frame
       due to which the validation of start of frame is called */
    if (validate_start_of_frame(frame_buffer)) {
      /* On start of frame available update the start frame status as true */
      start_of_frame_status_ = true;
      /* On Start frame is available return status as available */
      return FrameInputStatus::available;
    }
    /* On non available of start frame left shift the frame buffer */
    frame_buf_.shift_left();
    /* On Start frame is not available return status as waiting */
    return FrameInputStatus::waiting;
  }

  /* Validate the frame received and return the status */
  input_status_ = validate_frame(frame_buffer);
  if (input_status_ == FrameInputStatus::framing_error) {
    /* On checksum error update the start frame status as false */
    start_of_frame_status_ = false;
  }
  return input_status_;
}

FrameReceiver::FrameInputStatus FrameReceiver::input(const uint8_t new_byte) {
  /* Update the frame buffer with new byte received */
  input_status_ = this->update_frame_buffer(new_byte);

  /* Return the input status */
  return input_status_;
}

FrameReceiver::FrameOutputStatus FrameReceiver::output(Frame &frame) {
  /* Check for the frame availability in the buffer */
  if (input_status_ != FrameInputStatus::available) {
    return FrameOutputStatus::waiting;
  }

  /* On frame available update the frameBuffer with new frame available */
  if (frame_buf_.output(frame) != BufferStatus::ok) {
    /* return false on frame is not available */
    return FrameOutputStatus::waiting;
  }

  frame_buf_.reset();

  /* Return frame is available */
  return FrameOutputStatus::available;
}

}  // namespace Pufferfish::Driver::Serial::Nonin
