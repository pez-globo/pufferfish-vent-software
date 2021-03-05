/*
 * Frames.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "Pufferfish/Protocols/Chunks.h"
#include "Pufferfish/Util/Vector.h"

namespace Pufferfish::Driver::Serial::Backend {

struct FrameProps {
  static const size_t payload_max_size = 254;
  static const size_t chunk_max_size = payload_max_size + 2;    // including delimiter
  static const size_t encoded_max_size = payload_max_size + 1;  // including cobs
  using ChunkBuffer = Util::ByteVector<chunk_max_size>;
  using EncodedBuffer = Util::ByteVector<encoded_max_size>;
  using PayloadBuffer = Util::ByteVector<payload_max_size>;

  using InputStatus = Protocols::ChunkInputStatus;
  using OutputStatus = Protocols::ChunkOutputStatus;
};

using FrameChunkSplitter = Protocols::ChunkSplitter<FrameProps::encoded_max_size>;

// Decodes frames (length up to 255 bytes, excluding frame delimiter) with COBS
class COBSDecoder {
 public:
  COBSDecoder() = default;

  template <size_t input_size, size_t output_size>
  FrameProps::OutputStatus transform(
      const Util::ByteVector<input_size> &input_buffer,
      Util::ByteVector<output_size> &output_buffer) const;
};

// Encodes payloads (length up to 254 bytes) with COBS; does not add the frame delimiter
class COBSEncoder {
 public:
  COBSEncoder() = default;

  template <size_t input_size, size_t output_size>
  FrameProps::OutputStatus transform(
      const Util::ByteVector<input_size> &input_buffer,
      Util::ByteVector<output_size> &output_buffer) const;
};

class FrameReceiver {
 public:
  FrameReceiver() = default;

  // Call this until it returns available, then call output
  FrameProps::InputStatus input(uint8_t new_byte);
  FrameProps::OutputStatus output(FrameProps::PayloadBuffer &output_buffer);

 private:
  FrameChunkSplitter chunk_splitter_;
  const COBSDecoder cobs_decoder = COBSDecoder();
};

class FrameSender {
 public:
  FrameSender() = default;

  FrameProps::OutputStatus transform(
      const FrameProps::PayloadBuffer &input_buffer, FrameProps::ChunkBuffer &output_buffer) const;

 private:
  const COBSEncoder cobs_encoder = COBSEncoder();
  const Protocols::ChunkMerger chunk_merger = Protocols::ChunkMerger();
};

}  // namespace Pufferfish::Driver::Serial::Backend

#include "Frames.tpp"
