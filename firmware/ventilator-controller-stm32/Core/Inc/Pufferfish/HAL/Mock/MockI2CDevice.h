/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

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
  MockI2CDevice() {
  }

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
  void setRead(uint8_t *buf, size_t count);

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
  void getWrite(uint8_t *buf, size_t &count);

 private:
  /* Read and Write buffer size */
  static const uint8_t mReadBufSize = 50, mWriteBufSize = 50;

  /* The 50 bytes of read and write operation are used */
  uint8_t mReadBuf[mReadBufSize], mWriteBuf[mWriteBufSize];

  size_t mWriteCount;

};

}  // namespace HAL
}  // namespace Pufferfish
