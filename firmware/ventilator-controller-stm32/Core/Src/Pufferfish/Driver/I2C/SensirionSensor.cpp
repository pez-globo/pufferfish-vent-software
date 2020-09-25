/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/I2C/SensirionSensor.h"

#include <array>

#include "Pufferfish/HAL/CRC.h"

namespace Pufferfish::Driver::I2C {

I2CDeviceStatus SensirionSensor::read_with_crc(
    uint8_t *buf, size_t count, uint8_t polynomial, uint8_t init) {
  if (count % 2 > 0) {
    return I2CDeviceStatus::invalid_arguments;
  }

  std::array<uint8_t, 3> buf_crc{};

  for (; count > 0; count -= 2, buf += 2) {
    I2CDeviceStatus ret = dev_.read(buf_crc.data(), buf_crc.size());
    if (ret != I2CDeviceStatus::ok) {
      return ret;
    }

    uint8_t expected =
        Pufferfish::HAL::compute_crc8(buf_crc.data(), 2, polynomial, init, false, false, 0x00);
    if (expected != buf_crc[2]) {
      return I2CDeviceStatus::crc_check_failed;
    }

    buf[0] = buf_crc[0];
    buf[1] = buf_crc[1];
  }

  return I2CDeviceStatus::ok;
}
I2CDeviceStatus SensirionSensor::write(uint8_t *buf, size_t count) {
  return dev_.write(buf, count);
}

}  // namespace Pufferfish::Driver::I2C
