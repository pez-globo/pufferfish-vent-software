/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Chunks.cpp
 *
 *  Created on: Nov 3, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of Message class
 *
 */

#include "Pufferfish/Protocols/Messages.h"
#include "Pufferfish/Application/States.h"
#include "Pufferfish/Protocols/Chunks.h"
#include "Pufferfish/Util/Vector.h"
#include "Pufferfish/Util/Array.h"
#include "nanopb/pb.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

static const auto message_descriptors =
PF::Util::make_array<PF::Util::ProtobufDescriptor>(
    // array index should match the type code value
    PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>(),  // 0
    PF::Util::get_protobuf_descriptor<Alarms>(),                         // 1
    PF::Util::get_protobuf_descriptor<SensorMeasurements>(),             // 2
    PF::Util::get_protobuf_descriptor<CycleMeasurements>(),              // 3
    PF::Util::get_protobuf_descriptor<Parameters>(),                     // 4
    PF::Util::get_protobuf_descriptor<ParametersRequest>(),              // 5
    PF::Util::get_protobuf_descriptor<Ping>(),                           // 6
    PF::Util::get_protobuf_descriptor<Announcement>()                    // 7
);
static constexpr size_t num_descriptors = 8;

SCENARIO("Protocols::Message behaves correctly", "[messages]") {
  GIVEN("A protocol-buffer message") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<
      PF::Application::StateSegment,
      payload_max_size
    >;
    TestMessage test_message;

    WHEN("the regular alarm data is written to the buffer") {
      constexpr size_t buffer_size = 252UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarms;
      PF::Util::ByteVector<buffer_size> buffer;
      auto status = test_message.write(buffer, message_descriptors);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("the unknown data is written to the buffer") {
      constexpr size_t buffer_size = 252UL;
      test_message.payload.tag = PF::Application::MessageTypes::unknown;
      PF::Util::ByteVector<buffer_size> buffer;
      auto status = test_message.write(buffer, message_descriptors);

      THEN("the final status should be invalid_type") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("the regular alarm data is written to a buffer of zero size") {
      constexpr size_t buffer_size = 0UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarms;
      PF::Util::ByteVector<buffer_size> buffer;
      auto status = test_message.write(buffer, message_descriptors);

      THEN("the final status should be invalid_length") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("the regular alarm data is parsed from the buffer") {
      constexpr size_t buffer_size = 253UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarms;
      PF::Util::ByteVector<buffer_size> buffer;
      auto write_status = test_message.write(buffer, message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::alarms);
      auto parse_status = test_message.parse(buffer, message_descriptors);

      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("data is parsed from an invalid buffer") {
      constexpr size_t buffer_size = 253UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarms;
      PF::Util::ByteVector<buffer_size> buffer;
      buffer.resize(buffer_size);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::alarms);
      auto status = test_message.parse(buffer, message_descriptors);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_encoding);
      }
    }

    WHEN("the regular alarm data is parsed from the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageSender<TestMessage, num_descriptors> sender{message_descriptors};
      test_message.payload.tag = PF::Application::MessageTypes::alarms;
      PF::Util::ByteVector<buffer_size> buffer;
      auto transform_status = sender.transform(test_message, buffer);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("the regular alarm data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{message_descriptors};
      test_message.payload.tag = PF::Application::MessageTypes::alarms;

      PF::Util::ByteVector<buffer_size> buffer;
      auto write_status = test_message.write(buffer, message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::alarms);

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

  }
}
