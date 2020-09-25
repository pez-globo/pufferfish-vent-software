/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/AnalogInput.h"
#include "stm32h7xx_hal.h"

namespace Pufferfish {
namespace HAL {

/**
 * Abstract class for HAL Analog input in STM32
 */
class HALAnalogInput : public AnalogInput {
 public:
  /**
   * @brief Constructs a new Analog Input
   * @param hadc and tTimeout
   */
  HALAnalogInput(ADC_HandleTypeDef &hadc, uint32_t t_timeout)
      : adc_input_(hadc), timeout_(t_timeout) {}

  /**
   * @brief Calls HAL_ADC_Start
   * @param  None
   * @return ADCStatus returns error/ok/busy
   */
  ADCStatus start() override;

  /**
   * @brief Reads a Analog conversion data
   * @param  AnalogDataRead, Out parameter returns the read data
   * @return ADCStatus returns error/ok/timeout
   */
  ADCStatus read(uint32_t &analog_data_read) override;

  /**
   * Calls HAL_ADC_Stop
   * @param  None
   * @return ADCStatus returns error/ok
   */
  ADCStatus stop() override;

 private:
  ADC_HandleTypeDef &adc_input_;
  uint32_t timeout_ = 0;
};

}  // namespace HAL
}  // namespace Pufferfish
