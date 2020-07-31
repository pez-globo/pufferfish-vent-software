/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/Indicators/ShiftedOutput.h"

namespace Pufferfish {
namespace Driver {
namespace Indicators {

void ShiftedOutput::write(bool output) {
  mDev.setChannel(mChannel, output);
}

}  // namespace Indicators
}  // namespace HAL
}  // namespace Pufferfish

