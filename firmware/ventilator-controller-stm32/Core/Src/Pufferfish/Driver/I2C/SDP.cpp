/*
 *  Copyright (c) 2018, Sensirion AG <joahnnes.winkelmann@sensirion.com>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the Sensirion AG nor the names of its
 *        contributors may be used to endorse or promote products derived
 *        from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Modified work Copyright 2020, the Pez Globo team and the Pufferfish project
 * contributors
 *
 * SDP.cpp
 * March Boonyapaluk: A driver for the SDPxx flow sensor; modified from
 * Sensirion Arduino Library
 */

#include "Pufferfish/Driver/I2C/SDP.h"
// FIXME: After HAL/Interfaces/Endian.h is created, change to
// "Pufferfish/HAL/HAL.h"
#include "Pufferfish/HAL/Interfaces/Time.h"
#include "Pufferfish/HAL/STM32/Endian.h"
#include "Pufferfish/Util/Bytes.h"
#include "Pufferfish/Util/Parse.h"

namespace Pufferfish::Driver::I2C {

I2CDeviceStatus SDPSensor::serial_number(uint32_t &pn, uint64_t &sn) {
  measuring_ = false;

  // try to read product id
  static const uint16_t cmd1 = 0x367c;
  static const uint16_t cmd2 = 0xe102;

  I2CDeviceStatus ret = sensirion_.write(cmd1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  ret = sensirion_.write(cmd2);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  static const size_t pn_data_size = 12;
  std::array<uint8_t, pn_data_size> data{};
  I2CDeviceStatus ret2 = sensirion_.read(data);
  if (ret2 != I2CDeviceStatus::ok) {
    return ret2;
  }

  // read 32 bits product number
  pn = HAL::ntoh(Util::parse_network_order<uint32_t>(data.data(), sizeof(uint32_t)));

  // read 64 bits serial number
  static const size_t sn_offset = 4;
  sn = HAL::ntoh(Util::parse_network_order<uint64_t>(&(data[sn_offset]), sizeof(uint64_t)));

  return I2CDeviceStatus::ok;
}

I2CDeviceStatus SDPSensor::start_continuous(bool averaging) {
  static const uint8_t command_prefix = 0x36;
  static const uint8_t command_dp_average = 0x15;
  static const uint8_t command_dp_none = 0x1e;

  uint16_t command = Util::set_byte<1, uint16_t>(command_prefix);
  if (averaging) {
    command += command_dp_average;
  } else {
    command += command_dp_none;
  }
  I2CDeviceStatus ret = sensirion_.write(command);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  measuring_ = true;

  return I2CDeviceStatus::ok;
}

/// void SDPSensor::start_continuous_wait(bool stabilize) {
///   static const uint32_t wait = 8;
///   static const uint32_t stabilize_wait = 12;
///   //time_.delay(wait);
///   if (stabilize) {
///     //TODO:TBD time_.delay(stabilize_wait);
///   }
/// }

I2CDeviceStatus SDPSensor::read_full_sample(SDPSample &sample) {
  if (!measuring_) {
    I2CDeviceStatus ret = this->start_continuous();
    if (ret != I2CDeviceStatus::ok) {
      return ret;
    }
  }

  std::array<uint8_t, full_reading_size> data{};

  I2CDeviceStatus ret = sensirion_.read(data);
  if (ret == I2CDeviceStatus::read_error) {
    /// get NACK, no new data is available
    return I2CDeviceStatus::no_new_data;
  }
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  if (data[full_reading_size - 2] != 0 && data[full_reading_size - 1] != 0) {
    SDPSensor::parse_reading(data, sample);
  } else {
    return I2CDeviceStatus::no_new_data;
  }

  return I2CDeviceStatus::ok;
}

I2CDeviceStatus SDPSensor::stop_continuous() {
  static const uint16_t command = 0x3ff9;

  measuring_ = false;
  return sensirion_.write(command);
}

void SDPSensor::parse_reading(
    const std::array<uint8_t, full_reading_size> &data, SDPSample &sample) {
  static const size_t dp_raw_high = 0;
  static const size_t dp_raw_low = 1;
  static const size_t temp_raw_high = 2;
  static const size_t temp_raw_low = 3;
  static const size_t dp_scale_high = 4;
  static const size_t dp_scale_low = 5;
  int16_t dp_raw = (data[dp_raw_high] << static_cast<uint16_t>(CHAR_BIT)) + data[dp_raw_low];
  int16_t temp_raw = (data[temp_raw_high] << static_cast<uint16_t>(CHAR_BIT)) + data[temp_raw_low];
  int16_t dp_scale = (data[dp_scale_high] << static_cast<uint16_t>(CHAR_BIT)) + data[dp_scale_low];

  if (dp_scale != 0) {
    sample.differential_pressure = static_cast<float>(dp_raw) / static_cast<float>(dp_scale);
  }

  static const float temp_scale = 200;
  sample.temperature = static_cast<float>(temp_raw) / temp_scale;
}

I2CDeviceStatus SDPSensor::reset() {
  static const uint8_t command = 0x06;
  measuring_ = false;
  return sensirion_.write(command);
}

I2CDeviceStatus SDPSensor::test() {
  /// stop any measurement first
  this->stop_continuous();

  /// read and verify serial number
  I2CDeviceStatus status;
  uint32_t pn = 0;
  uint64_t sn = 0;

  status = this->serial_number(pn, sn);
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  /// NOTE(march): the pn number is not set in my SDP for some reason,
  ///   should re-enable this for a production sensor

  /// product number should be 0x030xxxxx
  ///  if ((pn & 0xFFF00000) != 0x03000000) {
  ///    return I2CDeviceStatus::testFailed;
  ///  }

  // try soft resetting
  status = this->reset();
  if (status != I2CDeviceStatus::ok) {
    return status;
  }
  static const uint8_t reset_delay = 25;
  time_.delay(reset_delay);

  // try start measurement
  static const uint8_t command_delay = 3;
  time_.delay(command_delay);
  status = this->start_continuous(true);
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  // FIXME: Removed start_continuous_wait static method
  // this->start_continuous_wait(true);
  // time_.delay(20);

  // read & verify output
  // three attempts for measuring data
  static const size_t read_attempts = 3;
  size_t i = 0;
  SDPSample sample{};
  for (i = 0; i < read_attempts; i++) {
    time_.delay(command_delay);
    status = this->read_full_sample(sample);

    if (status == I2CDeviceStatus::ok) {
      break;
    }
    if (status != I2CDeviceStatus::no_new_data) {
      return status;
    }
  }

  if (i == read_attempts) {
    return I2CDeviceStatus::test_failed;
  }

  // pressure range: -500 to 500
  static const float min_dp = -500;
  static const float max_dp = 500;
  if (sample.differential_pressure < min_dp || sample.differential_pressure > max_dp) {
    return I2CDeviceStatus::test_failed;
  }

  // operating temp range: -40 to +85
  static const float min_temp = -40;
  static const float max_temp = 85;
  if (sample.temperature < min_temp || sample.temperature > max_temp) {
    return I2CDeviceStatus::test_failed;
  }

  // stop reading
  time_.delay(command_delay);
  status = this->stop_continuous();
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  return I2CDeviceStatus::ok;
}

I2CDeviceStatus SDPSensor::read_pressure_sample(
    int16_t differential_pressure_scale, float &differential_pressure) {
  static const uint8_t data_len = 2;
  std::array<uint8_t, data_len> data{{0}};

  I2CDeviceStatus ret = sensirion_.read(data);
  if (ret == I2CDeviceStatus::read_error) {
    // get NACK, no new data is available
    return I2CDeviceStatus::no_new_data;
  }

  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  auto pressure_sample =
      static_cast<int16_t>((data[0] << static_cast<uint8_t>(CHAR_BIT)) + data[1]);
  differential_pressure =
      static_cast<float>(pressure_sample) / static_cast<float>(differential_pressure_scale);
  return I2CDeviceStatus::ok;
}

}  // namespace Pufferfish::Driver::I2C
