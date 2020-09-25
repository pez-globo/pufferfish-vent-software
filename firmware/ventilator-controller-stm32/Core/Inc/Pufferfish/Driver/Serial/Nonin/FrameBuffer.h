/// FrameBuffer.h
/// This file has class and methods to input the byte received from
/// sensor and output the frame on availability.

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

#pragma once

#include <stddef.h>
#include <array>
#include <stdint.h>
#include "Pufferfish/Statuses.h"
#include "Pufferfish/HAL/Types.h"

/* Frame */
const uint8_t frameMaxSize = 5;
using Frame = std::array<uint8_t, frameMaxSize>;

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

/**
 * FrameBuffer class to update buffer
 */
class FrameBuffer {
public:
  FrameBuffer(){
  }

  /**
   * @brief  Input method to update byte data into frame
   * @param  readByte byte data input
   * @return Frame buffer input status
   */
  BufferStatus input(const uint8_t readByte);

  /**
   * @brief  Output method to invoked on frame available
   * @param  frame data output
   * @return Frame buffer output status
   */
  BufferStatus output(Frame &frame);

  /**
   * @brief  reset the frame
   * @param  None
   * @return None
   */
  void reset();

  /**
   * @brief  Update the frame for Start of frame validation
   * @param  None
   * @return None
   */
  void shift_left();

private:
  /* Frame buffer */
  Frame frameBuffer;

  /* Length of frame received  */
  uint8_t received_length_ = 0;
};

} // Nonin
} // Serial
} // Driver
} // Pufferfish
