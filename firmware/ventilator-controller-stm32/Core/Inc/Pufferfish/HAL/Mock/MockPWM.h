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
  MockPWM() {

  }

  /**
   * Set a duty cycle of PWM, updates the private variable mLastDuty
   * @param duty    a number between 0.0 and 1.0 (inclusive) for the desired duty cycle
   */
  PWMStatus setDutyCycle(float duty) override;


  /**
   * @brief  Get a duty cycle of PWM updated in setDutyCycle
   * @param  None
   * @return returns the duty cycle
   */
  float getDutyCycle();

  /**
   * Set a duty cycle of PWM, updates the private variable mLastRawDuty
   * @param duty    a number between 0.0 and 1.0 (inclusive) for the desired duty cycle
   */
  void setDutyCycleRaw(uint32_t duty) override;

  /**
   * @brief  Get a duty cycle of PWM updated in setDutyCycleRaw
   * @param  None
   * @return returns the duty cycle
   */
  float getDutyCycleRaw();
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
  uint32_t getMaxDutyCycle() override;


  /**
   * @brief  sets the max duty cycle
   * @param  duty    Privides the max duty cycle
   * @return None
   */
  void setMaxDutyCycle(uint32_t duty);

 private:

  float mLastDuty = 0.0;

  uint32_t mLastRawDuty = 0;

  uint32_t mLastMaxDuty = 0;

};

} /* namespace HAL */
} /* namespace Pufferfish */
