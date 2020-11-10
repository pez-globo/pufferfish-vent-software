/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * SFM3000.h
 *
 *      Author: March Boonyapaluk
 *
 *  A driver for the FDO2 oxygen sensor
 */

#pragma once

#include "Commands.h"
#include "Pufferfish/HAL/Interfaces/BufferedUART.h"
#include "Pufferfish/Protocols/Chunks.h"

namespace Pufferfish::Driver::Serial::FDO2 {

static const char frame_end = 0x0d;

class ResponseReceiver {
 public:
  enum class InputStatus { ok = 0, output_ready, invalid_frame_length };
  enum class OutputStatus {
    available = 0,
    waiting,
    invalid_frame_length,
    invalid_header,
    invalid_args
  };

  ResponseReceiver() : chunks_(frame_end, true) {}

  InputStatus input(uint8_t new_byte);
  OutputStatus output(Response &output_response);

 private:
  Protocols::ChunkSplitter<Responses::max_len, char> chunks_;
};

class RequestSender {
 public:
  enum class Status { ok = 0, invalid_command, invalid_length };

  Status transform(const Request &input_request, Requests::ChunkBuffer &output_buffer) const;

 private:
  const Protocols::ChunkMerger chunks = Protocols::ChunkMerger(frame_end);
};

/**
 * Stateless low-level driver for FDO2 oxygen sensor device
 */
class Device {
 public:
  enum class Status {
    ok = 0,    /// Operation succeeded
    waiting,   /// Operation in progress
    timed_out  /// Operation timed out
  };

  static const uint16_t broadcast_interval = 100;  // ms

  explicit Device(volatile HAL::BufferedUART &uart) : uart_(uart) {}

  /**
   * Starts broadcast
   * @return ok on success, error code otherwise
   */
  Status start_broadcast();

  /**
   * Receives the next response
   * @param sample[out] the sensor reading; only valid on success
   * @return ok on success, error code otherwise
   */
  Status receive(Response &response);

  /**
   * Flashes LED
   * @return ok on success, error code otherwise
   */
  Status flash_led();

  /**
   * Requests version information
   * @return ok on success, error code otherwise
   */
  Status request_version();

 private:
  volatile HAL::BufferedUART &uart_;
  ResponseReceiver responses_;
  RequestSender requests_;

  BufferStatus write(const Requests::ChunkBuffer &request_buffer);
};

}  // namespace Pufferfish::Driver::Serial::FDO2
