/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/AnalogInput.h"

namespace Pufferfish {
namespace HAL {

/**
 * Abstract class for HAL Analog input in STM32
 */
class MockAnalogInput : public AnalogInput {
 public:
  /**
   * @brief Constructs a new Analog Input
   * @param hadc and tTimeout
   */
  MockAnalogInput(){

  }

  /**
   * @brief  Updates the mLastState to true
   * @param  None
   * @return ADCStatus returns ok
   */
  ADCStatus start() override;

  /**
   * @brief  Returns analog input data stored in lastInput
   * @param  AnalogDataRead, Out parameter returns the lastInput
   * @return ADCStatus returns ok
   */
  ADCStatus read(uint32_t &analogDataRead) override;

  /**
   * @brief  Updates the mLastState to false
   * @param  None
   * @return ADCStatus returns ok
   */
  ADCStatus stop() override;

  /**
   * @brief  Provides the state of Analog Input
   * @param  None
   * @return return state START(true)/STOP(false)
   */
  bool getAnalogInputState();

  /**
   * @brief  Updates the lastInput with setAnalogDataRead
   * @param  setAnalogDataRead value to update the lastInput
   * @return None
   */
  void setRead(uint32_t setAnalogDataRead);
 private:
  /* State for Analog input START */
  bool mLastState = false;
  /* Last Input data set */
  uint32_t lastInput;

};

} // namespace HAL
} // namespace Pufferfish
