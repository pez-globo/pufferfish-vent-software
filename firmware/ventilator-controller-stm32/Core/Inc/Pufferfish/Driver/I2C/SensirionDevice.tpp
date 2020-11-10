/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/CRC.h"
#include "SensirionDevice.h"

namespace Pufferfish::Driver::I2C {

template <size_t size>
I2CDeviceStatus SensirionDevice::read(
    std::array<uint8_t, size> &buf, uint8_t polynomial, uint8_t init) {
  static_assert(size % 2 == 0, "Read size must be an even number");

  std::array<uint8_t, 3 * size / 2> buf_with_crc{};
  I2CDeviceStatus ret = dev_.read(buf_with_crc.data(), buf_with_crc.size());
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  for (size_t word_start = 0; word_start < buf_with_crc.size(); word_start += 3) {
    uint8_t expected_crc = Pufferfish::HAL::compute_crc8(
        buf_with_crc.data() + word_start, sizeof(uint16_t), polynomial, init, false, false, 0x00);
    uint8_t received_crc = buf_with_crc[word_start + sizeof(uint16_t)];
    if (expected_crc != received_crc) {
      return I2CDeviceStatus::crc_check_failed;
    }

    buf[2 * word_start / 3] = buf_with_crc[word_start];
    buf[2 * word_start / 3 + sizeof(uint8_t)] = buf_with_crc[word_start + sizeof(uint8_t)];
  }

  return I2CDeviceStatus::ok;
}

}  // namespace Pufferfish::Driver::I2C
