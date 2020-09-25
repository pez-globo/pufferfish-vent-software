/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda s
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/PWM.h"

namespace Pufferfish {
namespace HAL {

/**
 * A generic PWM driver
 */
class MockPWM : public PWM {
 public:
  /**
   * Constructs a new MockPWM object
   * @param None
   */
  MockPWM() = default;

  /**
   * Set a duty cycle of PWM, updates the private variable mLastRawDuty
   * @param duty    a number between 0.0 and 1.0 (inclusive) for the desired
   * duty cycle
   */
  void set_duty_cycle_raw(uint32_t duty) override;

  /**
   * @brief  Get a duty cycle of PWM updated in setDutyCycleRaw
   * @param  None
   * @return returns the duty cycle
   */
  [[nodiscard]] float get_duty_cycle_raw() const;

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
   * Returns the maximum duty cycle updated in setMaxDutyCycle
   * @return the maximum duty cycle
   */
  uint32_t get_max_duty_cycle() override;

  /**
   * @brief  sets the max duty cycle
   * @param  duty    Provides the max duty cycle
   * @return None
   */
  void set_max_duty_cycle(uint32_t duty);

  /**
   * @brief  Provides the state of PWM
   * @param  None
   * @return State of PWM START(true)/STOP(false)
   */
  [[nodiscard]] bool get_pwm_state() const;

 private:
  /* Stores the Last raw duty cycle value */
  uint32_t last_raw_duty_ = 0;
  /* Stores the last Max duty cycle value */
  uint32_t last_max_duty_ = 0;

  bool state_ = false;
};

} /* namespace HAL */
} /* namespace Pufferfish */
