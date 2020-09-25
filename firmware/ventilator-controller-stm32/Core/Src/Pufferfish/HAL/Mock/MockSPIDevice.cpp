/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * HALSPIDevice.cpp
 *
 *  Created on:
 *      Author:
 */

#include "Pufferfish/HAL/Mock/MockSPIDevice.h"

namespace Pufferfish::HAL {

SPIDeviceStatus MockSPIDevice::read(uint8_t *buf, size_t count) {
  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;

  for (index = 0; index < minumum; index++) {
    buf[index] = read_buf_[index];
  }
  return SPIDeviceStatus::ok;
}

void MockSPIDevice::set_read(const uint8_t *buf, size_t count) {
  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;

  for (index = 0; index < minumum; index++) {
    read_buf_[index] = buf[index];
  }
}

SPIDeviceStatus MockSPIDevice::write(uint8_t *buf, size_t count) {
  size_t index = 0;

  write_count_ = (count < write_buf_size) ? count : write_buf_size;
  for (index = 0; index < write_count_; index++) {
    write_buf_[index] = buf[index];
  }

  return SPIDeviceStatus::ok;
}

void MockSPIDevice::get_write(uint8_t *buf, size_t &count) {
  size_t index = 0;

  count = write_count_;
  for (index = 0; index < count; index++) {
    buf[index] = write_buf_[index];
  }
}

void MockSPIDevice::set_write_read(const uint8_t *buf, size_t count) {
  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;

  for (index = 0; index < minumum; index++) {
    set_write_read_buf_[index] = buf[index];
  }
}

SPIDeviceStatus MockSPIDevice::write_read(uint8_t *tx_buf, uint8_t *rx_buf, size_t count) {
  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;

  for (index = 0; index < minumum; index++) {
    rx_buf[index] = set_write_read_buf_[index];
  }

  get_write_read_count_ = (count < write_buf_size) ? count : write_buf_size;
  for (index = 0; index < write_count_; index++) {
    get_write_read_buf_[index] = tx_buf[index];
  }

  return SPIDeviceStatus::ok;
}

void MockSPIDevice::get_write_read(uint8_t *buf, size_t count) {
  size_t index = 0;

  count = get_write_read_count_;
  for (index = 0; index < count; index++) {
    buf[index] = get_write_read_buf_[index];
  }
}

void MockSPIDevice::chip_select(bool input) {
  last_cs_ = input;
}

bool MockSPIDevice::get_chip_select() const {
  return last_cs_;
}

}  // namespace Pufferfish::HAL
