/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *  Driver for TCA9548A I2C extender
 */

#include "Pufferfish/Driver/I2C/TCA9548A.h"

#include <climits>

namespace Pufferfish::Driver::I2C {

I2CDeviceStatus TCA9548A::select_slot(uint8_t slot) {
  if (slot > CHAR_BIT - 1U) {
    return I2CDeviceStatus::invalid_ext_slot;
  }

  if (slot == current_slot_) {
    // already on the right slot, can skip
    return I2CDeviceStatus::ok;
  }

  uint8_t cmd = 1U << slot;
  I2CDeviceStatus ret = dev_.write(&cmd, 1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  current_slot_ = slot;

  return I2CDeviceStatus::ok;
}

I2CDeviceStatus TCA9548A::read_control_reg(uint8_t &control_reg) {
  I2CDeviceStatus ret = dev_.read(&control_reg, 1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  return I2CDeviceStatus::ok;
}

I2CDeviceStatus TCA9548A::test() {
  I2CDeviceStatus status;
  current_slot_ = default_slot;
  static const uint8_t test_slot = 4;

  status = this->select_slot(test_slot);
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  uint8_t conreg = 0;
  status = this->read_control_reg(conreg);
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  if (conreg != (1U << test_slot)) {
    return I2CDeviceStatus::test_failed;
  }

  return I2CDeviceStatus::ok;
}

I2CDeviceStatus TCA9548A::reset() {
  return I2CDeviceStatus::not_supported;
}

}  // namespace Pufferfish::Driver::I2C
