/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * ShiftRegister.h
 *
 *  Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {

class ShiftRegister {
 public:
  static const int baud_rate = 20;
  static const uint8_t num_channels = 8;

  ShiftRegister(
      HAL::DigitalOutput &serial_in,
      HAL::DigitalOutput &serial_clock,
      HAL::DigitalOutput &r_clock,
      HAL::DigitalOutput &serial_clear)
      : serial_in_(serial_in),
        serial_clock_(serial_clock),
        r_clock_(r_clock),
        serial_clear_(serial_clear) {}

  void set_channel(uint8_t chan, bool out);
  [[nodiscard]] bool get_channel(uint8_t chan) const;
  void update();
  void clear();

 private:
  HAL::DigitalOutput &serial_in_;
  HAL::DigitalOutput &serial_clock_;
  HAL::DigitalOutput &r_clock_;
  HAL::DigitalOutput &serial_clear_;

  uint8_t output_reg_ = 0;
};

}  // namespace Driver
}  // namespace Pufferfish
