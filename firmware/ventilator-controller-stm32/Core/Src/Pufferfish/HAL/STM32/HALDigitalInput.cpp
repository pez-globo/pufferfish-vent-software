/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/HALDigitalInput.h"

namespace Pufferfish {
namespace HAL {

bool HALDigitalInput::read() {
  return
      mInverted ?
      HAL_GPIO_ReadPin(&mPort, mPin) == GPIO_PIN_RESET :
      HAL_GPIO_ReadPin(&mPort, mPin) == GPIO_PIN_SET;
}

} // namespace HAL
} // namespace Pufferfish
