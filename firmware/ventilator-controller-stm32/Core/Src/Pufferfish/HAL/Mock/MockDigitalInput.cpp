/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Mock/MockDigitalInput.h"

namespace Pufferfish::HAL {

bool MockDigitalInput::read() {
  /* return lastInput, set during the setRead */
  return last_input_;
}

void MockDigitalInput::set_read(bool setting) {
  /* Update the lastInput with input setting value */
  last_input_ = setting;
}

}  // namespace Pufferfish::HAL
