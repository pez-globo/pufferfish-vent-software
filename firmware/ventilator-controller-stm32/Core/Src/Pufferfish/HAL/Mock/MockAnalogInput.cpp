/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include <Pufferfish/HAL/Mock/MockAnalogInput.h>

namespace Pufferfish {
namespace HAL {

ADCStatus MockAnalogInput::start() {
  mLastState = true;
  /* return ADCStatus as ok */
  return ADCStatus::ok;
}

ADCStatus MockAnalogInput::read(uint32_t &analogDataRead) {
  analogDataRead = lastInput;
  /* return ADCStatus as ok */
  return ADCStatus::ok;
}

void MockAnalogInput::setRead(uint32_t setAnalogDataRead) {

  /* Update the lastInput with input analog data read value */
  lastInput = setAnalogDataRead;
}


ADCStatus MockAnalogInput::stop() {
  mLastState = false;
  /* return ADCStatus as ok */
  return ADCStatus::ok;
}

bool MockAnalogInput::getAnalogInputState() {
  return mLastState;
;
}
} // namespace HAL
} // namespace Pufferfish

