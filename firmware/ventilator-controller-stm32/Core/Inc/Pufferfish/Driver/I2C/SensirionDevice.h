/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * SensirionSensor.h
 *
 *      Author: March Boonyapaluk
 */

#pragma once

#include <array>

#include "Pufferfish/HAL/Interfaces/I2CDevice.h"

namespace Pufferfish::Driver::I2C {

/**
 * A class for Sensiron devices with a CRC checking protocol
 */
class SensirionDevice {
 public:
  explicit SensirionDevice(HAL::I2CDevice &dev) : dev_(dev) {}

  /**
   * Reads a data from the sensor, while performing CRC check
   *
   * @param buf[out] the buffer for the data output
   * @param polynomial  a generator polynomial for CRC calculation
   * @param init    a initial value for CRC calculation
   * @tparam size number of bytes to read, must be an even number
   * @return ok on success, error code otherwise
   */
  template <size_t size>
  I2CDeviceStatus read(std::array<uint8_t, size> &buf, uint8_t polynomial, uint8_t init);

  /**
   * Writes a single-byte command to the device
   * @param byte_command the command to be sent
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus write(uint8_t byte_command);

  /**
   * Writes command to the device
   * @param command the command to be sent
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus write(uint16_t command);

  /**
   * Writes a command and arg to the device, and adds a CRC on the arg
   * @param command the command to be sent
   * @param arg the command's argument to be sent
   * @param polynomial  a generator polynomial for CRC calculation
   * @param init    a initial value for CRC calculation
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus write(uint16_t command, uint16_t arg, uint8_t polynomial, uint8_t init);

 private:
  HAL::I2CDevice &dev_;
};

}  // namespace Pufferfish::Driver::I2C

#include "SensirionDevice.tpp"
