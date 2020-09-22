/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#pragma once

namespace Pufferfish {
namespace HAL {

/**
 * An abstract class which represents a GPIO output
 */
class DigitalOutput {
 public:
  /**
   * Writes a digital output to the GPIO pin
   * @param output  true if desired output is active, false if non-active
   */
  virtual void write(bool output) = 0;
};

} // namespace HAL
} // namespace Pufferfish
