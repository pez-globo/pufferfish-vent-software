/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include <array>

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
   * @brief  Updates the private buffer variable mReadBuf with the input data
   * @param  buf update the private variable mReadBuf with the buffer input
   * @param  count size of data to set
   * @return None
   */
  void set_read(const uint8_t *buf, size_t count);

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

 private:
  static const uint8_t read_buf_size = 50;
  static const uint8_t write_buf_size = 50;

  std::array<uint8_t, read_buf_size> read_buf_{};
  std::array<uint8_t, write_buf_size> write_buf_{};

  size_t write_count_ = 0;
};

}  // namespace HAL
}  // namespace Pufferfish
