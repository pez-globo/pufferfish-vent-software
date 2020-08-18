/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: 
 *
 *  A wrapper for an SPI-based sensor
 */

#pragma once

#include "stm32h7xx_hal.h"

#include "Pufferfish/Statuses.h"

namespace Pufferfish {
namespace HAL {

/**
 * An abstract class which represents an SPI device with read/write interface
 */
class SPIDevice {
 public:
  /**
   * Reads data from the device
   * @param buf[out]    output of the data
   * @param count   the number of bytes to be read
   * @return ok on success, error code otherwise
   */
  virtual SPIDeviceStatus read(uint8_t *buf, size_t count) = 0;

  /**
   * Writes data to the device
   * @param buf the data to be written
   * @param count the number of bytes to write
   * @return ok on success, error code otherwise
   */
  virtual SPIDeviceStatus write(uint8_t *buf, size_t count) = 0;
};

}  // namespace HAL
}  // namespace Pufferfish
