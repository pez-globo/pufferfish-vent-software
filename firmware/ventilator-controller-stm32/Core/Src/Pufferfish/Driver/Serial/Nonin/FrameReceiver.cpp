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

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

/**
 * validateStartOfFrame function is called on beginning to get the first frame and
 * on there is a loss of bytes or noise in the bytes of frame received.
 * Called based on startOfFrameStatus private variable
 */
bool validateStartOfFrame(const Frame &newFrame)
{
  /* Check for the byte 1 is 01 and 1st bit of byte 2 is 0x81 for the start of frame */
  if(newFrame[0] == 0x01 && (newFrame[1] & 0x81) == 0x81 )
  {
    /* Checksum validation */
    if (((newFrame[0]+newFrame[1]+newFrame[2]+newFrame[3]) % 256) == newFrame[4])
    {
      return true;
    }
  }
  return false;
}

/**
 * validateFrame function is called to validated the every frame for Status byte and Checksum.
 */
FrameReceiver::FrameInputStatus validateFrame(const Frame &newFrame)
{
  /* Check for the byte 1 is 01 and 1st bit of byte 2 is 0x80 for status byte */
  if(newFrame[0] == 0x01 && (newFrame[1] & 0x80) == 0x80 )
  {
    /* Checksum validation */
    if (((newFrame[0]+newFrame[1]+newFrame[2]+newFrame[3]) % 256) == newFrame[4])
    {
      /* Return the start of packet status as available */
      return FrameReceiver::FrameInputStatus::available;
    }
  }

  /* return the frame status as not available */
  return FrameReceiver::FrameInputStatus::framingError;
}

FrameReceiver::FrameInputStatus FrameReceiver::updateFrameBuffer(uint8_t newByte)
{
  Frame frameBuffer;

  /* Input the new byte received and check for frame availability */
  if(frameBuf.input(newByte) == BufferStatus::partial)
  {
    /* return false on frame is not available */
    return FrameInputStatus::waiting;
  }

  /* On frame available update the frameBuffer with new frame available */
  if(frameBuf.output(frameBuffer) != BufferStatus::ok)
  {
    /* return false on frame is not available */
    return FrameInputStatus::notAvailable;
  }

  /* On startOfFrameStatus false Validate the start of frame */
  if(startOfFrameStatus == false)
  {
    /* Validate the start of frame in the beginning of reading sensor data and
       on there is loss of bytes in a frame or noise occurred in recived frame due to which
       the validation of start of frame is called */
    if(validateStartOfFrame(frameBuffer) == true)
    {
      /* On start of frame available update the start frame status as true */
      startOfFrameStatus = true;
      /* On Start frame is available return status as available */
      return FrameInputStatus::available;
    }
    /* On non available of start frame left shift the frame buffer */
    frameBuf.shift_left();
    /* On Start frame is not available return status as waiting */
    return FrameInputStatus::waiting;
  }

  /* Validate the frame received and return the status */
  inputStatus =  validateFrame(frameBuffer);
  if(inputStatus == FrameInputStatus::framingError) {
    /* On checksum error update the start frame status as false */
    startOfFrameStatus = false;
  }
  return inputStatus;
}

FrameReceiver::FrameInputStatus FrameReceiver::input(const uint8_t newByte) {
  /* Update the frame buffer with new byte received */
  inputStatus = this->updateFrameBuffer(newByte);

  /* Return the input status */
  return inputStatus;
}

FrameReceiver::FrameOutputStatus FrameReceiver::output(Frame &frame) {
  /* Check for the frame availability in the buffer */
  if (inputStatus != FrameInputStatus::available){
    return FrameOutputStatus::waiting;
  }

  /* On frame available update the frameBuffer with new frame available */
  if(frameBuf.output(frame) != BufferStatus::ok)
  {
    /* return false on frame is not available */
    return FrameOutputStatus::waiting;
  }

  frameBuf.reset();

  /* Return frame is available */
  return FrameOutputStatus::available;
}

} // Nonin
} // Serial
} // Driver
} // Pufferfish
