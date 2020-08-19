/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/STM32/AnalogInput.h"

namespace Pufferfish {
namespace HAL {

/**
 * @brief Calls HAL_ADC_Start
 * @param  None
 * @return ADCStatus returns error/ok/busy
 */
ADCStatus AnalogInput::start() {
  /*
   * return HAL_ADC_Start
   */
  return static_cast<ADCStatus>(HAL_ADC_Start(&AdcInput));

}

/**
  * @brief  Invokes HAL_ADC_PollForConversion for Adc_Input with Timeout
  *         for the polling conversion of read data.
  * @brief  Invokes HAL_ADC_GetValue to read data of Adc_Input
  * @param  AnalogDataRead, Out parameter returns the read data
  * @retval ADCStatus returns error/ok/timeout
  */
ADCStatus AnalogInput::read(uint32_t &AnalogDataRead) {

  /* */
  ADCStatus tmp_pollCvrRtn = ADCStatus::ok;

  /**
    * @brief  Polling conversion of ADC3, based on EOCSelection
    */
  tmp_pollCvrRtn = static_cast<ADCStatus>(HAL_ADC_PollForConversion(&AdcInput, Timeout));

  /* Check for the return value of HAL_ADC_PollForConversion */
  if (tmp_pollCvrRtn == ADCStatus::ok)
  {
    /**
      * @brief  invoking HAL_ADC_GetValue to read analog data
      *         returns analog read data of 4 bytes
      */
    AnalogDataRead = HAL_ADC_GetValue(&AdcInput);
  }
  else
  {
    /* Else Statements */
  }

  return tmp_pollCvrRtn;
}


/**
  * @brief Invokes HAL_ADC_Stop of Adc_Input
  * @param None
  * @retval ADCStatus returns error/ok
  */
ADCStatus AnalogInput::stop() {
  /*
   * return HAL_ADC_Stop
   */
  return static_cast<ADCStatus>(HAL_ADC_Stop(&AdcInput));
}

} // namespace HAL
} // namespace Pufferfish

