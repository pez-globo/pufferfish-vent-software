/*
 * CRCElements.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "Pufferfish/HAL/Interfaces/CRCChecker.h"
#include "Pufferfish/Util/Vector.h"

namespace Pufferfish::Protocols {

struct CRCElementHeaderProps {
  static const size_t payload_offset = sizeof(uint32_t);

  static const size_t header_size = payload_offset;
};

template <size_t body_max_size>
struct CRCElementProps {
  // This clearly is static initialization of a static const - probably clang-tidy
  // false positive?
  // NOLINTNEXTLINE(bugprone-dynamic-static-initializers)
  static const size_t payload_max_size = body_max_size - CRCElementHeaderProps::header_size;
  using PayloadBuffer = Util::ByteVector<payload_max_size>;
};

template <typename PayloadBuffer>
class CRCElement {
 public:
  explicit CRCElement(PayloadBuffer &payload) : payload_(payload) {}

  [[nodiscard]] uint32_t crc() const { return crc_; }
  [[nodiscard]] const PayloadBuffer &payload() const { return payload_; }

  template <size_t output_size>
  IndexStatus write(
      Util::ByteVector<output_size> &output_buffer,
      HAL::CRC32 &crc32c);  // updates length and crc fields

  template <size_t input_size>
  IndexStatus parse(
      const Util::ByteVector<input_size> &input_buffer);  // updates all fields, including payload

  template <size_t buffer_size>
  static uint32_t compute_body_crc(const Util::ByteVector<buffer_size> &buffer, HAL::CRC32 &crc32c);

 private:
  uint32_t crc_ = 0;
  PayloadBuffer &payload_;

  template <size_t output_size>
  IndexStatus write_protected(Util::ByteVector<output_size> &output_buffer) const;
};

// In this CRCElement, the payload can be modified through the parse method, so
// a const payload cannot be given in the constructor.
template <size_t body_max_size>
using ParsedCRCElement = CRCElement<typename CRCElementProps<body_max_size>::PayloadBuffer>;

// In this CRCElement, the payload can only be set through the constructor, so
// a const payload can be given in the constructor. However, the parse method
// is not available, as it would modify the payload given in the constructor.
template <size_t body_max_size>
using ConstructedCRCElement =
    CRCElement<const typename CRCElementProps<body_max_size>::PayloadBuffer>;

// Parses datagrams into payloads, with data integrity checking
template <size_t body_max_size>
class CRCElementReceiver {
 public:
  using Props = CRCElementProps<body_max_size>;
  enum class Status { ok = 0, invalid_parse, invalid_crc };

  explicit CRCElementReceiver(HAL::CRC32 &crc32c) : crc32c_(crc32c) {}

  template <size_t input_size>
  Status transform(
      const Util::ByteVector<input_size> &input_buffer,
      ParsedCRCElement<body_max_size> &output_crcelement);

 private:
  HAL::CRC32 &crc32c_;
};

// Generates datagrams from payloads
template <size_t body_max_size>
class CRCElementSender {
 public:
  using Props = CRCElementProps<body_max_size>;
  enum class Status { ok = 0, invalid_length };

  explicit CRCElementSender(HAL::CRC32 &crc32c) : crc32c_(crc32c) {}

  template <size_t output_size>
  Status transform(
      const typename Props::PayloadBuffer &input_payload,
      Util::ByteVector<output_size> &output_buffer);

 private:
  HAL::CRC32 &crc32c_;
};

}  // namespace Pufferfish::Protocols

#include "CRCElements.tpp"
