/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"
#include "ShiftRegister.h"

namespace Pufferfish {
namespace Driver {

/**
 * Represents an output in shift register
 */
class ShiftedOutput : public HAL::DigitalOutput {
 public:
  ShiftedOutput(ShiftRegister &dev, uint8_t channel) : dev_(dev), channel(channel) {}

  void write(bool output) override;

 private:
  ShiftRegister &dev_;
  const uint8_t channel;
};

}  // namespace Driver
}  // namespace Pufferfish
