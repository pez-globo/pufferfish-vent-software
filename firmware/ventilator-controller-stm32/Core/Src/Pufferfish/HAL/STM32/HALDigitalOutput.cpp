/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/HALDigitalOutput.h"

namespace Pufferfish::HAL {

void HALDigitalOutput::write(bool output) {
  if (inverted) {
    HAL_GPIO_WritePin(&port_, pin, output ? GPIO_PIN_RESET : GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(&port_, pin, output ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

}  // namespace Pufferfish::HAL
