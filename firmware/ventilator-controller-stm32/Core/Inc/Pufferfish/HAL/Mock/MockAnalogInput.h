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
  MockAnalogInput() = default;

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
  ADCStatus read(uint32_t &analog_data_read) override;

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
  [[nodiscard]] bool get_analog_input_state() const;

  /**
   * @brief  Updates the lastInput with setAnalogDataRead
   * @param  setAnalogDataRead value to update the lastInput
   * @return None
   */
  void set_read(uint32_t set_analog_data_read);

 private:
  /* State for Analog input START */
  bool last_state_ = false;
  /* Last Input data set */
  uint32_t last_input_ = 0;
};

}  // namespace HAL
}  // namespace Pufferfish
