/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/ShiftedOutput.h"

namespace Pufferfish::Driver {

void ShiftedOutput::write(bool output) {
  dev_.set_channel(channel, output);
}

}  // namespace Pufferfish::Driver
