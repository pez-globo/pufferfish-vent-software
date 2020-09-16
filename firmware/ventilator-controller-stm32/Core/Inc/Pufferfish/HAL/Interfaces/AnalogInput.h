/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include <stdint.h>
#include "Pufferfish/Statuses.h"

namespace Pufferfish {
namespace HAL {

/**
 * Represents a Analog input in STM32
 */
class AnalogInput{
 public:
  /**
   * @brief Calls HAL_ADC_Start
   * @param  None
   * @return ADCStatus returns error/ok/busy
   */
  virtual ADCStatus start() = 0;

  /**
   * @brief Reads a Analog conversion data
   * @param  AnalogDataRead, Out parameter returns the read data
   * @return ADCStatus returns error/ok/timeout
   */
  virtual ADCStatus read(uint32_t &AnalogDataRead) = 0;

  /**
   * Calls HAL_ADC_Stop
   * @param  None
   * @return ADCStatus returns error/ok
   */
  virtual ADCStatus stop() = 0;

};

} // namespace HAL
} // namespace Pufferfish
