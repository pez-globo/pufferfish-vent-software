/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/I2C/ExtendedI2CDevice.h"

namespace Pufferfish::Driver::I2C {

I2CDeviceStatus ExtendedI2CDevice::read(uint8_t *buf, size_t count) {
  I2CDeviceStatus stat = mux_.select_slot(ext_slot);
  if (stat != I2CDeviceStatus::ok) {
    return stat;
  }

  return dev_.read(buf, count);
}

I2CDeviceStatus ExtendedI2CDevice::write(uint8_t *buf, size_t count) {
  I2CDeviceStatus stat = mux_.select_slot(ext_slot);
  if (stat != I2CDeviceStatus::ok) {
    return stat;
  }

  return dev_.write(buf, count);
}

}  // namespace Pufferfish::Driver::I2C
