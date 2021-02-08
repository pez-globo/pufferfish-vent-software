/*
 * Messages.tpp
 *
 *  Created on: May 22, 2020
 *      Author: Ethan Li
 */

#pragma once

#include "Messages.h"
#include "nanopb/pb_decode.h"
#include "nanopb/pb_encode.h"

namespace Pufferfish::Protocols {

// Message

template <typename TaggedUnion, size_t max_size>
template <size_t output_size, size_t num_descriptors>
MessageStatus Message<TaggedUnion, max_size>::write(
    Util::ByteVector<output_size> &output_buffer,
    const Util::ProtobufDescriptors<num_descriptors> &pb_protobuf_descriptors) const {
  auto type = static_cast<uint8_t>(payload.tag); // declared local variable to keep write method const
  if (type > pb_protobuf_descriptors.size()) {
    return MessageStatus::invalid_type;
  }

  const pb_msgdesc_t *fields = pb_protobuf_descriptors[type];
  if (fields == Util::get_protobuf_descriptor<Util::UnrecognizedMessage>()) {
    return MessageStatus::invalid_type;
  }

  size_t encoded_size = 0;
  if (!pb_get_encoded_size(&encoded_size, fields, &(payload.value))) {
    return MessageStatus::invalid_encoding;
  }

  if (output_buffer.resize(header_size + encoded_size) != IndexStatus::ok) {
    return MessageStatus::invalid_length;
  }

  output_buffer[type_offset] = type;
  pb_ostream_t stream = pb_ostream_from_buffer(
      output_buffer.buffer() + header_size, output_buffer.size() - header_size);
  if (!pb_encode(&stream, fields, &(payload.value))) {
    return MessageStatus::invalid_encoding;
  }

  return MessageStatus::ok;
}

template <typename TaggedUnion, size_t max_size>
template <size_t input_size, size_t num_descriptors>
MessageStatus Message<TaggedUnion, max_size>::parse(
    const Util::ByteVector<input_size> &input_buffer,
    const Util::ProtobufDescriptors<num_descriptors> &pb_protobuf_descriptors) {
  if (input_buffer.size() < Message::header_size) {
    return MessageStatus::invalid_length;
  }

  type = input_buffer[Message::type_offset];
  if (type > pb_protobuf_descriptors.size()) {
    return MessageStatus::invalid_type;
  }

  // TODO(lietk12): add proper checking
  payload.tag = static_cast<typename TaggedUnion::Tag>(type);
  const pb_msgdesc_t *fields = pb_protobuf_descriptors[type];
  if (fields == Util::get_protobuf_descriptor<Util::UnrecognizedMessage>()) {
    return MessageStatus::invalid_type;
  }

  pb_istream_t stream = pb_istream_from_buffer(
      input_buffer.buffer() + header_size, input_buffer.size() - header_size);
  if (!pb_decode(&stream, fields, &(payload.value))) {
    return MessageStatus::invalid_encoding;
  }

  return MessageStatus::ok;
}

// MessageReceiver

template <typename Message, size_t num_descriptors>
MessageReceiver<Message, num_descriptors>::MessageReceiver(
    const Util::ProtobufDescriptors<num_descriptors> &descriptors)
    : descriptors_(descriptors) {}

template <typename Message, size_t num_descriptors>
template <size_t input_size>
MessageStatus MessageReceiver<Message, num_descriptors>::transform(
    const Util::ByteVector<input_size> &input_buffer, Message &output_message) const {
  return output_message.parse(input_buffer, descriptors_);
}

// MessageSender

template <typename Message, size_t num_descriptors>
MessageSender<Message, num_descriptors>::MessageSender(
    const Util::ProtobufDescriptors<num_descriptors> &descriptors)
    : descriptors_(descriptors) {}

template <typename Message, size_t num_descriptors>
template <size_t output_size>
MessageStatus MessageSender<Message, num_descriptors>::transform(
    const Message &input_message, Util::ByteVector<output_size> &output_buffer) const {
  return input_message.write(output_buffer, descriptors_);
}

}  // namespace Pufferfish::Protocols
