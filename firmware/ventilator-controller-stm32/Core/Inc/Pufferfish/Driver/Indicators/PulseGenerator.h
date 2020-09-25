/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "Pufferfish/Driver/Indicators/DigitalFunction.h"

namespace Pufferfish {
namespace Driver {
namespace Indicators {

/*
 * PWMGenerator subclass of DigitalFunctionGenerator Generates the pulse for
 * the provided period and duty cycle
 */
class PWMGenerator : public DigitalFunctionGenerator {
 public:
  /**
   * @brief DigitalFunctionGenerator Constructor
   * @param PulsePeriod - pulse period (frequency)
   *        PulseDuty - pulse duty cycle
   */
  PWMGenerator(uint32_t pulse_period, uint32_t pulse_duty)
      : pulse_period_(pulse_period), pulse_duty_(pulse_duty) {}

  /**
   * @brief  start method updates the mReset to TRUE to start the alarm
   * @param  None
   * @return None
   */
  void start(uint32_t current_time) override;

  /**
   * @brief update method updates the mSwitching private variable based on
   *        period and duty cycle for the provided currentTime
   * @param currentTime current time in micro-seconds.
   * @return None
   */
  void update(uint32_t current_time) override;

  /**
   * @brief  returns the bool output of mSwitching
   * @param  None
   * @return TRUE/FALSE of pulse switching
   */
  bool output() override;

  /**
   * @brief stop method updates the mReset to FALSE to stop the alarm
   * @param None
   */
  void stop() override;

 private:
  /* Period/frequency and Pulse duty private variables */
  uint32_t pulse_period_ = 0, pulse_duty_ = 0;

  /* mOutput provides the pulse high or low for the provided current time */
  bool output_ = false;

  /* Stores the last cycle period of current time */
  uint32_t last_cycle_ = 0;

  /* Pulse generated based on mGenerating TRUE/FALSE */
  bool generating_ = false;
};

/**
 * PulsedPWMGenerator subclass of DigitalFunctionGenerator
 * Generates the pulse for the provided high period, low period and duty cycle
 */
class PulsedPWMGenerator : public DigitalFunctionGenerator {
 public:
  /**
   * @brief  PulsedPWMGenerator Constructor
   * @param  Low and high frequency objects of PWMGenerator
   */
  PulsedPWMGenerator(PWMGenerator &pulse_pwm_generator1, PWMGenerator &pulse_pwm_generator2)
      : pulse_pwm_generator1_(pulse_pwm_generator1), pulse_pwm_generator2_(pulse_pwm_generator2) {}

  /**
   * @brief  start method updates the mReset to TRUE to start the alarm
   * @param  None
   * @return None
   */
  void start(uint32_t current_time) override;

  /**
   * @brief  update method updates the mSwitching private variable based on
   *         period and duty cycle for the provided currentTime
   * @param  currentTime current time in micro-seconds.
   * @return None
   */
  void update(uint32_t current_time) override;

  /**
   * @brief  returns the bool output of mSwitching
   * @param  None
   * @return TRUE/FALSE of pulse switching
   */
  bool output() override;

  /**
   * @brief  stop method updates the mReset to FALSE to stop the alarm
   * @param  None
   * @return None
   */
  void stop() override;

 private:
  PWMGenerator &pulse_pwm_generator1_;
  PWMGenerator &pulse_pwm_generator2_;
};

}  // namespace Indicators
}  // namespace Driver
}  // namespace Pufferfish
