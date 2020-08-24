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
 * Represents a GPIO output in STM32, for mock functional testing
 */
class MockDigitalOutput{
 public:
  /**
   * Constructs a new DigitalOutput
   * @param m_port
   * @param m_pin
   */
  MockDigitalOutput(GPIO_TypeDef &m_port, const uint16_t m_pin) : mPort(m_port), mPin(m_pin) {

  }

  /**
   * Writes a digital output to the GPIO pin,
   * @param set the output value to lastOutput
   */
  void write(bool output);

  /**
   * Test method to get value passed on write method
   */
  bool getWrite();

 private:

  GPIO_TypeDef &mPort;

  uint16_t const mPin;

  bool lastOutput;
};

} // namespace HAL
} // namespace Pufferfish
