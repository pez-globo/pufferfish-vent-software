/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *  Software-backed CRC calculation.
 */

#pragma once

#include <climits>

#include "CRCChecker.h"

namespace Pufferfish {
namespace HAL {

template <typename Checksum>
Checksum SoftCRC<Checksum>::compute(const uint8_t *data, size_t size) {
  // Adapted from https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
  static const size_t width = CHAR_BIT * sizeof(Checksum);
  Checksum remainder = init;

  // Divide the message by the polynomial, a byte at a time.
  for (size_t i = 0; i < size; ++i) {
    uint8_t byte = data[i];
    if (ref_in) {
      byte = reflect(byte);
    }
    uint8_t lookup_index = byte ^ (remainder >> (width - 8));
    remainder = crc_table_[lookup_index] ^ (remainder << 8);
  }

  if (ref_out) {
    remainder = reflect(remainder);
  }
  return remainder ^ xor_out;
}

template <typename Checksum>
void SoftCRC<Checksum>::setup() {
  // Adapted from https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
  static const size_t width = CHAR_BIT * sizeof(Checksum);
  static const Checksum top_bit = 1U << (width - 1U);

  // Compute the remainder of each possible dividend
  for (size_t dividend = 0; dividend < table_size; ++dividend) {
    Checksum remainder = dividend << (width - 8);
    for (uint8_t i = CHAR_BIT; i > 0; --i) {
      if (remainder & top_bit) {
        remainder = (remainder << 1) ^ polynomial;
      } else {
        remainder = remainder << 1;
      }
    }
    crc_table_[dividend] = remainder;
  }
}

template <typename T>
T reflect(T num) {
  // Adapted from https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
  T reflection = 0;
  const size_t num_bits = CHAR_BIT * sizeof(T);

  for (size_t i = 0; i < num_bits; ++i) {
    if ((num & 0x01) != 0) {
      reflection |= 1 << (num_bits - 1U - i);
    }
    num = num >> 1;
  }

  return reflection;
}

}  // namespace HAL
}  // namespace Pufferfish
