/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"
#include "stm32h7xx_hal.h"

namespace Pufferfish {
namespace HAL {

/**
 * Represents a GPIO output in STM32
 */
class HALDigitalOutput : public DigitalOutput {
 public:
  /**
   * Constructs a new DigitalOutput
   * @param port  GPIO port of the MCU (A, B, ...)
   * @param pin   GPIO pin of the MCU (1, 2, ...)
   * @param inverted   true if the output is active-low, false for active-high
   */
  HALDigitalOutput(GPIO_TypeDef &port, const uint16_t pin, const bool inverted = false)
      : port_(port), pin(pin), inverted(inverted) {}

  /**
   * Writes a digital output to the GPIO pin
   * @param output  true if desired output is active
   *  (HIGH when inverted=false, and LOW when inverted=true)
   *  false otherwise
   */
  void write(bool output) override;

 private:
  GPIO_TypeDef &port_;
  const uint16_t pin;
  const bool inverted;
};

}  // namespace HAL
}  // namespace Pufferfish
