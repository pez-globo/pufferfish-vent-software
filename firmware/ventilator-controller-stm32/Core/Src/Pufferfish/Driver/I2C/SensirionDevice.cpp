/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/I2C/SensirionDevice.h"

#include <array>

#include "Pufferfish/Util/Bytes.h"

namespace Pufferfish::Driver::I2C {

I2CDeviceStatus SensirionDevice::write(uint8_t command) {
  return dev_.write(&command, sizeof(uint8_t));
}

I2CDeviceStatus SensirionDevice::write(uint16_t command) {
  std::array<uint8_t, sizeof(uint16_t)> buf{
      {Util::get_byte<1>(command), Util::get_byte<0>(command)}};
  return dev_.write(buf.data(), buf.size());
}

I2CDeviceStatus SensirionDevice::write(uint16_t command, uint16_t arg) {
  static const size_t write_buf_size = 2 * sizeof(uint16_t) + sizeof(uint8_t);
  std::array<uint8_t, write_buf_size> write_buf{
      {Util::get_byte<1>(command),
       Util::get_byte<0>(command),
       Util::get_byte<1>(arg),
       Util::get_byte<0>(arg),
       0}};

  uint8_t crc = crc8_.compute(write_buf.data() + sizeof(uint16_t), sizeof(uint16_t));
  write_buf[4] = crc;

  return dev_.write(write_buf.data(), write_buf.size());
}

}  // namespace Pufferfish::Driver::I2C
