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

#include "Pufferfish/Driver/Serial/Nonin/FrameReceiver.h"

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

/* Enum class for amplitude representation of signal quality  */
enum class SignalAmplitude {
  noPerfusion = 0,  /// No loss in signal quality
  redPerfusion,     /// Red Perfusion – Amplitude representation of low signal quality
  yellowPerfusion,  /// YPRF: Yellow Perfusion – Amplitude representation of medium signal quality
  greenPerfusion    /// Green Perfusion – Amplitude representation of high signal quality
};

/* Structure defines the sensor data in packet for measurements */
struct StatusByteStruct {
  bool bit7;
  bool sensorDisconnect;
  bool artifact;
  bool outOfTrack;
  bool sensorAlarm;
  SignalAmplitude SignalPerfusion;
};

/* Packet of 25 frames */
using Packet = std::array<Frame, 25>;
/* Status Byte error of 25 frames */
using StatusByteError = std::array<StatusByteStruct, 25>;
/* PLETH for 25 frames */
using Pleth = std::array<uint8_t, 25>;

/* Structure defines the sensor data in packet for measurements */
struct PacketMeasurements {
  /* Heart Rate and SpO2 measurements */
  uint16_t heartRate;
  uint8_t  SpO2;
  uint8_t SpO2D;
  uint8_t SpO2DFast;
  uint8_t SpO2DBeat;
  uint16_t eHeartRate;
  uint8_t eSpO2;
  uint8_t eSpO2D;
  uint16_t HeartRateD;
  uint16_t eHeartRateD;
  uint8_t  noninOEMRevision;
  /* PLETH measurements */
  Pleth packetPleth;
  /* StatusByteErrors measurements */
  bool bit7[25];
  bool sensorDisconnect[25];
  bool artifact[25];
  bool outOfTrack[25];
  bool sensorAlarm[25];
  SignalAmplitude SignalPerfusion[25];
};

/**
 * @brief  Inline function to get the SpO2 data
 * @param  ByteData - Byte of SpO2 Data received from packet
 * @return Masked value of SpO2 from input Spo2Data
 */
inline uint16_t get6BitData(uint8_t ByteData) {
  /* Mask Bit0 to Bit6 for SpO2 data  */
  return (ByteData & 0x07F);
}

/**
 * @brief  Inline function to get the Heart rate from HR MSB and HR LSB
 * @param  msbByte - MSB to extract 2 bits
 * @param  lsbByte - MSB to extract 5 bits
 * @return Calculated 9 bit data from MSB and LSB
 */
inline uint16_t get9BitData(uint8_t msbByte  , uint8_t lsbByte) {
  /* Pack 2 bits of MSB and 6 bits of LSB for 9 bits of heart rate data  */
  return (((static_cast<uint16_t>(msbByte) << 7) & 0x18) | (static_cast<uint16_t>(lsbByte) & 0x7F)) & 0x01FF;
}

/*
 * PacketReceiver class for
 */
class PacketReceiver {
 public:
  /* Size of Packet */
  static const size_t packetSize = 25;

  /* PacketReceiver Input status */
  enum class PacketInputStatus {
    available = 0,     /// Input is available to read output
    waiting,           /// Input is wait to read more bytes
    notAvailable,      /// Input is not available
    missedData         /// missed one or more frames in previous received packet
    };

  /* PacketReceiver Output status */
  enum class PacketOutputStatus {
    available = 0,   /// Output measurements are available
    waiting          /// Output is waiting to receive more byte for measurements
    };

  /**
   * @brief  Constructor for PacketReceiver
   * @param  None
   */
  PacketReceiver(){
  }

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
   * @brief  Output is called after the input status is available to read measurements
   * @param  sensorMeasurements is updated on available of measurements
   * @return Packet Output status on available of measurements
   */
  PacketOutputStatus output(PacketMeasurements &SensorMeasurements);

 private:
  /* Packet data received */
  Packet packet_data_;

  /* Packet frame received length from PacketReceiver input */
  size_t received_length_ = 25;

  /* Input status for a packet */
  PacketInputStatus input_status_;
};

} // Nonin
} // Serial
} // Driver
} // Pufferfish
