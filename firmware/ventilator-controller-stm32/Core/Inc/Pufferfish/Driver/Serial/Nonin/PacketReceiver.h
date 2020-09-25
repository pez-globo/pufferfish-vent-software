/// PacketReceiver.h
/// This file has call and methos for packet receiver takes the input frames and
/// output the packet measurements on availability

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

#include <array>

#include "FrameBuffer.h"
#include "FrameReceiver.h"

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

/* Enum class for amplitude representation of signal quality  */
enum class SignalAmplitude {
  no_perfusion = 0,  /// No loss in signal quality
  red_perfusion,     /// Red Perfusion – Amplitude representation of low signal
                     /// quality
  yellow_perfusion,  /// YPRF: Yellow Perfusion – Amplitude representation of
                     /// medium signal quality
  green_perfusion    /// Green Perfusion – Amplitude representation of high signal
                     /// quality
};

/* Structure defines the sensor data in packet for measurements */
struct StatusByteStruct {
  bool bit7;
  bool sensor_disconnect;
  bool artifact;
  bool out_of_track;
  bool sensor_alarm;
  SignalAmplitude signal_perfusion;
};

/* Packet of 25 frames */
static const size_t packet_size = 25;
using Packet = std::array<Frame, packet_size>;
/* Status Byte error of 25 frames */
using StatusByteError = std::array<StatusByteStruct, packet_size>;
/* PLETH for 25 frames */
using Pleth = std::array<uint8_t, packet_size>;

/* Structure defines the sensor data in packet for measurements */
struct PacketMeasurements {
  /* Heart Rate and SpO2 measurements */
  uint16_t heart_rate;
  uint8_t spo2;
  uint8_t spo2_d;
  uint8_t spo2_d_fast;
  uint8_t spo2_d_beat;
  uint16_t e_heart_rate;
  uint8_t e_spo2;
  uint8_t e_spo2_d;
  uint16_t heart_rate_d;
  uint16_t e_heart_rate_d;
  uint8_t nonin_oem_revision;
  /* PLETH measurements */
  Pleth packet_pleth;
  /* StatusByteErrors measurements */
  std::array<bool, packet_size> bit7;
  std::array<bool, packet_size> sensor_disconnect;
  std::array<bool, packet_size> artifact;
  std::array<bool, packet_size> out_of_track;
  std::array<bool, packet_size> sensor_alarm;
  std::array<SignalAmplitude, packet_size> signal_perfusion;
};

static const uint8_t mask_6bit = 0x7F;

/**
 * @brief  Inline function to get the SpO2 data
 * @param  ByteData - Byte of SpO2 Data received from packet
 * @return Masked value of SpO2 from input Spo2Data
 */
inline uint16_t get_6bit_data(uint8_t byte_data) {
  /* Mask Bit0 to Bit6 for SpO2 data  */
  return byte_data & mask_6bit;
}

/**
 * @brief  Inline function to get the Heart rate from HR MSB and HR LSB
 * @param  msbByte - MSB to extract 2 bits
 * @param  lsbByte - MSB to extract 5 bits
 * @return Calculated 9 bit data from MSB and LSB
 */
inline uint16_t get_9bit_data(uint8_t msb_byte, uint8_t lsb_byte) {
  /* Pack 2 bits of MSB and 6 bits of LSB for 9 bits of heart rate data  */
  static const uint16_t msb_shift = 7;
  static const uint16_t mask_msb = 0x18;
  static const uint16_t mask_9bit = 0x01FF;
  const uint16_t msb =
      static_cast<uint16_t>(static_cast<uint16_t>(msb_byte) << msb_shift) & mask_msb;
  const uint16_t lsb = static_cast<uint16_t>(lsb_byte) & mask_6bit;
  return static_cast<uint16_t>(msb | lsb) & mask_9bit;
}

/*
 * PacketReceiver class for
 */
class PacketReceiver {
 public:
  /* PacketReceiver Input status */
  enum class PacketInputStatus {
    available = 0,  /// Input is available to read output
    waiting,        /// Input is wait to read more bytes
    not_available,  /// Input is not available
    missed_data     /// missed one or more frames in previous received packet
  };

  /* PacketReceiver Output status */
  enum class PacketOutputStatus {
    available = 0,  /// Output measurements are available
    waiting         /// Output is waiting to receive more byte for measurements
  };

  /**
   * @brief  Constructor for PacketReceiver
   * @param  None
   */
  PacketReceiver() = default;

  /**
   * @brief  Reset the packet on error occurred in input
   * @param  None
   * @return None
   */
  void reset();

  /**
   * @brief  Input is called until packet is filled
   * @param  frame input frame of 5 bytes
   * @param  frameIndex frame index of a packet
   * @return Packet input status on available of packet
   */
  PacketInputStatus input(const Frame &frame);

  /**
   * @brief  Output is called after the input status is available to read
   * measurements
   * @param  sensorMeasurements is updated on available of measurements
   * @return Packet Output status on available of measurements
   */
  PacketOutputStatus output(PacketMeasurements &sensor_measurements);

 private:
  /* Packet data received */
  Packet packet_data_{};

  /* Packet frame received length from PacketReceiver input */
  size_t received_length_ = packet_size;

  /* Input status for a packet */
  PacketInputStatus input_status_ = PacketInputStatus::waiting;
};

}  // namespace Nonin
}  // namespace Serial
}  // namespace Driver
}  // namespace Pufferfish
