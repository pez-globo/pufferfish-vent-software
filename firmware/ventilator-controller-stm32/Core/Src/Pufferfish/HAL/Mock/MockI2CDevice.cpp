/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Mock/MockI2CDevice.h"

namespace Pufferfish::HAL {

I2CDeviceStatus MockI2CDevice::read(uint8_t *buf, size_t count) {
  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;

  for (index = 0; index < minumum; index++) {
    buf[index] = read_buf_[index];
  }
  return I2CDeviceStatus::ok;
}

void MockI2CDevice::set_read(const uint8_t *buf, size_t count) {
  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;

  for (index = 0; index < minumum; index++) {
    read_buf_[index] = buf[index];
  }
}

I2CDeviceStatus MockI2CDevice::write(uint8_t *buf, size_t count) {
  size_t index = 0;

  write_count_ = (count < write_buf_size) ? count : write_buf_size;
  for (index = 0; index < write_count_; index++) {
    write_buf_[index] = buf[index];
  }

  return I2CDeviceStatus::ok;
}

void MockI2CDevice::get_write(uint8_t *buf, size_t &count) {
  size_t index = 0;

  count = write_count_;
  for (index = 0; index < count; index++) {
    buf[index] = write_buf_[index];
  }
}

}  // namespace Pufferfish::HAL
