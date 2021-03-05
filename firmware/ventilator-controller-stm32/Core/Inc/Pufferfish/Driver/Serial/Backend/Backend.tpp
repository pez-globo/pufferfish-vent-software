/*
 * Serial.tpp
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#pragma once

#include "Backend.h"

namespace Pufferfish::Driver::Serial::Backend {

// BackendReceiver

BackendReceiver::InputStatus BackendReceiver::input(uint8_t new_byte) {
  switch (frame_.input(new_byte)) {
    case FrameProps::InputStatus::output_ready:
      return InputStatus::output_ready;
    case FrameProps::InputStatus::invalid_length:
      return InputStatus::invalid_frame_length;
    case FrameProps::InputStatus::ok:
      break;
  }
  return InputStatus::ok;
}

BackendReceiver::OutputStatus BackendReceiver::output(BackendMessage &output_message) {
  FrameProps::PayloadBuffer temp_buffer1;
  BackendCRCReceiver::Props::PayloadBuffer temp_buffer2;
  BackendDatagramReceiver::Props::PayloadBuffer temp_buffer3;

  // Frame
  switch (frame_.output(temp_buffer1)) {
    case FrameProps::OutputStatus::waiting:
      return OutputStatus::waiting;
    case FrameProps::OutputStatus::invalid_length:
      return OutputStatus::invalid_frame_length;
    case FrameProps::OutputStatus::ok:
      break;
  }

  // CRCElement
  BackendParsedCRC receive_crc(temp_buffer2);
  switch (crc_.transform(temp_buffer1, receive_crc)) {
    case BackendCRCReceiver::Status::invalid_parse:
      return OutputStatus::invalid_crcelement_parse;
    case BackendCRCReceiver::Status::invalid_crc:
      return OutputStatus::invalid_crcelement_crc;
    case BackendCRCReceiver::Status::ok:
      break;
  }

  // Datagram
  BackendParsedDatagram receive_datagram(temp_buffer3);
  switch (datagram_.transform(temp_buffer2, receive_datagram)) {
    case BackendDatagramReceiver::Status::invalid_parse:
      return OutputStatus::invalid_datagram_parse;
    case BackendDatagramReceiver::Status::invalid_length:
      return OutputStatus::invalid_datagram_length;
    case BackendDatagramReceiver::Status::invalid_sequence:
      // TODO(lietk12): emit a warning about invalid sequence
    case BackendDatagramReceiver::Status::ok:
      break;
  }

  // Message
  switch (message_.transform(temp_buffer3, output_message)) {
    case Protocols::MessageStatus::invalid_length:
      return OutputStatus::invalid_message_length;
    case Protocols::MessageStatus::invalid_type:
      return OutputStatus::invalid_message_type;
    case Protocols::MessageStatus::invalid_encoding:
      return OutputStatus::invalid_message_encoding;
    case Protocols::MessageStatus::ok:
      break;
  }
  return OutputStatus::available;
}

// BackendSender

BackendSender::Status BackendSender::transform(
    const BackendMessage &input_message, FrameProps::ChunkBuffer &output_buffer) {
  BackendDatagramSender::Props::PayloadBuffer temp_buffer1;
  BackendCRCSender::Props::PayloadBuffer temp_buffer2;
  FrameProps::PayloadBuffer temp_buffer3;

  // Message
  switch (message_.transform(input_message, temp_buffer1)) {
    case Protocols::MessageStatus::invalid_length:
      return Status::invalid_message_length;
    case Protocols::MessageStatus::invalid_type:
      return Status::invalid_message_type;
    case Protocols::MessageStatus::invalid_encoding:
      return Status::invalid_message_encoding;
    case Protocols::MessageStatus::ok:
      break;
  }

  // Datagram
  switch (datagram_.transform(temp_buffer1, temp_buffer2)) {
    case BackendDatagramSender::Status::invalid_length:
      return Status::invalid_datagram_length;
    case BackendDatagramSender::Status::ok:
      break;
  }

  // CRCElement
  switch (crc_.transform(temp_buffer2, temp_buffer3)) {
    case BackendCRCSender::Status::invalid_length:
      return Status::invalid_crcelement_length;
    case BackendCRCSender::Status::ok:
      break;
  }

  // Frame
  switch (frame_.transform(temp_buffer3, output_buffer)) {
    case FrameProps::OutputStatus::invalid_length:
      return Status::invalid_frame_length;
    case FrameProps::OutputStatus::ok:
      break;
    default:
      return Status::invalid_return_code;
  }
  return Status::ok;
}

// Backend

Backend::Status Backend::input(uint8_t new_byte) {
  // Input into receiver
  switch (receiver_.input(new_byte)) {
    case BackendReceiver::InputStatus::output_ready:
      break;
    case BackendReceiver::InputStatus::invalid_frame_length:
      // TODO(lietk12): handle error case first
    case BackendReceiver::InputStatus::ok:
      return Status::waiting;
  }

  // Output from receiver
  BackendMessage message;
  switch (receiver_.output(message)) {
    case BackendReceiver::OutputStatus::invalid_datagram_sequence:
      // TODO(lietk12): handle warning case first
    case BackendReceiver::OutputStatus::available:
      break;
    case BackendReceiver::OutputStatus::invalid_frame_length:
    case BackendReceiver::OutputStatus::invalid_crcelement_parse:
    case BackendReceiver::OutputStatus::invalid_crcelement_crc:
    case BackendReceiver::OutputStatus::invalid_datagram_parse:
    case BackendReceiver::OutputStatus::invalid_datagram_length:
    case BackendReceiver::OutputStatus::invalid_message_length:
    case BackendReceiver::OutputStatus::invalid_message_type:
    case BackendReceiver::OutputStatus::invalid_message_encoding:
      // TODO(lietk12): handle error cases first
      return Status::invalid;
    case BackendReceiver::OutputStatus::waiting:
      return Status::waiting;
  }

  // Input into state synchronization
  switch (synchronizer_.input(message.payload)) {
    case BackendStateSynchronizer::InputStatus::ok:
      break;
    case BackendStateSynchronizer::InputStatus::invalid_type:
      // TODO(lietk12): handle error case
      return Status::invalid;
  }

  return Status::ok;
}

void Backend::update_clock(uint32_t current_time) {
  synchronizer_.input(current_time);
}

Backend::Status Backend::output(FrameProps::ChunkBuffer &output_buffer) {
  // Output from state synchronization
  BackendMessage message;
  switch (synchronizer_.output(message.payload)) {
    case BackendStateSynchronizer::OutputStatus::available:
      break;
    case BackendStateSynchronizer::OutputStatus::waiting:
      return Status::waiting;
  }

  switch (sender_.transform(message, output_buffer)) {
    case BackendSender::Status::ok:
      break;
    case BackendSender::Status::invalid_message_length:
    case BackendSender::Status::invalid_message_type:
    case BackendSender::Status::invalid_message_encoding:
    case BackendSender::Status::invalid_datagram_length:
    case BackendSender::Status::invalid_crcelement_length:
    case BackendSender::Status::invalid_frame_length:
    case BackendSender::Status::invalid_return_code:
      // TODO(lietk12): handle error cases first
      return Status::invalid;
  }

  return Status::ok;
}

}  // namespace Pufferfish::Driver::Serial::Backend
