/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Mock/MockDigitalInput.h"

namespace Pufferfish {
namespace HAL {

bool MockDigitalInput::read() {

  /* return lastInput, set during the setRead */
  return lastInput;
}

void MockDigitalInput::setRead(bool setting){
  /* Update the lastInput with input setting value */
  lastInput = setting;
}

} // namespace HAL
} // namespace Pufferfish
