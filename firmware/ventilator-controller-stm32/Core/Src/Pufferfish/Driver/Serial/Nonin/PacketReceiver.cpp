/// PacketReceiver.cpp
/// Packet receiver takes the input frames and output the packet
/// measurements on availability

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

#include "Pufferfish/Driver/Serial/Nonin/PacketReceiver.h"

namespace Pufferfish {
namespace Driver {
namespace Serial {
namespace Nonin {

/*
 * @brief  Validates the status byte of frame
 * @param  frame index of packet received from sensor
 * @param  statusByte structure is updated based on input byteValue
 * @return None
 */
void readStatusByte(PacketMeasurements &sensorMeasurements,const size_t &frameIndex, const uint8_t &byteValue )
{
  /* BIT7: Always high */
  sensorMeasurements.bit7[frameIndex] = ((byteValue & 0x80) == 0x00) ? true : false;
  /* BIT6: SNSD: Sensor Disconnect – Sensor is not connected to oximeter or sensor is inoperable */
  sensorMeasurements.sensorDisconnect[frameIndex] = ((byteValue & 0x40) == 0x00) ? false : true;
  /* BIT5: ARTF: Artifact – A detected pulse beat didn’t match the current pulse interval */
  sensorMeasurements.artifact[frameIndex] = ((byteValue & 0x20) == 0x00) ? false : true;
  /* BIT4: OOT: Out Of Track – An absence of consecutive good pulse signals */
  sensorMeasurements.outOfTrack[frameIndex] = ((byteValue & 0x10) == 0x00) ? false : true;
  /* BIT3: SNSA: Sensor Alarm – Sensor is providing unusable data for analysis */
  sensorMeasurements.sensorAlarm[frameIndex] = ((byteValue & 0x08) == 0x00) ? false : true;

  /**
   * BIT2 and BIT1: YPRF: Yellow Perfusion – Amplitude representation of medium signal quality.
   * BIT2: RPRF: Red Perfusion – Amplitude representation of low signal quality.
   * BIT1: GPRF: Green Perfusion – Amplitude representation of high signal quality.
   */
  if (byteValue & 0x06) {
    sensorMeasurements.SignalPerfusion[frameIndex] = SignalAmplitude::yellowPerfusion;
  } else if (byteValue & 0x04) {
    sensorMeasurements.SignalPerfusion[frameIndex] = SignalAmplitude::redPerfusion;
  } else if (byteValue & 0x02) {
    sensorMeasurements.SignalPerfusion[frameIndex] = SignalAmplitude::greenPerfusion;
  } else {
    sensorMeasurements.SignalPerfusion[frameIndex] = SignalAmplitude::noPerfusion;
  }
}

void readPacketMeasurements(PacketMeasurements &sensorMeasurements,
                            const Packet &packetData) {
  /* 4th byte of a frame contains heart beat and SpO2 data */
  const uint8_t Byte1 = 1, Byte2 = 2, Byte3 = 3;
  size_t frameIndex;

  /**
   * Heart Rate: 4-beat average values in standard mode.
   * E-HR MSB : sensorData 1st frame of packet contains HR-Bit8 and HR-Bit7 in bit1 and bit0
   * E-HR LSB : sensorData 2nd frame of packet contains HR-Bit6 t0 HR-Bit0 in bit6 to bit0
   */
  sensorMeasurements.heartRate = get9BitData(packetData[0][Byte3], packetData[1][Byte3]);

  /**
   * SpO2: 4-beat average values in standard mode.
   * sensorData 3rd frame of packet contains SpO2-Bit6 t0 SpO2-Bit0 in bit6 to bit0
   */
  sensorMeasurements.SpO2 = get6BitData(packetData[2][Byte3]);

  /**
   * Nonin Oximeter Firmware Revision Level
   * sensorData 4th frame of packet contains Firmware Revision Level
   */
  sensorMeasurements.noninOEMRevision = packetData[3][Byte3];

  /**
   * SpO2-D: 4-beat average displayed values in display mode
   * sensorData 9th frame of packet contains SpO2-D-Bit6 t0 SpO2-D-Bit0 in bit6 to bit0
   */
  sensorMeasurements.SpO2D = get6BitData(packetData[8][Byte3]);

  /**
   * SpO2 Fast: Non-slew limited saturation with 4-beat averaging in standard mode.
   * sensorData 10th frame of packet contains SpO2-D-Bit6 t0 SpO2-D-Bit0 in bit6 to bit0
   */
  sensorMeasurements.SpO2DFast = get6BitData(packetData[9][Byte3]);

  /**
   * SpO2 B-B: Un-averaged, non-slew limited, beat to beat value in standard mode
   * sensorData 11th frame of packet contains SpO2-D-Bit6 t0 SpO2-D-Bit0 in bit6 to bit0
   */
  sensorMeasurements.SpO2DBeat = get6BitData(packetData[10][Byte3]);

  /**
   * E-Heart Rate: 8-beat average values in standard mode.
   * E-HR MSB : sensorData 14th frame of packet contains HR-Bit8 and HR-Bit7 in bit1 and bit0
   * E-HR LSB : sensorData 15th frame of packet contains HR-Bit6 t0 HR-Bit0 in bit6 to bit0
   */
  sensorMeasurements.eHeartRate = get9BitData(packetData[13][Byte3], packetData[14][Byte3]);

  /**
   * E-SpO2: 8-beat average values in standard mode.
   * sensorData 16th frame of packet contains SpO2-Bit6 t0 SpO2-Bit0 in bit6 to bit0
   */
  sensorMeasurements.eSpO2 = get6BitData(packetData[15][Byte3]);

  /**
   * E-SpO2-D: 8-beat average displayed values in display mode
   * sensorData 17th frame of packet contains SpO2-D-Bit6 to E-SpO2-D-Bit0 in bit6 to bit0
   */
  sensorMeasurements.eSpO2D = get6BitData(packetData[16][Byte3]);

  /**
   * HR-D: 8-beat average values in standard mode.
   * HR-D MSB : sensorData 20th frame of packet contains HR-Bit8 and HR-Bit7 in bit1 and bit0
   * HR-D LSB : sensorData 21st frame of packet contains HR-Bit6 t0 HR-Bit0 in bit6 to bit0
   */
  sensorMeasurements.HeartRateD = get9BitData(packetData[19][Byte3], packetData[20][Byte3]);

  /**
   * E-HR-D: 8-beat average displayed values in display mode
   * E-HR-D MSB : sensorData 22nd frame of packet contains HR-Bit8 and HR-Bit7 in bit1 and bit0
   * E-HR-D LSB : sensorData 23rd frame of packet contains HR-Bit6 t0 HR-Bit0 in bit6 to bit0
   */
  sensorMeasurements.eHeartRateD = get9BitData(packetData[21][Byte3], packetData[22][Byte3]);

  /* Update the PLETH and Status byte errors into the packet measurements */
  for (frameIndex = 0; frameIndex < PacketReceiver::packetSize; frameIndex++) {
    sensorMeasurements.packetPleth[frameIndex] = packetData[frameIndex][Byte2];
    readStatusByte(sensorMeasurements, frameIndex, packetData[frameIndex][Byte1]);
  }
}

PacketReceiver::PacketInputStatus PacketReceiver::input(const Frame &frame) {
  /* Check the frame received is first frame in the packet SYNC bit is 1 */
  if ( (frame[1] & 0x01) == 0x01) {
    if(received_length_ != 25){

      /* Update the frame index to 0 */
      received_length_ = 0;
      /* Update the frame received to packet */
      packet_data_[received_length_] = frame;
      /* Increment the frame index */
      received_length_ = received_length_ + 1;

      /* Update input status to missedData to report few frames of data are missed in previous packet */
      input_status_ = PacketInputStatus::missedData;
      return input_status_;
    }
    /* Update the frame index to 0 */
    received_length_ = 0;
  }

  /* Update the frame received to packet */
  packet_data_[received_length_] = frame;
  /* Increment the frame index */
  received_length_ = received_length_ + 1;

  /* Check for the packet data is complete */
  if (received_length_ != packetSize) {
    /* Update input status is waiting to receive more frames */
    input_status_ = PacketInputStatus::waiting;
    return input_status_;
  }

  /* Update input status is available for measurements */
  input_status_ = PacketInputStatus::available;
  return input_status_;
}

PacketReceiver::PacketOutputStatus PacketReceiver::output(PacketMeasurements &SensorMeasurements) {
  /* Check for the frame availability in the buffer */
  if (input_status_ != PacketInputStatus::available){
    /* Return PacketOutputStatus as waiting to receive packet data */
    return PacketOutputStatus::waiting;
  }
  /* Read PacketBuffer and Update the measurements */
  readPacketMeasurements(SensorMeasurements, packet_data_);

  /* Return Packet Output status as available */
  return PacketOutputStatus::available;
}

} // Nonin
} // Serial
} // Driver
} // Pufferfish
