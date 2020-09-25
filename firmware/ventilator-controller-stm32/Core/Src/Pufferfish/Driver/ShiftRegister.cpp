/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * ShiftRegister.h
 *
 *  Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/ShiftRegister.h"

namespace Pufferfish::Driver {

void ShiftRegister::set_channel(uint8_t chan, bool out) {
  if (out) {
    output_reg_ |= (1U << chan);
  } else {
    output_reg_ &= ~(1U << chan);
  }
}

bool ShiftRegister::get_channel(uint8_t chan) const {
  return (output_reg_ & (1U << chan)) != 0U;
}

void ShiftRegister::update() {
  // reset all pins to initial state
  serial_in_.write(false);
  serial_clear_.write(false);
  serial_clock_.write(false);
  r_clock_.write(false);
  HAL::delay_micros(baud_rate);

  for (int i = num_channels - 1; i >= 0; i--) {
    serial_in_.write(get_channel(i));
    HAL::delay_micros(baud_rate);
    serial_clock_.write(true);
    HAL::delay_micros(baud_rate);
    serial_clock_.write(false);
    HAL::delay_micros(baud_rate);
  }

  // set RClock to display all the LED at once
  r_clock_.write(true);
  HAL::delay_micros(baud_rate);
  r_clock_.write(false);
}

void ShiftRegister::clear() {
  serial_clear_.write(true);
  HAL::delay_micros(baud_rate);
  serial_clear_.write(false);
}

}  // namespace Pufferfish::Driver
