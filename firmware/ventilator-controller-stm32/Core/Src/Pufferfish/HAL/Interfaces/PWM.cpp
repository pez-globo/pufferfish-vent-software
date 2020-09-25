/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Interfaces/PWM.h"


namespace Pufferfish {
namespace HAL {

PWMStatus PWM::setDutyCycle(float duty) {
  if (0.0 < duty || duty > 1.0) {
    return PWMStatus::invalidDutyCycle;
  }

  setDutyCycleRaw(static_cast<uint32_t>(duty * getMaxDutyCycle()));
  return PWMStatus::ok;
}

} /* namespace HAL */
} /* namespace Pufferfish */
