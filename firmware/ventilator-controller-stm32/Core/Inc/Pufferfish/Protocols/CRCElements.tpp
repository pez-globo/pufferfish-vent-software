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

  output_buffer.copy_from(
      payload_.buffer(), payload_.size(), CRCElementHeaderProps::payload_offset);
  return IndexStatus::ok;
}

template <typename PayloadBuffer>
template <size_t input_size>
IndexStatus CRCElement<PayloadBuffer>::parse(const Util::ByteVector<input_size> &input_buffer) {
  static_assert(
      !std::is_const<PayloadBuffer>::value,
      "Parse method unavailable for CRCElements with const PayloadBuffer type");

  if (input_buffer.size() < CRCElementHeaderProps::header_size) {
    return IndexStatus::out_of_bounds;
  }
  Util::read_ntoh(input_buffer.buffer(), crc_);
  payload_.copy_from(
      input_buffer.buffer() + CRCElementHeaderProps::payload_offset,
      input_buffer.size() - CRCElementHeaderProps::payload_offset);
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
    ParsedCRCElement<body_max_size> &output_datagram) {
  if (output_datagram.parse(input_buffer) != IndexStatus::ok) {
    return Status::invalid_parse;
  }

  if (ParsedCRCElement<body_max_size>::compute_body_crc(input_buffer, crc32c_) !=
      output_datagram.crc()) {
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
  ConstructedCRCElement<body_max_size> datagram(input_payload);
  if (datagram.write(output_buffer, crc32c_) != IndexStatus::ok) {
    return Status::invalid_length;
  }

  return Status::ok;
}

}  // namespace Pufferfish::Protocols
