/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Mock/MockDigitalOutput.h"

namespace Pufferfish {
namespace HAL {

void MockDigitalOutput::write(bool output) {

  /* Update the lastOutput with parameter output value */
  lastOutput = output;
}

bool MockDigitalOutput::getWrite() {

  /* return the write value, written during the write method */
  return lastOutput;

}
} // namespace HAL
} // namespace Pufferfish
