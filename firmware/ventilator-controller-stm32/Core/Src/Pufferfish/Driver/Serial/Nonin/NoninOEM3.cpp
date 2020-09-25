/// NoninOEM3.cpp
/// This file has methods to output the sensor measurements of NoninOEM
/// 3 sensor based on packet received.

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

#include "Pufferfish/Driver/Serial/Nonin/NoninOEM3.h"

namespace Pufferfish::Driver::Serial::Nonin {

NoninOEM::NoninPacketStatus NoninOEM::output(PacketMeasurements &sensor_measurements) {
  uint8_t read_byte = 0;
  Frame frame_buffer;

  /* Read a byte from BufferedUART */
  if (nonin_uart_.read(read_byte) == BufferStatus::empty) {
    /* Return waiting to receive new bytes of data from sensor */
    return NoninPacketStatus::waiting;
  }

  /* FrameReceiver */
  /* Input byte to frame receiver and validate the frame available */
  switch (frame_receiver_.input(read_byte)) {
    /* Return sensor status is waiting to receive more bytes of data */
    case FrameReceiver::FrameInputStatus::framing_error:
      return NoninPacketStatus::framing_error;

    /* Return sensor status is waiting to receive more bytes of data */
    case FrameReceiver::FrameInputStatus::waiting:
      return NoninPacketStatus::waiting;

    /* Return the frame status is not available */
    case FrameReceiver::FrameInputStatus::not_available:
      return NoninPacketStatus::not_available;

    /* On PacketInputStatus available continue */
    case FrameReceiver::FrameInputStatus::available:
      break;
  }

  /* On frame input available invoke output method to receive frame */
  if (frame_receiver_.output(frame_buffer) == FrameReceiver::FrameOutputStatus::waiting) {
    /* Return sensor status is waiting to receive more bytes of data */
    return NoninPacketStatus::waiting;
  }

  /* PaketParser */
  /* Input frame to packet and validate the frame available */
  switch (packet_receiver_.input(frame_buffer)) {
    /* Return sensor status is waiting to receive more frames of data */
    case PacketReceiver::PacketInputStatus::waiting:
      return NoninPacketStatus::waiting;

    /* Discard the packet due to status byte error, wait for the new packet to
     * receive */
    case PacketReceiver::PacketInputStatus::not_available:
      return NoninPacketStatus::not_available;

    /* Discard the packet due to status byte error, wait for the new packet to
     * receive */
    case PacketReceiver::PacketInputStatus::missed_data:
      return NoninPacketStatus::missed_data;

    /* On PacketInputStatus available continue */
    case PacketReceiver::PacketInputStatus::available:
      break;
  }

  /* On packet input available invoke output method to read sensor measurements
   */
  if (packet_receiver_.output(sensor_measurements) !=
      PacketReceiver::PacketOutputStatus::available) {
    /* Return sensor status is waiting to receive more bytes of data */
    return NoninPacketStatus::waiting;
  }

  /* Return available on measurements are available */
  return NoninPacketStatus::available;
}

}  // namespace Pufferfish::Driver::Serial::Nonin
