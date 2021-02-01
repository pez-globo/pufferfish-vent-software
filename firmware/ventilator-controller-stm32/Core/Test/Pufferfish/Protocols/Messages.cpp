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
#include "Pufferfish/Application/mcu_pb.h"
#include "Pufferfish/Protocols/Chunks.h"
#include "Pufferfish/Test/BackendDefs.h"
#include "Pufferfish/Test/Util.h"
#include "Pufferfish/Util/Array.h"
#include "Pufferfish/Util/Vector.h"
#include "catch2/catch.hpp"
#include "nanopb/pb.h"

#include <iostream>
namespace PF = Pufferfish;
namespace BE = PF::Driver::Serial::Backend;

static constexpr size_t num_descriptors = 8;

SCENARIO("Protocols::The Messages class correctly writes to the output buffer", "[messages]") {
  GIVEN("A Message object constructed with StateSegment Taggedunion and a payload of size 252 bytes") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<PF::Application::StateSegment, payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    WHEN("The index of payload tag is greater than the message descriptor array size") {
      constexpr auto message_descriptors =
      PF::Util::make_array<PF::Util::ProtobufDescriptor>(
          // array index should match the type code value
          PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>()
      );

      test_message.payload.value.parameters_request.flow = 60;
      test_message.payload.tag = PF::Application::MessageTypes::parameters_request;

      auto write_status = test_message.write(buffer, message_descriptors);

      THEN("The write status is equal to invalid type") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The Message type in the payload is Unrecognized") {
      test_message.payload.tag = PF::Application::MessageTypes::unknown;

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to invalid type") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The output buffer cannot hold the payload data") {
      constexpr size_t size = 10UL;
      PF::Util::ByteVector<size> input_buffer;
      test_message.payload.tag = PF::Application::MessageTypes::sensor_measurements;
      test_message.payload.value.sensor_measurements.fio2 = 40;
      test_message.payload.value.sensor_measurements.spo2 = 92;

      auto write_status = test_message.write(input_buffer, BE::message_descriptors);

      THEN("The write status is equal to invalid length") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("The alarm limits data is written to a buffer of zero size") {
      constexpr size_t buffer_size = 0UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarm_limits;
      PF::Util::ByteVector<buffer_size> buffer;
      auto status = test_message.write(buffer, BE::message_descriptors);

      THEN("The final status should be invalid_length") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("The sensor measurments message type data is written") {
      test_message.payload.tag = PF::Application::MessageTypes::sensor_measurements;
      test_message.payload.value.sensor_measurements.flow = 50;

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }

      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(buffer[0] == 0x02);
      }
    }

    WHEN("The cycle measurments message type data is written") {
      test_message.payload.tag = PF::Application::MessageTypes::cycle_measurements;
      test_message.payload.value.cycle_measurements.rr = 50;

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }

      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(buffer[0] == 0x03);
      }
    }

    WHEN("The parameters message type data is written") {
      test_message.payload.tag = PF::Application::MessageTypes::parameters;
      test_message.payload.value.parameters.fio2 = 60;

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }

      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(buffer[0] == 0x04);
      }
    }

    WHEN("The parameters request message type data is written") {
      test_message.payload.tag = PF::Application::MessageTypes::parameters_request;
      test_message.payload.value.parameters_request.mode = VentilationMode_hfnc;

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }

      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(buffer[0] == 0x05);
      }

      THEN("The last byte is eqaul to 6") {
        REQUIRE(buffer[2] == 0x06);
      }
    }
  }
}

