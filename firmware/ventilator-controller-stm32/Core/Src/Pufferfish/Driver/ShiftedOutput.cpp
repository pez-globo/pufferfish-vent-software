/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/ShiftedOutput.h"

namespace Pufferfish {
namespace Driver {

void ShiftedOutput::write(bool output) {
  mDev.setChannel(mChannel, output);
}

}  // namespace HAL
}  // namespace Pufferfish

