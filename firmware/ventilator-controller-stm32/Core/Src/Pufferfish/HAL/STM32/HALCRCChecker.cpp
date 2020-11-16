/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/HALCRCChecker.h"

namespace Pufferfish::HAL {

uint32_t HALCRC32::compute(const uint8_t *data, size_t size) {
  return ~HAL_CRC_Calculate(
      &hcrc_,
      // We need to perform a const cast because the STM32 HAL function is not const-correct,
      // even though the function implementation does not modify the input data.
      // We need to perform a reinterpret cast because the STM32 HAL function asks for it.
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast, cppcoreguidelines-pro-type-reinterpret-cast)
      const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(data)),
      size);
}

}  // namespace Pufferfish::HAL
