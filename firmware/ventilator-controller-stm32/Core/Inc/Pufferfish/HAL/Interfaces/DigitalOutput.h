/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "stm32h7xx_hal.h"

namespace Pufferfish {
namespace HAL {

/**
 * Represents a GPIO output in STM32
 */
class DigitalOutput {
 public:
  /**
   * Writes a digital output to the GPIO pin
   * @param output  true if desired output is HIGH, false if LOW
   */
  virtual void write(bool output) = 0;

};

} // namespace HAL
} // namespace Pufferfish
