/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda s
 */

#include <Pufferfish/HAL/Mock/MockPWM.h>

namespace Pufferfish {
namespace HAL {

void MockPWM::setDutyCycleRaw(uint32_t duty) {
  mLastRawDuty = duty;
}

float MockPWM::getDutyCycleRaw()
{
  return mLastRawDuty;
}

PWMStatus MockPWM::start() {
  mState = true;
  return PWMStatus::ok;
}

bool MockPWM::getPWMState() {
  return mState;
}
PWMStatus MockPWM::stop() {
  mState = false;
  return PWMStatus::ok;
}

uint32_t MockPWM::getMaxDutyCycle() {
  return mLastMaxDuty;
}

void MockPWM::setMaxDutyCycle(uint32_t duty){
  mLastMaxDuty = duty;
}


} /* namespace HAL */
} /* namespace Pufferfish */
