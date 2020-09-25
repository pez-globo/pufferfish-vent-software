/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 *
 *  A wrapper for an I2C-based sensor
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "Pufferfish/Statuses.h"

namespace Pufferfish {
namespace HAL {

/**
 * An abstract class which represents an arbitrary I2C device with read/write
 * interface
 */
class I2CDevice {
 public:
  /**
   * Reads data from the device
   * @param buf[out]    output of the data
   * @param count   the number of bytes to be read
   * @return ok on success, error code otherwise
   */
  virtual I2CDeviceStatus read(uint8_t *buf, size_t count) = 0;

  /**
   * Writes data to the device
   * @param buf the data to be written
   * @param count the number of bytes to write
   * @return ok on success, error code otherwise
   */
  virtual I2CDeviceStatus write(uint8_t *buf, size_t count) = 0;
};

}  // namespace HAL
}  // namespace Pufferfish