SCENARIO("Protocols::The Messages class correctly parses the input buffer and updates type and payload fields", "[messages]") {
  GIVEN("A Message object constructed with StateSegment Taggedunion and a payload of size 252 bytes") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<PF::Application::StateSegment, payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    WHEN("The value at zero index of the input buffer is equal to 0") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto data = PF::Util::make_array<uint8_t>(0x00);
      for (auto& input : data) {
        input_buffer.push_back(input);
      }

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The parse status is equal to invalid type") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::invalid_type);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::unknown);
      }
    }

    WHEN("input buffer size is less than 1 byte") {
      PF::Util::ByteVector<buffer_size> input_buffer;

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The parse status is equal to invalid length") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    // WHEN("The first byte of input buffer is greater than descriptor array size") {
    //   PF::Util::ByteVector<buffer_size> input_buffer;
    //   auto data = PF::Util::make_array<uint8_t>(0x08); // SIGSEGV - Segmentation violation signal
    //   for (auto& input : data) {
    //     input_buffer.push_back(input);
    //   }

    //   auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

    //   THEN("The parse status is equal to invalid length") {
    //     REQUIRE(parse_status == PF::Protocols::MessageStatus::invalid_type);
    //   }
    // }

    WHEN("The data is parsed from an invalid buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Util::ByteVector<buffer_size> buffer;
      auto data = PF::Util::make_array<uint8_t>(0x06);
      for (auto& input : data) {
        buffer.push_back(input);
      }
      buffer.resize(buffer_size);
      auto status = test_message.parse(buffer, BE::message_descriptors);

      THEN("The final status should be ok") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_encoding);
      }
    }

    WHEN("The parameters request data parsed") {
      auto input_data = PF::Util::make_array<uint8_t>(0x05, 0x10, 0x06, 0x45, 0x00, 0x00, 0x20, 0x42, 0x4d, 0x00, 0x00, 0x70, 0x42);
      PF::Util::ByteVector<buffer_size> input_buffer;
      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }
      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("type is equal to 5 or sensor measurments") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(test_message.payload.value.parameters_request.flow == 60);
        REQUIRE(test_message.payload.value.parameters_request.mode == VentilationMode_hfnc);
        REQUIRE(test_message.payload.value.parameters_request.ventilating == false);
        REQUIRE(test_message.payload.value.parameters_request.fio2 == 40);
      }
    }

    WHEN("The parameters request message type is written and then parsed from the buffer") {
      constexpr size_t buffer_size = 253UL;

      ParametersRequest parameters_request;
      parameters_request.ventilating = true;
      parameters_request.fio2 = 40;
      parameters_request.mode = VentilationMode_hfnc;
      parameters_request.flow = 60;
      test_message.payload.set(parameters_request);
      test_message.payload.tag = PF::Application::MessageTypes::parameters_request;

      PF::Util::ByteVector<buffer_size> buffer;
      auto write_status = test_message.write(buffer, BE::message_descriptors);
      THEN("The final status should be ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(buffer[0] == 0x05);
      }
      auto parse_status = test_message.parse(buffer, BE::message_descriptors);
      THEN("The final status should be ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(test_message.payload.value.parameters_request.fio2 == 40);
        REQUIRE(test_message.payload.value.parameters_request.flow == 60);
      }
    }
  }
}

