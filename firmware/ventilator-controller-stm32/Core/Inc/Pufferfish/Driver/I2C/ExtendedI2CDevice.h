/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#pragma once

#include "I2CMux.h"
#include "Pufferfish/HAL/HAL.h"
#include "Pufferfish/Types.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {

/**
 * An abstraction of an I2C device behind I2C multiplexer(s)
 */
class ExtendedI2CDevice : public HAL::I2CDevice {
 public:
  /**
   * Construct a new extended I2C device
   *
   * @param dev a device behind the multiplexer -- can be HALI2CDevice or another ExtendedI2CDevice
   * in a nested config
   * @param mux an I2C multiplexer
   * @param extSlot the location in the multiplexer to which the device is connected
   */
  ExtendedI2CDevice(I2CDevice &dev, I2CMux &mux, uint8_t ext_slot)
      : dev_(dev), mux_(mux), ext_slot(ext_slot) {}

  I2CDeviceStatus read(uint8_t *buf, size_t count) override;
  I2CDeviceStatus write(uint8_t *buf, size_t count) override;

 private:
  I2CDevice &dev_;
  I2CMux &mux_;
  const uint8_t ext_slot;
};

}  // namespace I2C
}  // namespace Driver
}  // namespace Pufferfish
