/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/Driver/Indicators/PulseGenerator.h"

/**
 * @brief  Inline function to validate the value in the range of max and min
 * @param  value input value to validate the range
 * @param  max input value of maximum range
 * @param  min input value of minimum range
 * @return TRUE/FALSE based on value is in range of max and min values
 */
inline bool out_of_range(uint32_t value, uint32_t min, uint32_t max) {
  return (value > max) || (value < min);
}

namespace Pufferfish::Driver::Indicators {

void PWMGenerator::start(uint32_t current_time) {
  /* Set the value to true to start the pulse */
  generating_ = true;
  /* Update the LastCycle with currentTime to start the pulse with raising edge
   */
  last_cycle_ = current_time;
  /* Invoke the update function of this object */
  this->update(current_time);
}

void PWMGenerator::update(uint32_t current_time) {
  /* validate the pulse duration */
  uint32_t pulse_duration = current_time - last_cycle_;

  /* Validate the reset value start the pulse */
  if (generating_) {
    /* trimming the duration to be within [0, pulse_period_) */
    if (pulse_duration >= pulse_period_) {
      /* update the mLastCycle with current time */
      last_cycle_ = current_time;
    }
    /* Trimming the high or low based on pulse duty */
    output_ = (pulse_duration < pulse_duty_);
  } else {
    output_ = false;
  }
  /* Validate the saturation of pulseDuration of frequency with in maximum
     and minimum range */
  if (out_of_range(pulse_duration, 0, pulse_period_)) {
    last_cycle_ = current_time;
  }
}

bool PWMGenerator::output() {
  return output_;
}

void PWMGenerator::stop() {
  /* Set the reset value to false to stop the pulse */
  generating_ = false;
}

void PulsedPWMGenerator::start(uint32_t current_time) {
  /* Start the pulse of low and high period/frequency */
  pulse_pwm_generator1_.start(current_time);
  pulse_pwm_generator2_.start(current_time);
}

void PulsedPWMGenerator::update(uint32_t current_time) {
  /* Update the pulse of low and high period/frequency */
  pulse_pwm_generator1_.update(current_time);
  pulse_pwm_generator2_.update(current_time);
}

bool PulsedPWMGenerator::output() {
  /* Get the output of low and high period/frequency */
  bool output_pulse1 = pulse_pwm_generator1_.output();
  bool output_pulse2 = pulse_pwm_generator2_.output();

  /* return the AND of pulses of high and low frequency */
  return output_pulse1 && output_pulse2;
}

void PulsedPWMGenerator::stop() {
  /* Stop the pulse of high and low frequency */
  pulse_pwm_generator1_.stop();
  pulse_pwm_generator2_.stop();
}

}  // namespace Pufferfish::Driver::Indicators
