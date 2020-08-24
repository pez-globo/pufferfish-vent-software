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
 * Represents a GPIO input in STM32
 */
class MockDigitalInput{
 public:
  /**
   * Constructs a new Digital Input
   * @param m_port
   * @param m_pin
   */
  MockDigitalInput(GPIO_TypeDef &m_port, const uint16_t m_pin) : mPort(m_port), mPin(m_pin) {}

  /**
   * Reads a digital input from the GPIO pin
   * @return true/false based on the value set on setRead
   */
  bool read();

  /*
   * Test method to set value to be returned by the read method
   * @return None
   */
  void setRead(bool setting);

 private:
  GPIO_TypeDef &mPort;

  uint16_t const mPin;

  bool lastInput = 0;
};

} // namespace HAL
} // namespace Pufferfish
