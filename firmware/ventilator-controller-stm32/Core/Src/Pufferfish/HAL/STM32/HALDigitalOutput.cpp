/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/HALDigitalOutput.h"

namespace Pufferfish {
namespace HAL {

void HALDigitalOutput::write(bool output) {
  HAL_GPIO_WritePin(&mPort, mPin, output ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

} // namespace HAL
} // namespace Pufferfish
