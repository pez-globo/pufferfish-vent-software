/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/DigitalOutput.h"

namespace Pufferfish {
namespace HAL {

/**
 * Represents a GPIO output in STM32, for mock functional testing
 */
class MockDigitalOutput : public DigitalOutput{
 public:
  /**
   * Constructs a new DigitalOutput
   * @param None
   */
  MockDigitalOutput(){

  }

  /**
   * Writes a digital output to the GPIO pin,
   * @param set the output value to lastOutput
   */
  void write(bool output) override;

  /**
   * Test method to get value passed on write method
   */
  bool getWrite();

 private:

  bool lastOutput;
};

} // namespace HAL
} // namespace Pufferfish
