/*
 * CRCElements.tpp
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <type_traits>

#include "CRCElements.h"
#include "Pufferfish/Util/Endian.h"

namespace Pufferfish::Protocols {

// CRCElement

template <typename PayloadBuffer>
template <size_t output_size>
IndexStatus CRCElement<PayloadBuffer>::write(
    Util::ByteVector<output_size> &output_buffer, HAL::CRC32 &crc32c) {
  static_assert(
      Util::ByteVector<output_size>::max_size() >=
          (PayloadBuffer::max_size() + CRCElementHeaderProps::header_size),
      "Write method unavailable as the size of the output buffer is too small");

  if (write_protected(output_buffer) != IndexStatus::ok) {
    return IndexStatus::out_of_bounds;
  }

  crc_ = compute_body_crc(output_buffer, crc32c);
  Util::write_hton(crc_, output_buffer.buffer());
  return IndexStatus::ok;
}

template <typename PayloadBuffer>
template <size_t output_size>
IndexStatus CRCElement<PayloadBuffer>::write_protected(
    Util::ByteVector<output_size> &output_buffer) const {
  if (output_buffer.resize(CRCElementHeaderProps::header_size + payload_.size()) !=
      IndexStatus::ok) {
    return IndexStatus::out_of_bounds;
  }

  if (output_buffer.copy_from(
          payload_.buffer(), payload_.size(), CRCElementHeaderProps::payload_offset) !=
      IndexStatus::ok) {
    return IndexStatus::out_of_bounds;
  };
  return IndexStatus::ok;
}

template <typename PayloadBuffer>
template <size_t input_size>
IndexStatus CRCElement<PayloadBuffer>::parse(const Util::ByteVector<input_size> &input_buffer) {
  static_assert(
      !std::is_const<PayloadBuffer>::value,
      "Parse method unavailable for CRCElements with const PayloadBuffer type");
  static_assert(
      Util::ByteVector<input_size>::max_size() <=
          (PayloadBuffer::max_size() + CRCElementHeaderProps::header_size),
      "Parse method unavailable as the input buffer size is too large");

  if (input_buffer.size() < CRCElementHeaderProps::header_size) {
    return IndexStatus::out_of_bounds;
  }
  Util::read_ntoh(input_buffer.buffer(), crc_);
  if (payload_.copy_from(
          input_buffer.buffer() + CRCElementHeaderProps::payload_offset,
          input_buffer.size() - CRCElementHeaderProps::payload_offset) != IndexStatus::ok) {
    return IndexStatus::out_of_bounds;
  };
  return IndexStatus::ok;
}

template <typename PayloadBuffer>
template <size_t buffer_size>
uint32_t CRCElement<PayloadBuffer>::compute_body_crc(
    const Util::ByteVector<buffer_size> &buffer, HAL::CRC32 &crc32c) {
  return crc32c.compute(
      buffer.buffer() + CRCElementHeaderProps::payload_offset,  // exclude the CRC field
      buffer.size() - sizeof(uint32_t)                          // exclude the size of the CRC field
  );
}

// CRCElementReceiver

template <size_t body_max_size>
template <size_t input_size>
typename CRCElementReceiver<body_max_size>::Status CRCElementReceiver<body_max_size>::transform(
    const Util::ByteVector<input_size> &input_buffer,
    ParsedCRCElement<body_max_size> &output_crcelement) {
  if (output_crcelement.parse(input_buffer) != IndexStatus::ok) {
    return Status::invalid_parse;
  }

  if (ParsedCRCElement<body_max_size>::compute_body_crc(input_buffer, crc32c_) !=
      output_crcelement.crc()) {
    return Status::invalid_crc;
  }

  return Status::ok;
}

// CRCElementSender

template <size_t body_max_size>
template <size_t output_size>
typename CRCElementSender<body_max_size>::Status CRCElementSender<body_max_size>::transform(
    const typename Props::PayloadBuffer &input_payload,
    Util::ByteVector<output_size> &output_buffer) {
  ConstructedCRCElement<body_max_size> crcelement(input_payload);
  if (crcelement.write(output_buffer, crc32c_) != IndexStatus::ok) {
    return Status::invalid_length;
  }

  return Status::ok;
}

}  // namespace Pufferfish::Protocols
