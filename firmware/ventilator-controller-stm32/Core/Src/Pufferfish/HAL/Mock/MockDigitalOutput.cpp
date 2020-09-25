/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Mock/MockDigitalOutput.h"

namespace Pufferfish::HAL {

void MockDigitalOutput::write(bool output) {
  /* Update the lastOutput with parameter output value */
  last_output_ = output;
}

bool MockDigitalOutput::get_write() const {
  /* return the write value, written during the write method */
  return last_output_;
}
}  // namespace Pufferfish::HAL
