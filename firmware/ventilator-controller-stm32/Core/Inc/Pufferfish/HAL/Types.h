/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Ethan Li
 *
 * Platform-dependent type definitions
 */

#pragma once

#include <cstdint>

namespace Pufferfish {
namespace HAL {

// This is the largest type which can be atomically updated on 32-bit
// processors. For 16-bit processors, it should be changed to uint16_t. For
// 8-bit processors, it should be changed to uint8_t.
using AtomicSize = uint32_t;

}  // namespace HAL
}  // namespace Pufferfish
