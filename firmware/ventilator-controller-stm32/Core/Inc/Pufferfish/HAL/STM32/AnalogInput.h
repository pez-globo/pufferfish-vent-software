/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "stm32h7xx_hal.h"

/*
 * FIXME: Need to find proper header file to define this macro
 * Defines the timeout for the Adc poll conversion timeout
 */
#define ADC_POLL_TIMEOUT 10

namespace Pufferfish {
namespace HAL {

/**
 * Represents a Analog input in STM32
 */
class AnalogInput{

 public:
  /**
   * Constructs a new Analog Input
   * @param hadc and tTimeout
   */
  AnalogInput(ADC_HandleTypeDef *hadc, uint32_t tTimeout) : AdcInput(hadc),  Timeout(tTimeout){}

  /**
   * Calls HAL_ADC_Start
   * @param  None
   * @return HAL_StatusTypeDef returns HAL_ERROR/HAL_TIMEOUT/HAL_OK
   */
  HAL_StatusTypeDef Start();

  /**
   * Reads a Analog conversion data
   * @param  AnalogDataRead, Out parameter returns the read data
   * @return HAL_StatusTypeDef returns HAL_ERROR/HAL_TIMEOUT/HAL_OK
   */
  HAL_StatusTypeDef read(uint32_t* AnalogDataRead);

  /**
   * Calls HAL_ADC_Stop
   * @param  None
   * @return HAL_StatusTypeDef returns HAL_ERROR/HAL_TIMEOUT/HAL_OK
   */
  HAL_StatusTypeDef Stop();

 private:
  ADC_HandleTypeDef *AdcInput {nullptr};
  uint32_t Timeout {0};
};

} // namespace HAL
} // namespace Pufferfish
