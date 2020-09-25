/// NoninOEM3.h
/// This file has class and methods to output the sensor measurements
/// of NoninOEM 3 sensor based on packet received.

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

#include "Pufferfish/Driver/Serial/Nonin/PacketReceiver.h"
#include "Pufferfish/Driver/Serial/Nonin/FrameReceiver.h"
#include "Pufferfish/HAL/STM32/BufferedUART.h"

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

/* Define noninOEMUART for BufferedUART with 512 bytes of received buffer */
using NoninOEMUART = HAL::BufferedUART<512, 10>;

/**
 * NoninOEM class to receive a byte from Nonin OEM III using UART and calculates the
 * measurements on complete packet availability and returns the measurements
 */
class NoninOEM {
 public:
  /* PacketReceiver Input status */
  enum class NoninPacketStatus {
    available = 0,  /// Packet/measurements is available
    waiting,        /// Packet/measurements is waiting to receive more bytes of data
    notAvailable,   /// Packet/measurements are not available
    framingError,   /// Error in checksum or status byte or in byte 1 of a frame
    missedData      /// Missed a packet due loss of frames
  };

  /**
   * Constructor for NoninOEM
   * @param  noninOEMUART BufferredUART with 512 bytes reception buffer
   */
  NoninOEM(volatile NoninOEMUART &uart) : noninUART(uart){
  }

  /**
   * @brief  Method inputs the byte to packet and reads the packet measurements on availability
   * @param  sensorMeasurements is updated on available of packet/measurements
   * @return returns the status of Nonin OEM III packet measurements
   */
  NoninPacketStatus output(PacketMeasurements &sensorMeasurements);

 private:
  /* Create an object bufferredUART with 512 bytes of reception buffer */
  volatile NoninOEMUART &noninUART;

  /* Create an object of FrameReceiver */
  FrameReceiver frameReceiver;

  /* Create an object of PacketReceiver */
  PacketReceiver packetReceiver;

  /* Frame Buffer stores bytes of data received from PacketReceiver input */
  // Frame frameBuffer;

};

} // Nonin
} // Serial
} // Driver
} // Pufferfish
