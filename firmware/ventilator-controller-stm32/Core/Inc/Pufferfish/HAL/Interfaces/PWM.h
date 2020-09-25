/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include <stdint.h>
#include "Pufferfish/Statuses.h"

namespace Pufferfish {
namespace HAL {

/**
 * A generic PWM driver
 */
class PWM {
 public:

  /**
   * Set a duty cycle of PWM, can be done when PWM is active
   * this function does NOT start the PWM output
   * @param duty    a number between 0.0 and 1.0 (inclusive) for the desired duty cycle
   */
  PWMStatus setDutyCycle(float duty);

  /**
   * Set a duty cycle of PWM, can be done when PWM is active
   * this function does NOT start the PWM output.
   * This is faster than setDutyCycle() as no floating point calculation is done
   * @param duty    an integer between 0 and getMaxDutyCycle() (inclusive) for the desired duty cycle
   */
  virtual void setDutyCycleRaw(uint32_t duty) = 0;

  /**
   * Start the PWM output
   * @return ok if the operation is successful, error code otherwise
   */
  virtual PWMStatus start() = 0;

  /**
   * Stop the PWM output
   * @return ok if the operation is successful, error code otherwise
   */
  virtual PWMStatus stop() = 0;

  /**
   * Returns the maximum duty cycle that can be set with setDutyCycleRaw()
   * @return the maximum duty cycle
   */
  virtual uint32_t getMaxDutyCycle() = 0;
};

} /* namespace HAL */
} /* namespace Pufferfish */
