/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include <Pufferfish/HAL/STM32/HALAnalogInput.h>

namespace Pufferfish {
namespace HAL {

/**
 * @brief  Calls HAL_ADC_Start
 * @param  None
 * @return ADCStatus returns error/ok
 */
ADCStatus HALAnalogInput::start() {

  /* Invoke HAL_ADC_Start to start the ADC */
  HAL_StatusTypeDef startRetVal = HAL_ADC_Start(&adcInput);

  /* Validate the HAL_ADC_Start return value to return ADCStatus */
  if (startRetVal == HAL_OK)
  {
    /* return ADCStatus as ok */
    return ADCStatus::ok;
  }
  else
  {
    /* return ADCStatus as error */
    return ADCStatus::error;
  }
}

/**
  * @brief  Invokes HAL_ADC_PollForConversion for Adc_Input with Timeout
  *         for the polling conversion of read data.
  * @brief  Invokes HAL_ADC_GetValue to read data of Adc_Input
  * @param  AnalogDataRead, Out parameter returns the read data
  * @retval ADCStatus returns error/ok
  */
ADCStatus HALAnalogInput::read(uint32_t &analogDataRead) {

  HAL_StatusTypeDef tmpPollCvrRtn = HAL_OK;

  /**
    * @brief  Polling conversion of ADC3, based on EOCSelection
    */
  tmpPollCvrRtn = HAL_ADC_PollForConversion(&adcInput, timeout);

  /* Validate the HAL_ADC_PollForConversion return value */
  if (tmpPollCvrRtn == HAL_OK)
  {
    /**
      * @brief  invoking HAL_ADC_GetValue to read analog data
      *         returns analog read data of 4 bytes
      */
    analogDataRead = HAL_ADC_GetValue(&adcInput);

    /* return ADCStatus as ok */
    return ADCStatus::ok;
  }
  else
  {
    /* return ADCStatus as error */
    return ADCStatus::error;
  }
}


/**
  * @brief  Invokes HAL_ADC_Stop of Adc_Input
  * @param  None
  * @retval ADCStatus returns error/ok
  */
ADCStatus HALAnalogInput::stop() {

  /* Invoke HAL_ADC_Stop to stop the ADC */
  HAL_StatusTypeDef startRetVal = HAL_ADC_Stop(&adcInput);

  /* Validate the HAL_ADC_Stop return value to return ADCStatus */
  if (startRetVal == HAL_OK)
  {
    /* return ADCStatus as ok */
    return ADCStatus::ok;
  }
  else
  {
    /* return ADCStatus as error */
    return ADCStatus::error;
  }
}

} // namespace HAL
} // namespace Pufferfish
