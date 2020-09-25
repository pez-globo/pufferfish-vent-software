/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * SensirionSensor.h
 *
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {

/**
 * A class for Sensiron flow sensors CRC checking protocol
 */
class SensirionSensor {
 public:
  explicit SensirionSensor(HAL::I2CDevice &dev) : dev_(dev) {}

  /**
   * Reads a data from the sensor, while performing CRC check
   *
   * @param buf[out] the buffer for the data output
   * @param count number of bytes to read, must be an even number
   * @param polynomial  a generator polynomial for CRC calculation
   * @param init    a initial value for CRC calculation
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus read_with_crc(uint8_t *buf, size_t count, uint8_t polynomial, uint8_t init);

  /**
   * Writes data to the device
   * @param buf the data to be written
   * @param count the number of bytes to write
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus write(uint8_t *buf, size_t count);

 private:
  HAL::I2CDevice &dev_;
};

}  // namespace I2C
}  // namespace Driver
}  // namespace Pufferfish