SCENARIO("Protocols::The Message Receiver class correctly transforms messages into paylaods", "[messages]") {
  GIVEN("A MessageReceiver object is constructed with default parameters") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<PF::Application::StateSegment, payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

    WHEN("The parameters data is parsed from the buffer") {
      auto input_data = PF::Util::make_array<uint8_t>(0x04, 0x45, 0x00, 0x00, 0xA0, 0x42);
      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters);
        REQUIRE(test_message.payload.value.parameters.fio2 == 80);
      }
    }

    WHEN("The parameters request data is parsed from the buffer") {
      constexpr size_t buffer_size = 254UL;
      auto input_data = PF::Util::make_array<uint8_t>(0x05, 0x10, 0x06, 0x45, 0x00, 0x00, 0x20, 0x42, 0x4d, 0x00, 0x00, 0x70, 0x42);
      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto status = receiver.transform(input_buffer, test_message);

      REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters_request);
      REQUIRE(status == PF::Protocols::MessageStatus::ok);
      REQUIRE(test_message.payload.value.parameters_request.mode == VentilationMode_hfnc);
    }

    WHEN("The alarm limits request data is parsed from the buffer") {
      auto input_data = PF::Util::make_array<uint8_t>(0x07, 0x12, 0x00);
      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits_request);
        REQUIRE(test_message.payload.value.alarm_limits_request.has_fio2 == true);
      }
    }

    WHEN("The alarm limits data is parsed from the buffer") {
      auto input_data = PF::Util::make_array<uint8_t>(0x06, 0x6A, 0x00);
      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto transform_status = receiver.transform(input_buffer, test_message);


      THEN("The final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits);
        REQUIRE(test_message.payload.value.alarm_limits.has_flow == true);
      }
    }

    WHEN("The alarm limits is parsed from the buffer") {
      auto body = std::string("\x06\x08\xA8\xA6\xCD\xA9\x03\x12\x04\x08\x32\x10\x46\x1A\x02\x10\x2E\x2A\x04\x08\xFD\xFF\x01\x3A\x0A\x08\xFD\xFF\x01\x10\xD0\xF6\xAE\xA9\x03\x42\x03\x08\xAE\x03\x4A\x04\x08\xFD\xFF\x01\x5A\x04\x08\xBA\xAC\x01\x72\x0C\x08\xB4\xBD\xE5\xE7\x06\x10\xE8\xB7\xFE\xB2\x01");

      PF::Util::ByteVector<buffer_size> input_buffer;

      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.lower == 50);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.upper == 70);
      }
    }

    WHEN("The cycle measurements data is parsed from the buffer") {
      auto input_data = PF::Util::make_array<uint8_t>(0x03, 0x3d, 0x00, 0x96, 0x43, 0x00);
      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::cycle_measurements);
      }
    }

    WHEN("bad data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{
          BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("The final status should be invalid_length") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("bad type is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{
          BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;
      auto input_data = PF::Util::make_array<uint8_t>(0x00);

      for (auto& data : input_data) {
        buffer.push_back(data);
      }

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("The final status should be invalid_length") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("invalid buffer is provided") {
      PF::Util::ByteVector<buffer_size> input_buffer;

      auto input_data = PF::Util::make_array<uint8_t>(0x06, 0x6A, 0x03);

      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("invalid encoding") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_encoding);
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits);
      }
    }
  }
}

SCENARIO("Protocols::The Message Sender class correctly transforms payloads into messages", "[messages]") {
  GIVEN("A MessageSender object is constructed with default parameters") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<PF::Application::StateSegment, payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    PF::Protocols::MessageSender<TestMessage, num_descriptors> sender{BE::message_descriptors};

    WHEN("The parameters data from the message is written to the buffer") {
      Parameters parameters;
      parameters.fio2 = 60;
      parameters.flow = 40;
      parameters.mode = VentilationMode_hfnc;
      parameters.rr = 20;
      test_message.payload.set(parameters);
      test_message.payload.tag = PF::Application::MessageTypes::parameters;

      PF::Util::ByteVector<buffer_size> buffer;
      auto transform_status = sender.transform(test_message, buffer);

      THEN("The final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(buffer[0] == 0x04);
      }
    }

    WHEN("The message type value is greater than descriptor size") {
      constexpr auto message_descriptors =
      PF::Util::make_array<PF::Util::ProtobufDescriptor>(
          // array index should match the type code value
          PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>()
      );

      auto transform_status = sender.transform(test_message, buffer);
      test_message.payload.tag = PF::Application::MessageTypes::sensor_measurements;
      THEN("The final status should be invalid type") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The sensor measurments data from the message is written to the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageSender<TestMessage, num_descriptors> sender{BE::message_descriptors};

      SensorMeasurements sensor_measurments;
      sensor_measurments.flow = 30;
      sensor_measurments.spo2 = 92;
      sensor_measurments.fio2 = 50;
      test_message.payload.set(sensor_measurments);
      test_message.payload.tag = PF::Application::MessageTypes::sensor_measurements;

      PF::Util::ByteVector<buffer_size> buffer;
      auto transform_status = sender.transform(test_message, buffer);

      THEN("The final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }
  }
}
