/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * STWD100.h
 *
 *      Author: Laxmanrao R
 *
 *  A driver for the STWD100 External watchdog
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {
namespace WatchDog {

/**
 * An abstract class for STWD100 WatchDog module
 */

class STWD100 {
 public:
  /**
   * Constructor for STWD100 external WatchDog driver
   * @param enablePin for STWD100
   * @param inputPin for STWD100
   */
  STWD100(HAL::DigitalOutput &enable_pin, HAL::DigitalOutput &input_pin)
      : enable_pin_(enable_pin), input_pin_(input_pin) {}

  /**
   * Enable watchdog timer
   */
  void enable();
  /**
   * Disable watchdog timer
   */
  void disable();
  /**
   * Reset watchdog timer when controller program experienced a freeze or hang
   */
  void toggle_input();

 private:
  static const uint32_t reset_time = 2;  /// TBD: resetTime as to be fixed
  HAL::DigitalOutput &enable_pin_;
  HAL::DigitalOutput &input_pin_;
};

}  // namespace WatchDog
}  // namespace Driver
}  // namespace Pufferfish
