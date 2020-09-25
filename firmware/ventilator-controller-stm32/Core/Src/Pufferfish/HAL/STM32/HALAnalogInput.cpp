/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/STM32/HALAnalogInput.h"

namespace Pufferfish::HAL {

/**
 * @brief  Calls HAL_ADC_Start
 * @param  None
 * @return ADCStatus returns error/ok
 */
ADCStatus HALAnalogInput::start() {
  /* Invoke HAL_ADC_Start to start the ADC */
  HAL_StatusTypeDef start_ret_val = HAL_ADC_Start(&adc_input_);

  /* Validate the HAL_ADC_Start return value to return ADCStatus */
  if (start_ret_val == HAL_OK) {
    /* return ADCStatus as ok */
    return ADCStatus::ok;
  }

  /* return ADCStatus as error */
  return ADCStatus::error;
}

/**
 * @brief  Invokes HAL_ADC_PollForConversion for Adc_Input with Timeout
 *         for the polling conversion of read data.
 * @brief  Invokes HAL_ADC_GetValue to read data of Adc_Input
 * @param  AnalogDataRead, Out parameter returns the read data
 * @retval ADCStatus returns error/ok
 */
ADCStatus HALAnalogInput::read(uint32_t &analog_data_read) {
  HAL_StatusTypeDef tmp_poll_cvr_rtn = HAL_OK;

  /**
   * @brief  Polling conversion of ADC3, based on EOCSelection
   */
  tmp_poll_cvr_rtn = HAL_ADC_PollForConversion(&adc_input_, timeout_);

  /* Validate the HAL_ADC_PollForConversion return value */
  if (tmp_poll_cvr_rtn == HAL_OK) {
    /**
     * @brief  invoking HAL_ADC_GetValue to read analog data
     *         returns analog read data of 4 bytes
     */
    analog_data_read = HAL_ADC_GetValue(&adc_input_);

    /* return ADCStatus as ok */
    return ADCStatus::ok;
  }

  /* return ADCStatus as error */
  return ADCStatus::error;
}

/**
 * @brief  Invokes HAL_ADC_Stop of Adc_Input
 * @param  None
 * @retval ADCStatus returns error/ok
 */
ADCStatus HALAnalogInput::stop() {
  /* Invoke HAL_ADC_Stop to stop the ADC */
  HAL_StatusTypeDef start_ret_val = HAL_ADC_Stop(&adc_input_);

  /* Validate the HAL_ADC_Stop return value to return ADCStatus */
  if (start_ret_val == HAL_OK) {
    /* return ADCStatus as ok */
    return ADCStatus::ok;
  }

  /* return ADCStatus as error */
  return ADCStatus::error;
}

}  // namespace Pufferfish::HAL
