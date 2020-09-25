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

#include "Pufferfish/Driver/Serial/Nonin/FrameReceiver.h"
#include "Pufferfish/Driver/Serial/Nonin/PacketReceiver.h"
#include "Pufferfish/HAL/STM32/BufferedUART.h"

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

/* Define noninOEMUART for BufferedUART with 512 bytes of received buffer */
static const size_t nonin_oem_uart_size = 512;
using NoninOEMUART = HAL::BufferedUART<nonin_oem_uart_size, 1>;

/**
 * NoninOEM class to receive a byte from Nonin OEM III using UART and calculates
 * the measurements on complete packet availability and returns the measurements
 */
class NoninOEM {
 public:
  /* PacketReceiver Input status */
  enum class NoninPacketStatus {
    available = 0,  /// Packet/measurements is available
    waiting,        /// Packet/measurements is waiting to receive more bytes of data
    not_available,  /// Packet/measurements are not available
    framing_error,  /// Error in checksum or status byte or in byte 1 of a frame
    missed_data     /// Missed a packet due loss of frames
  };

  /**
   * Constructor for NoninOEM
   * @param  noninOEMUART BufferredUART with 512 bytes reception buffer
   */
  explicit NoninOEM(volatile NoninOEMUART &uart) : nonin_uart_(uart) {}

  /**
   * @brief  Method inputs the byte to packet and reads the packet measurements
   * on availability
   * @param  sensorMeasurements is updated on available of packet/measurements
   * @return returns the status of Nonin OEM III packet measurements
   */
  NoninPacketStatus output(PacketMeasurements &sensor_measurements);

 private:
  /* Create an object bufferredUART with 512 bytes of reception buffer */
  volatile NoninOEMUART &nonin_uart_;

  /* Create an object of FrameReceiver */
  FrameReceiver frame_receiver_;

  /* Create an object of PacketReceiver */
  PacketReceiver packet_receiver_;

  /* Frame Buffer stores bytes of data received from PacketReceiver input */
  // Frame frameBuffer;
};

}  // namespace Nonin
}  // namespace Serial
}  // namespace Driver
}  // namespace Pufferfish
