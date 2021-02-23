/*
 * Frames.cpp
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#include "Pufferfish/Driver/Serial/Backend/Frames.h"

namespace Pufferfish::Driver::Serial::Backend {

// FrameReceiver

FrameProps::InputStatus FrameReceiver::input(uint8_t new_byte) {
  bool input_overwritten = false;
  auto status = chunk_splitter_.input(new_byte, input_overwritten);
  if (input_overwritten) {
    return FrameProps::InputStatus::input_overwritten;
  }

  switch (status) {
    case Protocols::ChunkInputStatus::output_ready:
      return FrameProps::InputStatus::output_ready;
    case Protocols::ChunkInputStatus::invalid_length:
      return FrameProps::InputStatus::invalid_length;
    case Protocols::ChunkInputStatus::ok:
      break;
  }

  return FrameProps::InputStatus::ok;
}

FrameProps::OutputStatus FrameReceiver::output(FrameProps::ChunkBuffer &output_buffer) {
  FrameProps::ChunkBuffer temp_buffer;

  // Chunk
  FrameProps::OutputStatus status = chunk_splitter_.output(temp_buffer);
  if (status != FrameProps::OutputStatus::ok) {
    return status;
  }

  // COBS
  status = cobs_decoder.transform(temp_buffer, output_buffer);
  if (status != FrameProps::OutputStatus::ok) {
    return status;
  }

  return FrameProps::OutputStatus::ok;
}

// FrameSender

FrameProps::OutputStatus FrameSender::transform(
    const FrameProps::ChunkBuffer &input_buffer, FrameProps::ChunkBuffer &output_buffer) const {
  // COBS
  FrameProps::OutputStatus status = cobs_encoder.transform(input_buffer, output_buffer);
  if (status != FrameProps::OutputStatus::ok) {
    return status;
  }

  // Chunk
  status = chunk_merger.transform(output_buffer);
  if (status != FrameProps::OutputStatus::ok) {
    return status;
  }

  return FrameProps::OutputStatus::ok;
}

}  // namespace Pufferfish::Driver::Serial::Backend
