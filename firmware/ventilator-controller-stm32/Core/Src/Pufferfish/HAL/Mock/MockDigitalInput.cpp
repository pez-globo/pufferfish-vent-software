/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Mock/MockDigitalInput.h"

namespace Pufferfish {
namespace HAL {

bool MockDigitalInput::read() {

  return lastInput;

  /*
  return HAL_GPIO_ReadPin(&mPort, mPin) == GPIO_PIN_SET;
  */
}

void MockDigitalInput::setRead(bool setting){
  lastInput = setting;
}

} // namespace HAL
} // namespace Pufferfish
