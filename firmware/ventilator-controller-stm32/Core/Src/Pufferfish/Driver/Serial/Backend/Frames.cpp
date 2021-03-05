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
  return chunk_splitter_.input(new_byte);
}

FrameProps::OutputStatus FrameReceiver::output(FrameProps::PayloadBuffer &output_buffer) {
  FrameProps::EncodedBuffer temp_buffer;

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
    const FrameProps::PayloadBuffer &input_buffer, FrameProps::ChunkBuffer &output_buffer) const {
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
