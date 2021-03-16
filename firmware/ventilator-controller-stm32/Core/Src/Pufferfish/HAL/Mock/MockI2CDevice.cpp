/// MockI2CDevice.cpp
/// This file has methods for mock abstract interfaces for testing I2C
/// Device.

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

#include "Pufferfish/HAL/Mock/MockI2CDevice.h"

namespace Pufferfish::HAL {

I2CDeviceStatus MockI2CDevice::read(uint8_t *buf, size_t count) {
  if (read_buf_queue_.empty()) {
    return I2CDeviceStatus::no_new_data;
  }

  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;
  if (return_status_ != I2CDeviceStatus::ok) {
    return return_status_;
  }

  const auto &read_buf = read_buf_queue_.front();
  for (index = 0; index < minumum; index++) {
    buf[index] = read_buf[index];
  }
  read_buf_queue_.pop();

  return I2CDeviceStatus::ok;
}

void MockI2CDevice::add_read(const uint8_t *buf, size_t count) {
  size_t index = 0;
  size_t minumum = (count < read_buf_size) ? count : read_buf_size;

  read_buf_queue_.emplace();
  auto &read_buf = read_buf_queue_.back();

  for (index = 0; index < minumum; index++) {
    read_buf[index] = buf[index];
  }
}

I2CDeviceStatus MockI2CDevice::write(uint8_t *buf, size_t count) {
  size_t index = 0;
  if (return_status_ != I2CDeviceStatus::ok) {
    return return_status_;
  }

  write_buf_queue_.emplace();
  auto &write_buf = write_buf_queue_.back();

  write_count_ = (count < write_buf_size) ? count : write_buf_size;
  for (index = 0; index < write_count_; index++) {
    write_buf[index] = buf[index];
  }

  return I2CDeviceStatus::ok;
}

void MockI2CDevice::get_write(uint8_t *buf, size_t &count) {
  if (write_buf_queue_.empty()) {
    return;
  }

  size_t index = 0;

  const auto &write_buf = write_buf_queue_.front();

  count = write_count_;
  for (index = 0; index < count; index++) {
    buf[index] = write_buf[index];
  }

  write_buf_queue_.pop();
}

void MockI2CDevice::set_return_status(I2CDeviceStatus input) {
  return_status_ = input;
}

}  // namespace Pufferfish::HAL
