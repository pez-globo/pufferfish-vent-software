/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "stm32h7xx_hal.h"
#include "Pufferfish/HAL/Interfaces/DigitalInput.h"

namespace Pufferfish {
namespace HAL {

/**
 * Represents a GPIO input in STM32
 */
class HALDigitalInput : public DigitalInput {
 public:
  /**
   * Constructs a new HALDigitalInput
   * @param m_port  GPIO port of the MCU (A, B, ...)
   * @param m_pin   GPIO pin of the MCU (1, 2, ...)
   */
  HALDigitalInput(GPIO_TypeDef &port, const uint16_t pin, const bool inverted =
  false)
      :
      mPort(port),
      mPin(pin),
      mInverted(inverted) {
  }

  /**
   * Reads a digital input from the GPIO pin
   * @return true if the pin is active (HIGH when inverted=false,
   *  and LOW when inverted=true), false otherwise
   */
  bool read();
 private:
  GPIO_TypeDef &mPort;
  const uint16_t mPin;
  const bool mInverted;
};

} // namespace HAL
} // namespace Pufferfish
