/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

namespace Pufferfish {
namespace HAL {

/**
 * Represents a GPIO input in STM32
 */
class DigitalInput {
 public:
  /**
   * Reads a digital input from the GPIO pin
   * @return true if the pin state is high, false if low
   */
  virtual bool read() = 0;

};

} // namespace HAL
} // namespace Pufferfish
