/*
 * Messages.h
 *
 *  Created on: May 22, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <cstdint>

#include "Pufferfish/Util/Protobuf.h"
#include "Pufferfish/Util/Vector.h"
#include "nanopb/pb_common.h"

namespace Pufferfish::Protocols {

enum class MessageStatus { ok = 0, invalid_length, invalid_type, invalid_encoding };

// Messages

template <typename TaggedUnion, typename MessageTypes, size_t max_size>
class Message {
 public:
  static const size_t type_offset = 0;
  // This clearly is static initialization of a static const - probably clang-tidy
  // false positive?
  // NOLINTNEXTLINE(bugprone-dynamic-static-initializers)
  static const size_t payload_offset = type_offset + sizeof(uint8_t);

  // This clearly is static initialization of a static const - probably clang-tidy
  // false positive?
  // NOLINTNEXTLINE(bugprone-dynamic-static-initializers)
  static const size_t header_size = payload_offset;
  // NOLINTNEXTLINE(bugprone-dynamic-static-initializers)
  static const size_t payload_max_size = max_size - header_size;

  uint8_t type = 0;
  TaggedUnion payload{};

  template <size_t output_size, size_t num_descriptors>
  MessageStatus write(
      Util::ByteVector<output_size> &output_buffer,
      const Util::ProtobufDescriptors<num_descriptors> &pb_protobuf_descriptors) const;

  template <size_t input_size, size_t num_descriptors>
  MessageStatus parse(
      const Util::ByteVector<input_size> &input_buffer,
      const Util::ProtobufDescriptors<num_descriptors>
          &pb_protobuf_descriptors);  // updates type and payload fields
};

// Parses messages into payloads, with data integrity checking
template <typename Message, size_t num_descriptors>
class MessageReceiver {
 public:
  explicit MessageReceiver(const Util::ProtobufDescriptors<num_descriptors> &descriptors);

  template <size_t input_size>
  MessageStatus transform(
      const Util::ByteVector<input_size> &input_buffer, Message &output_message) const;

 private:
  const Util::ProtobufDescriptors<num_descriptors> &descriptors_;
};

// Generates messages from payloads
template <typename Message, size_t num_descriptors>
class MessageSender {
 public:
  explicit MessageSender(const Util::ProtobufDescriptors<num_descriptors> &descriptors);

  template <size_t output_size>
  MessageStatus transform(
      const Message &input_message, Util::ByteVector<output_size> &output_buffer) const;

 private:
  const Util::ProtobufDescriptors<num_descriptors> &descriptors_;
};

}  // namespace Pufferfish::Protocols

#include "Messages.tpp"
