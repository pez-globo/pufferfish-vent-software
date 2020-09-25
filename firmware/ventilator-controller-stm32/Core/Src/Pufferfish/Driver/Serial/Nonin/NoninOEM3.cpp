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

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

NoninOEM::NoninPacketStatus NoninOEM::output(PacketMeasurements &sensorMeasurements){
  uint8_t readByte;
  Frame frameBuffer;

  /* Read a byte from BufferedUART */
  if(noninUART.read(readByte) == BufferStatus::empty)
  {
    /* Return waiting to receive new bytes of data from sensor */
    return NoninPacketStatus::waiting;
  }

  /* FrameReceiver */
  /* Input byte to frame receiver and validate the frame available */
  switch(frameReceiver.input(readByte)){
    /* Return sensor status is waiting to receive more bytes of data */
    case FrameReceiver::FrameInputStatus::framingError : return NoninPacketStatus::framingError;

    /* Return sensor status is waiting to receive more bytes of data */
    case FrameReceiver::FrameInputStatus::waiting : return NoninPacketStatus::waiting;

    /* Return the frame status is not available */
    case FrameReceiver::FrameInputStatus::notAvailable : return NoninPacketStatus::notAvailable;

    /* On PacketInputStatus available continue */
    case FrameReceiver::FrameInputStatus::available    : break;
  }

  /* On frame input available invoke output method to receive frame */
  if (frameReceiver.output(frameBuffer) == FrameReceiver::FrameOutputStatus::waiting)
  {
    /* Return sensor status is waiting to receive more bytes of data */
    return NoninPacketStatus::waiting;
  }

  /* PaketParser */
  /* Input frame to packet and validate the frame available */
  switch(packetReceiver.input(frameBuffer))
  {
    /* Return sensor status is waiting to receive more frames of data */
    case PacketReceiver::PacketInputStatus::waiting      : return NoninPacketStatus::waiting;

    /* Discard the packet due to status byte error, wait for the new packet to receive */
    case PacketReceiver::PacketInputStatus::notAvailable : return NoninPacketStatus::notAvailable;

    /* Discard the packet due to status byte error, wait for the new packet to receive */
    case PacketReceiver::PacketInputStatus::missedData : return NoninPacketStatus::missedData;

    /* On PacketInputStatus available continue */
    case PacketReceiver::PacketInputStatus::available    : break;
  }

  /* On packet input available invoke output method to read sensor measurements  */
  if (packetReceiver.output(sensorMeasurements) !=  PacketReceiver::PacketOutputStatus::available)
  {
    /* Return sensor status is waiting to receive more bytes of data */
    return NoninPacketStatus::waiting;
  }

  /* Return available on measurements are available */
  return NoninPacketStatus::available;
}

} // Nonin
} // Serial
} // Driver
} // Pufferfish
