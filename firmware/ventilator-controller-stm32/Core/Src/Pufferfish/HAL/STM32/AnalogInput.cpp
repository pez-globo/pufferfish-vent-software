/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/STM32/AnalogInput.h"

namespace Pufferfish {
namespace HAL {

/**
  * @brief Invokes HAL_ADC_Start of Adc_Input
  * @param None
  * @retval HAL_StatusTypeDef returns HAL_BUSY/HAL_ERROR/HAL_OK
  */
HAL_StatusTypeDef AnalogInput::Start() {
  /*
   * return HAL_ADC_Start
   */
  return HAL_ADC_Start(AdcInput);

}

/**
  * @brief  Invokes HAL_ADC_PollForConversion for Adc_Input with Timeout
  *         for the polling conversion of read data.
  * @brief  Invokes HAL_ADC_GetValue to read data of Adc_Input
  * @param  AnalogDataRead, Out parameter returns the read data
  * @retval HAL_StatusTypeDef returns HAL_TIMEOUT/HAL_ERROR/HAL_OK
  */
HAL_StatusTypeDef AnalogInput::read(uint32_t* AnalogDataRead) {

  /* */
  HAL_StatusTypeDef tmp_pollCvrRtn {HAL_OK};

  /**
    * @brief  Polling conversion of ADC3, based on EOCSelection
    * @param  Adc_Input and Timeout
    * @return HAL_StatusTypeDef returns HAL_ERROR/HAL_TIMEOUT/HAL_OK
    */
  tmp_pollCvrRtn = HAL_ADC_PollForConversion(AdcInput, Timeout);

  /* Check for the return value of HAL_ADC_PollForConversion */
  if (tmp_pollCvrRtn == HAL_OK)
  {
    /**
      * @brief  invoking HAL_ADC_GetValue to read analog data
      * @param  Adc_Input
      * @retval AnalogDataRead, returns 4 bytes of analog read data
      */
    *AnalogDataRead = HAL_ADC_GetValue(AdcInput);
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
  * @retval HAL_StatusTypeDef returns HAL_BUSY/HAL_ERROR/HAL_OK
  */
HAL_StatusTypeDef AnalogInput::Stop() {
  /*
   * return HAL_ADC_Stop
   */
  return HAL_ADC_Stop(AdcInput);
}

} // namespace HAL
} // namespace Pufferfish

