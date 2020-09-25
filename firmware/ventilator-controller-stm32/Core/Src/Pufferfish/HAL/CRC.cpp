/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Ethan Li
 */

#include "Pufferfish/HAL/CRC.h"

#include <climits>

namespace Pufferfish::HAL {

uint8_t compute_crc8(
    const uint8_t *data,
    size_t size,
    const uint8_t polynomial,
    uint8_t init,
    bool ref_in,
    bool ref_out,
    uint8_t xor_out) {
  uint8_t crc = init;
  static const uint8_t crc_mask = 1U << (CHAR_BIT - 1U);

  // calculates 8-Bit checksum with given polynomial
  for (size_t byte_ctr = 0; byte_ctr < size; ++byte_ctr) {
    crc ^= ref_in ? reflect8(data[byte_ctr]) : (data[byte_ctr]);
    for (uint8_t bit = CHAR_BIT; bit > 0; --bit) {
      if ((crc & crc_mask) > 0) {
        crc = crc << 1U;
        crc ^= polynomial;
      } else {
        crc = crc << 1U;
      }
    }
  }

  if (ref_out) {
    crc = reflect8(crc);
  }

  return crc ^ xor_out;
}

uint8_t reflect8(uint8_t num) {
  uint8_t res_val = 0;

  for (uint8_t i = 0; i < CHAR_BIT; i++) {
    if ((num & (1U << i)) != 0) {
      res_val |= static_cast<uint8_t>(1U << (CHAR_BIT - 1U - i));
    }
  }

  return res_val;
}

}  // namespace Pufferfish::HAL
