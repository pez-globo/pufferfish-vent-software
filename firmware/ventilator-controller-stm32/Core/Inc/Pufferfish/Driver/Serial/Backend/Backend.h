/*
 * Serial.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <cstdint>

#include "Frames.h"
#include "Pufferfish/Application/States.h"
#include "Pufferfish/HAL/Interfaces/CRCChecker.h"
#include "Pufferfish/Protocols/CRCElements.h"
#include "Pufferfish/Protocols/Datagrams.h"
#include "Pufferfish/Protocols/Messages.h"
#include "Pufferfish/Protocols/States.h"
#include "Pufferfish/Util/Array.h"

namespace Pufferfish::Driver::Serial::Backend {

// States

static const auto message_descriptors = Util::make_array<Util::ProtobufDescriptor>(
    // array index should match the type code value
    Util::get_protobuf_descriptor<Util::UnrecognizedMessage>(),  // 0
    Util::get_protobuf_descriptor<Alarms>(),                     // 1
    Util::get_protobuf_descriptor<SensorMeasurements>(),         // 2
    Util::get_protobuf_descriptor<CycleMeasurements>(),          // 3
    Util::get_protobuf_descriptor<Parameters>(),                 // 4
    Util::get_protobuf_descriptor<ParametersRequest>(),          // 5
    Util::get_protobuf_descriptor<Ping>(),                       // 6
    Util::get_protobuf_descriptor<Announcement>()                // 7
);

// State Synchronization

using StateOutputScheduleEntry = Protocols::StateOutputScheduleEntry<Application::MessageTypes>;

static const auto state_sync_schedule = Util::make_array<const StateOutputScheduleEntry>(
    StateOutputScheduleEntry{10, Application::MessageTypes::sensor_measurements},
    StateOutputScheduleEntry{10, Application::MessageTypes::parameters},
    StateOutputScheduleEntry{10, Application::MessageTypes::alarms},
    StateOutputScheduleEntry{10, Application::MessageTypes::sensor_measurements},
    StateOutputScheduleEntry{10, Application::MessageTypes::ping},
    StateOutputScheduleEntry{10, Application::MessageTypes::alarms},
    StateOutputScheduleEntry{10, Application::MessageTypes::sensor_measurements},
    StateOutputScheduleEntry{10, Application::MessageTypes::parameters_request},
    StateOutputScheduleEntry{10, Application::MessageTypes::cycle_measurements});

// Backend
using BackendMessage = Protocols::Message<
    Application::StateSegment,
    Protocols::DatagramProps<
        Driver::Serial::Backend::FrameProps::payload_max_size>::payload_max_size>;

class BackendReceiver {
 public:
  enum class InputStatus { ok = 0, output_ready, invalid_frame_length };
  enum class OutputStatus {
    available = 0,
    waiting,
    invalid_frame_length,
    invalid_crcelement_parse,
    invalid_crcelement_crc,
    invalid_datagram_parse,
    invalid_datagram_length,
    invalid_datagram_sequence,
    invalid_message_length,
    invalid_message_type,
    invalid_message_encoding
  };

  explicit BackendReceiver(HAL::CRC32 &crc32c) : crc_(crc32c), message_(message_descriptors) {}

  // Call this until it returns outputReady, then call output
  InputStatus input(uint8_t new_byte);
  OutputStatus output(BackendMessage &output_message);

 private:
  using BackendCRCReceiver = Protocols::CRCElementReceiver<FrameProps::payload_max_size>;
  using BackendParsedCRC = Protocols::ParsedCRCElement<FrameProps::payload_max_size>;
  using BackendDatagramReceiver =
      Protocols::DatagramReceiver<BackendCRCReceiver::Props::payload_max_size>;
  using BackendParsedDatagram =
      Protocols::ParsedDatagram<BackendCRCReceiver::Props::payload_max_size>;
  using BackendMessageReceiver =
      Protocols::MessageReceiver<BackendMessage, message_descriptors.size()>;

  FrameReceiver frame_;
  BackendCRCReceiver crc_;
  BackendDatagramReceiver datagram_;
  BackendMessageReceiver message_;
};

class BackendSender {
 public:
  enum class Status {
    ok = 0,
    invalid_message_length,
    invalid_message_type,
    invalid_message_encoding,
    invalid_datagram_length,
    invalid_crcelement_length,
    invalid_frame_length,
    invalid_return_code
  };

  explicit BackendSender(HAL::CRC32 &crc32c) : message_(message_descriptors), crc_(crc32c) {}

  Status transform(const BackendMessage &input_message, FrameProps::ChunkBuffer &output_buffer);

 private:
  using BackendCRCSender = Protocols::CRCElementSender<FrameProps::payload_max_size>;
  using BackendDatagramSender =
      Protocols::DatagramSender<BackendCRCSender::Props::payload_max_size>;
  using BackendMessageSender = Protocols::MessageSender<BackendMessage, message_descriptors.size()>;

  BackendMessageSender message_;
  BackendDatagramSender datagram_;
  BackendCRCSender crc_;
  FrameSender frame_;
};

class Backend {
 public:
  enum class Status { ok = 0, waiting, invalid };

  Backend(HAL::CRC32 &crc32c, Application::States &states)
      : receiver_(crc32c), sender_(crc32c), synchronizer_(states, state_sync_schedule) {}

  Status input(uint8_t new_byte);
  void update_clock(uint32_t current_time);
  Status output(FrameProps::ChunkBuffer &output_buffer);

 private:
  using BackendStateSynchronizer = Protocols::StateSynchronizer<
      Application::States,
      Application::StateSegment,
      Application::MessageTypes,
      state_sync_schedule.size()>;

  BackendReceiver receiver_;
  BackendSender sender_;
  BackendStateSynchronizer synchronizer_;
};

}  // namespace Pufferfish::Driver::Serial::Backend

#include "Backend.tpp"
