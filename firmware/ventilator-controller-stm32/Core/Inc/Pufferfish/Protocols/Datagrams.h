/*
 * Datagrams.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "Pufferfish/Util/Vector.h"

namespace Pufferfish::Protocols {

struct DatagramHeaderProps {
  static const size_t seq_offset = 0;
  static const size_t length_offset = seq_offset + sizeof(uint8_t);
  static const size_t payload_offset = length_offset + sizeof(uint8_t);

  static const size_t header_size = payload_offset;
};

template <size_t body_max_size>
struct DatagramProps {
  // This clearly is static initialization of a static const - probably clang-tidy
  // false positive?
  // NOLINTNEXTLINE(bugprone-dynamic-static-initializers)
  static const size_t payload_max_size = body_max_size - DatagramHeaderProps::header_size;
  using PayloadBuffer = Util::ByteVector<payload_max_size>;
};

template <typename PayloadBuffer>
class Datagram {
 public:
  explicit Datagram(PayloadBuffer &payload, uint8_t seq)
      : seq_(seq), length_(static_cast<uint8_t>(payload.size())), payload_(payload) {}

  explicit Datagram(PayloadBuffer &payload) : Datagram(payload, 0) {}

  [[nodiscard]] uint8_t seq() const { return seq_; }
  [[nodiscard]] uint8_t length() const { return length_; }
  [[nodiscard]] const PayloadBuffer &payload() const { return payload_; }

  template <size_t output_size>
  IndexStatus write(Util::ByteVector<output_size> &output_buffer);  // updates length crc field

  template <size_t input_size>
  IndexStatus parse(
      const Util::ByteVector<input_size> &input_buffer);  // updates all fields, including payload

 private:
  uint8_t seq_ = 0;
  uint8_t length_ = 0;
  PayloadBuffer &payload_;
};

// In this Datagram, the payload can be modified through the parse method, so
// a const payload cannot be given in the constructor.
template <size_t body_max_size>
using ParsedDatagram = Datagram<typename DatagramProps<body_max_size>::PayloadBuffer>;

// In this Datagram, the payload can only be set through the constructor, so
// a const payload can be given in the constructor. However, the parse method
// is not available, as it would modify the payload given in the constructor.
template <size_t body_max_size>
using ConstructedDatagram = Datagram<const typename DatagramProps<body_max_size>::PayloadBuffer>;

// Parses datagrams into payloads, with data integrity checking
template <size_t body_max_size>
class DatagramReceiver {
 public:
  using Props = DatagramProps<body_max_size>;
  enum class Status { ok = 0, invalid_parse, invalid_length, invalid_sequence };

  DatagramReceiver() = default;

  template <size_t input_size>
  Status transform(
      const Util::ByteVector<input_size> &input_buffer,
      ParsedDatagram<body_max_size> &output_datagram);

 private:
  uint8_t expected_seq_ = 0;
};

// Generates datagrams from payloads
template <size_t body_max_size>
class DatagramSender {
 public:
  using Props = DatagramProps<body_max_size>;
  enum class Status { ok = 0, invalid_length };

  DatagramSender() = default;

  template <size_t output_size>
  Status transform(
      const typename Props::PayloadBuffer &input_payload,
      Util::ByteVector<output_size> &output_buffer);

 private:
  uint8_t next_seq_ = 0;
};

}  // namespace Pufferfish::Protocols

#include "Datagrams.tpp"
