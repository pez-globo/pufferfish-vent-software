/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "stm32h7xx_hal.h"

#include "Pufferfish/Statuses.h"

namespace Pufferfish {
namespace HAL {

/**
 * Represents a Analog input in STM32
 */
class AnalogInput{

 public:
  /**
   * @brief Constructs a new Analog Input
   * @param hadc and tTimeout
   */
  AnalogInput(ADC_HandleTypeDef &hadc, uint32_t tTimeout) : AdcInput(hadc),  Timeout(tTimeout){}

  /**
   * @brief Calls HAL_ADC_Start
   * @param  None
   * @return ADCStatus returns error/ok/busy
   */
  ADCStatus start();

  /**
   * @brief Reads a Analog conversion data
   * @param  AnalogDataRead, Out parameter returns the read data
   * @return ADCStatus returns error/ok/timeout
   */
  ADCStatus read(uint32_t &AnalogDataRead);

  /**
   * Calls HAL_ADC_Stop
   * @param  None
   * @return ADCStatus returns error/ok
   */
  ADCStatus stop();

 private:
  ADC_HandleTypeDef &AdcInput;
  uint32_t Timeout = 0;

};

} // namespace HAL
} // namespace Pufferfish
