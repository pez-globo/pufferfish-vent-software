/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 *
 *  A driver for the SFM3000 Low Pressure Drop Digital Flow Meter
 */

#include "Pufferfish/Driver/I2C/SFM3000.h"

#include <array>

#include "Pufferfish/HAL/Interfaces/Time.h"
#include "Pufferfish/HAL/STM32/Endian.h"
#include "Pufferfish/Util/Parse.h"

namespace Pufferfish::Driver::I2C {

I2CDeviceStatus SFM3000::start_measure() {
  static const uint16_t command = 0x1000;
  I2CDeviceStatus ret = sensirion_.write(command);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  measuring_ = true;
  return I2CDeviceStatus::ok;
}

I2CDeviceStatus SFM3000::serial_number(uint32_t &sn) {
  static const uint16_t command = 0x31ae;
  measuring_ = false;

  I2CDeviceStatus ret = sensirion_.write(command);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  std::array<uint8_t, sizeof(uint32_t)> buffer{};
  I2CDeviceStatus ret2 = sensirion_.read(buffer);
  if (ret2 != I2CDeviceStatus::ok) {
    return ret2;
  }

  sn = HAL::ntoh(Util::parse_network_order<uint32_t>(buffer.data(), buffer.size()));
  return I2CDeviceStatus::ok;
}

I2CDeviceStatus SFM3000::read_sample(SFM3000Sample &sample) {
  // read flow raw
  if (!measuring_) {
    I2CDeviceStatus ret = this->start_measure();
    if (ret != I2CDeviceStatus::ok) {
      return ret;
    }
    time_.delay(1);
  }

  std::array<uint8_t, sizeof(uint16_t)> buffer{};
  I2CDeviceStatus ret = sensirion_.read(buffer);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  sample.raw_flow = HAL::ntoh(Util::parse_network_order<uint16_t>(buffer.data(), buffer.size()));

  // convert to actual flow rate
  sample.flow =
      static_cast<float>(static_cast<int32_t>(sample.raw_flow) - offset_flow) / scale_factor_;

  return I2CDeviceStatus::ok;
}

I2CDeviceStatus SFM3000::reset() {
  static const uint16_t command = 0x2000;
  measuring_ = false;

  return sensirion_.write(command);
}

I2CDeviceStatus SFM3000::test() {
  // read serial number
  I2CDeviceStatus status;
  uint32_t sn = 0;

  status = this->serial_number(sn);
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  // start measurement
  status = this->start_measure();
  if (status != I2CDeviceStatus::ok) {
    return status;
  }
  SFM3000Sample sample{};

  // ignore the first read, might be invalid
  this->read_sample(sample);
  time_.delay(1);

  // read and verify output
  status = this->read_sample(sample);
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  // pressure range: -200 to 200
  static const float flow_min = -200;
  static const float flow_max = 200;
  if (sample.flow < flow_min || sample.flow > flow_max) {
    return I2CDeviceStatus::test_failed;
  }

  return I2CDeviceStatus::ok;
}

}  // namespace Pufferfish::Driver::I2C
