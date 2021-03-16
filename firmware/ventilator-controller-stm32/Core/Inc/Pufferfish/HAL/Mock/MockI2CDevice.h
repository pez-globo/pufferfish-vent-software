/// MockBufferedUART.h
/// This file has mock class and methods for unit testing of I2C Device.

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

#include <array>
#include <queue>

#include "Pufferfish/HAL/Interfaces/I2CDevice.h"

namespace Pufferfish {
namespace HAL {

/**
 * MockI2CDevice class
 */
class MockI2CDevice : public I2CDevice {
 public:
  /**
   * Constructs an Mock I2C Device object
   * @param None
   */
  MockI2CDevice() = default;

  /**
   * @brief  Read method to read data from private buffer variable mReadBuf
   * @param  buf returns the data stored in setRead method
   * @param  count size of data to set
   * @return returns I2CDeviceStatus::ok
   */
  I2CDeviceStatus read(uint8_t *buf, size_t count) override;

  /**
   * @brief  Append the input data to the read queue
   * @param  buf the input data to append
   * @param  count size of input data to add to queue
   * @return None
   */
  void add_read(const uint8_t *buf, size_t count);

  /**
   * @brief  Updates the private buffer variable mWriteBuf with the input data
   * @param  buf update the private variable mWriteBuf with the buffer input
   * @param  count size of data to write
   * @return returns I2CDeviceStatus::ok
   */
  I2CDeviceStatus write(uint8_t *buf, size_t count) override;

  /**
   * @brief  Reads private buffer variable mWriteBuf and updates to buf
   * @param  buf returns the data stored in write method
   * @param  count buffer size to return
   * @return None
   */
  void get_write(uint8_t *buf, size_t &count);

  /**
   * @brief  sets return_status_ private variable
   * @param  input I2CDeviceStatus
   * @return None
   */
  void set_return_status(I2CDeviceStatus input);

 private:
  static const uint8_t read_buf_size = 50;
  static const uint8_t write_buf_size = 50;

  using ReadBuffer = std::array<uint8_t, read_buf_size>;
  using WriteBuffer = std::array<uint8_t, write_buf_size>;

  std::queue<ReadBuffer> read_buf_queue_;
  std::queue<WriteBuffer> write_buf_queue_;

  I2CDeviceStatus return_status_ = I2CDeviceStatus::ok;

  size_t write_count_ = 0;
};

}  // namespace HAL
}  // namespace Pufferfish
