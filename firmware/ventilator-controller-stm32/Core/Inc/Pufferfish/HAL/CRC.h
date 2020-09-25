/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *  Hardware-backed CRC calculation.
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace Pufferfish {
namespace HAL {

/**
 * Computes an 8-bits cyclic redundancy check code with a software-based
 * algorithm
 *
 * @param data      a pointer to the data to be computed into a CRC code
 * @param size      size of the data
 * @param polynomial    the CRC generator polynomial
 * @param init      initial value of CRC code
 * @param refIn     true if the input should be reflected
 * @param refOut    true if the output should be reflected
 * @param xorOut    value to be xor-ed with the final output
 * @return an 8-bits CRC of the given data
 */
uint8_t compute_crc8(
    const uint8_t *data,
    size_t size,
    uint8_t polynomial,
    uint8_t init = 0x00,
    bool ref_in = false,
    bool ref_out = false,
    uint8_t xor_out = 0x00);

/**
 * Reverses all the bits in the input
 * @param num   an 8-bit integer
 * @return num with all the bits reversed/inverted
 */
uint8_t reflect8(uint8_t num);

}  // namespace HAL
}  // namespace Pufferfish
