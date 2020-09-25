/// FrameBuffer.cpp
/// This file has methods to input the byte received from sensor and
/// output the frame on availability.

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

#include "Pufferfish/Driver/Serial/Nonin/FrameBuffer.h"

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

BufferStatus FrameBuffer::input(const uint8_t byte) {
  /* Validate the frame buffer is already full */
  if (received_length_ == frameMaxSize) {
    return BufferStatus::full;
  }
  /* Update the frameBuffer with new byte received */
  frameBuffer[received_length_] = byte;

  /* Increment the frame buffer index */
  received_length_++;

  /* On frame index is equal to frame size reture frame status as Ok */
  if (received_length_ == frameMaxSize) {
    return BufferStatus::ok;
  }
  
  /* Return the frame buffer status as partial and frame buffer should be updated with new bytes */
  return BufferStatus::partial;
}

BufferStatus FrameBuffer::output(Frame &frame) {
  /* Check for available of frame */
  if (received_length_ != frameMaxSize) {
    /* Return frame buffer is partial update */
    return BufferStatus::partial;
  }
  /* Update the frame buffer */
  frame = frameBuffer;

  /* Return ok on frame buffer updated */
  return BufferStatus::ok;
}

void FrameBuffer::reset(){
  /* Update the index of frame buffer to zero */
  received_length_ = 0;
}

void FrameBuffer::shift_left(){
  uint8_t index;
  /* On no frame data available frameBuffer and frameIndex are not updated */
  if (received_length_ > 0){
    /* Update the frame buffer and index to receive new byte data */
    for(index = 0; index < (received_length_-1);index++){
      frameBuffer[index] = frameBuffer[index+1];
    }
    received_length_--;
  }
}

} // Nonin
} // Serial
} // Driver
} // Pufferfish
