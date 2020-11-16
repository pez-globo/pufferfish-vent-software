/*
 * CRC.h
 *
 *  Created on: May 19, 2020
 *      Author: Ethan Li
 *
 *  CRC calculation.
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace Pufferfish::HAL {

template <typename Checksum>
class CRCChecker {
 public:
  /**
   * Computes a cyclic redundancy check code
   *
   * @param data      a pointer to the data to be computed into a CRC code
   * @param size      size of the data
   */
  virtual Checksum compute(const uint8_t *data, size_t size) = 0;
};

using CRC8 = CRCChecker<uint8_t>;
using CRC32 = CRCChecker<uint32_t>;

}  // namespace Pufferfish::HAL
