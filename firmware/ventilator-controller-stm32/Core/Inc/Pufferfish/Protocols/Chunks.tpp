/*
 * Frames.tpp
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#pragma once

#include "Chunks.h"

namespace Pufferfish::Protocols {

// ChunkSplitter

template <size_t buffer_size, typename Byte>
ChunkInputStatus ChunkSplitter<buffer_size, Byte>::input(
    uint8_t new_byte, bool &input_overwritten) {
  if (input_status_ == ChunkInputStatus::output_ready) {
    buffer_.clear();
    input_overwritten = true;
    input_status_ = ChunkInputStatus::ok;
  }

  if (include_delimiter || new_byte != delimiter) {
    if (buffer_.push_back(new_byte) != IndexStatus::ok) {
      input_status_ = ChunkInputStatus::invalid_length;
      return input_status_;
    }
  }

  if (new_byte == delimiter) {
    input_status_ = ChunkInputStatus::output_ready;
    return input_status_;
  }

  return ChunkInputStatus::ok;
}

template <size_t buffer_size, typename Byte>
ChunkOutputStatus ChunkSplitter<buffer_size, Byte>::output(
    Util::Vector<Byte, buffer_size> &output_buffer) {
  if (input_status_ == ChunkInputStatus::ok) {
    return ChunkOutputStatus::waiting;
  }

  output_buffer.copy_from(buffer_);
  buffer_.clear();
  ChunkOutputStatus output_status = ChunkOutputStatus::ok;
  if (input_status_ == ChunkInputStatus::invalid_length) {
    output_status = ChunkOutputStatus::invalid_length;
  }
  input_status_ = ChunkInputStatus::ok;
  return output_status;
}

// ChunkMerger

template <size_t buffer_size, typename Byte>
ChunkOutputStatus ChunkMerger::transform(
    Util::Vector<Byte, buffer_size> &input_output_buffer) const {
  if (input_output_buffer.push_back(delimiter) == IndexStatus::ok) {
    return ChunkOutputStatus::ok;
  }
  return ChunkOutputStatus::invalid_length;
}

}  // namespace Pufferfish::Protocols
