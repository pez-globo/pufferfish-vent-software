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
   * @param  buf -
   * @param  count =
   * @return returns I2CDeviceStatus::ok
   */
  I2CDeviceStatus read(uint8_t *buf, size_t count) override;

  /**
   * @brief  Updates the private buffer variable mReadBuf with the input data
   * @param  buf
   * @param  count
   * @return None
   */
  void setRead(uint8_t *buf, size_t count);

  /**
   * @brief  Updates the private buffer variable mWriteBuf with the input data
   * @param  buf
   * @param  count
   * @return returns I2CDeviceStatus::ok
   */
  I2CDeviceStatus write(uint8_t *buf, size_t count) override;

  /**
   * @brief  Reads private buffer variable mWriteBuf and updates to buf
   * @param  buf
   * @param  count
   * @return None
   */
  void getWrite(uint8_t *buf, size_t count);

 private:

  uint8_t *mReadBuf, *mWriteBuf;

  size_t mCount;

};

}  // namespace HAL
}  // namespace Pufferfish
