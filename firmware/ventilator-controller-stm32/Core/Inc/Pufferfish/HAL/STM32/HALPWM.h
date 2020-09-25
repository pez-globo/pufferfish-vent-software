/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Solenoid.h
 *
 *  Created on: Jul 9, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/PWM.h"
#include "Pufferfish/Statuses.h"
#include "stm32h7xx_hal.h"

namespace Pufferfish {
namespace HAL {

/**
 * A generic PWM driver
 */
class HALPWM : public PWM {
 public:
  /**
   * Constructs a new PWM device
   * @param htim    a timer that is handling a PWM device
   * @param channel a channel of the timer that the device is connected, must be
   * init in PWM mode
   */
  HALPWM(TIM_HandleTypeDef &htim, uint32_t channel) : htim_(htim), channel(channel) {}

  /**
   * Set a duty cycle of PWM, can be done when PWM is active
   * this function does NOT start the PWM output.
   * This is faster than setDutyCycle() as no floating point calculation is done
   * @param duty    an integer between 0 and getMaxDutyCycle() (inclusive) for
   * the desired duty cycle
   */
  void set_duty_cycle_raw(uint32_t duty) override;

  /**
   * Start the PWM output
   * @return ok if the operation is successful, error code otherwise
   */
  PWMStatus start() override;

  /**
   * Stop the PWM output
   * @return ok if the operation is successful, error code otherwise
   */
  PWMStatus stop() override;

  /**
   * Returns the maximum duty cycle that can be set with setDutyCycleRaw()
   * @return the maximum duty cycle
   */
  uint32_t get_max_duty_cycle() override;

 private:
  TIM_HandleTypeDef &htim_;
  const uint32_t channel;
};

} /* namespace HAL */
} /* namespace Pufferfish */
