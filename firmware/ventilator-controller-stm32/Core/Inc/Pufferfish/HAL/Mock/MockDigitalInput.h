/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/DigitalInput.h"

namespace Pufferfish {
namespace HAL {

/**
 * Represents a GPIO input in STM32
 */
class MockDigitalInput : public DigitalInput {
 public:
  /**
   * Constructs a new Digital Input
   * @param None
   */
  MockDigitalInput() = default;

  /**
   * Reads a digital input from the GPIO pin
   * @return TRUE/FALSE based on the value set on setRead
   */
  bool read() override;

  /*
   * @brief  Test method to set value to be returned by the read method
   * @param  setting - Set TRUE/FASLE to read
   * @return None
   */
  void set_read(bool setting);

 private:
  bool last_input_ = false;
};

}  // namespace HAL
}  // namespace Pufferfish
