/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda s
 */

#include <Pufferfish/HAL/Mock/MockPWM.h>

namespace Pufferfish::HAL {

void MockPWM::set_duty_cycle_raw(uint32_t duty) {
  last_raw_duty_ = duty;
}

float MockPWM::get_duty_cycle_raw() const {
  return last_raw_duty_;
}

PWMStatus MockPWM::start() {
  state_ = true;
  return PWMStatus::ok;
}

bool MockPWM::get_pwm_state() const {
  return state_;
}
PWMStatus MockPWM::stop() {
  state_ = false;
  return PWMStatus::ok;
}

uint32_t MockPWM::get_max_duty_cycle() {
  return last_max_duty_;
}

void MockPWM::set_max_duty_cycle(uint32_t duty) {
  last_max_duty_ = duty;
}

}  // namespace Pufferfish::HAL
