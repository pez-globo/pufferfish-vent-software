/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *  Driver for TCA9548A I2C extender
 */

#pragma once

#include "I2CMux.h"
#include "Pufferfish/Driver/Testable.h"
#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {

/**
 * Driver for TCA9548A I2C multiplexer
 */
class TCA9548A : public I2CMux, public Testable {
 public:
  static constexpr uint16_t default_i2c_addr = 0x70;

  explicit TCA9548A(HAL::I2CDevice &dev) : dev_(dev) {}

  I2CDeviceStatus select_slot(uint8_t slot) override;

  /**
   * Read the current control register from the mux; see TCA datasheet
   * @param controlReg[out] the control register
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus read_control_reg(uint8_t &control_reg);

  [[nodiscard]] uint8_t get_current_slot() const override { return current_slot_; }

  I2CDeviceStatus test() override;
  I2CDeviceStatus reset() override;

 private:
  static const uint8_t default_slot = 0xff;
  uint8_t current_slot_ = default_slot;
  HAL::I2CDevice &dev_;
};

}  // namespace I2C
}  // namespace Driver
}  // namespace Pufferfish
