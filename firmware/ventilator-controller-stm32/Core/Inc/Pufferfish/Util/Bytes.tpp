/*
 * Bytes.tpp
 *
 *  Created on: Aug 31, 2020
 *      Author: Ethan Li
 */

#pragma once

#include "Bytes.h"

namespace Pufferfish::Util {

template <typename T>
T set_byte(uint8_t byte, size_t byte_index) {
  return byte << static_cast<size_t>(byte_index * CHAR_BIT);
}

}  // namespace Pufferfish::Util
