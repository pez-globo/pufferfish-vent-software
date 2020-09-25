/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Solenoid.cpp
 *
 *  Created on: Jul 9, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/HALPWM.h"

namespace Pufferfish {
namespace HAL {

void HALPWM::setDutyCycleRaw(uint32_t duty) {
  __HAL_TIM_SET_COMPARE(&mHtim, mChannel, duty);
}

PWMStatus HALPWM::start() {
  HAL_StatusTypeDef stat = HAL_TIM_PWM_Start(&mHtim, mChannel);
  return stat == HAL_OK ? PWMStatus::ok : PWMStatus::halError;
}

PWMStatus HALPWM::stop() {
  HAL_StatusTypeDef stat = HAL_TIM_PWM_Stop(&mHtim, mChannel);
  return stat == HAL_OK ? PWMStatus::ok : PWMStatus::halError;
}

uint32_t HALPWM::getMaxDutyCycle() {
  return __HAL_TIM_GET_AUTORELOAD(&mHtim);
}

} /* namespace HAL */
} /* namespace Pufferfish */
