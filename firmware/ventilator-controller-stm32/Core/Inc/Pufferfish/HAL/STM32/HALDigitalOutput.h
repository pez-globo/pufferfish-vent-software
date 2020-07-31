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
   * @param m_port  GPIO port of the MCU (A, B, ...)
   * @param m_pin   GPIO pin of the MCU (1, 2, ...)
   */
  HALDigitalOutput(GPIO_TypeDef &m_port, const uint16_t m_pin) : mPort(m_port), mPin(m_pin) {}

  void write(bool output) override;
 private:
  GPIO_TypeDef &mPort;
  uint16_t const mPin;
};

} // namespace HAL
} // namespace Pufferfish
