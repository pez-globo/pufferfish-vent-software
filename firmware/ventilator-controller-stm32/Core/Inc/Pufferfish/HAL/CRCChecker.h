/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *  Software-backed CRC calculation.
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "Pufferfish/HAL/Interfaces/CRCChecker.h"

namespace Pufferfish {
namespace HAL {

template <typename Checksum>
struct CRCParameters {
  const Checksum polynomial;
  const Checksum init;
  const bool ref_in;
  const bool ref_out;
  const Checksum xor_out;
};
using CRC8Parameters = CRCParameters<uint8_t>;
using CRC32Parameters = CRCParameters<uint32_t>;

static constexpr CRC32Parameters crc32c_params = {0x1edc6f41, 0xffffffff, true, true, 0xffffffff};

/**
 * Computes a cyclic redundancy check code with a lookup table
 *
 * @param polynomial    the CRC generator polynomial
 * @param init      initial value of CRC code
 * @param refIn     true if the input should be reflected
 * @param refOut    true if the output should be reflected
 * @param xorOut    value to be xor-ed with the final output
 * @return an 8-bits CRC of the given data
 */
template <typename Checksum>
class SoftCRC : public CRCChecker<Checksum> {
 public:
  SoftCRC(Checksum polynomial, Checksum init, bool ref_in, bool ref_out, Checksum xor_out);
  explicit SoftCRC(const CRCParameters<Checksum> &parameters);

  Checksum compute(const uint8_t *data, size_t size) override;

 private:
  static const size_t table_size = 256;

  const Checksum polynomial;
  const Checksum init;
  const bool ref_in{};
  const bool ref_out{};
  const Checksum xor_out;

  // Reflected lookup table generated at http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
  // using CRC32_C preset
  std::array<uint32_t, table_size> crc_table_;

  void setup();
};
using SoftCRC8 = SoftCRC<uint8_t>;
using SoftCRC32 = SoftCRC<uint32_t>;

/**
 * Reverses all the bits in the input
 * @param num   an integer
 * @return num with all the bits reversed/inverted
 */
template <typename T>
T reflect(T num);

}  // namespace HAL
}  // namespace Pufferfish

#include "CRCChecker.tpp"
