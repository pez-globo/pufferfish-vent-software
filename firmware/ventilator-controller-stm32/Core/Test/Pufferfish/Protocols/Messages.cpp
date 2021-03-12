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

namespace PF = Pufferfish;
namespace BE = PF::Driver::Serial::Backend;

static constexpr size_t num_descriptors = 8;

auto exp_sensor_measurements =
    std::string("\x02\x25\x00\x00\xF0\x41\x35\x00\x00\xAA\x42\x3D\x00\x00\x90\x42", 16);
auto exp_cycle_measurements = std::string("\x03\x1D\x00\x00\x20\x41\x3D\x00\x00\x96\x43", 11);
auto exp_parameters = std::string("\x04\x10\x06\x45\x00\x00\x70\x42\x50\x01", 10);
auto exp_parameters_request = std::string("\x05\x10\x06\x45\x00\x00\xA0\x42\x50\x01", 10);
auto exp_alarm_limits = std::string("\x06\x12\x04\x08\x15\x10\x64", 7);
auto exp_alarm_limits_request = std::string("\x07\x12\x04\x08\x32\x10\x5C", 7);

SCENARIO(
    "Protocols::The message correctly writes to the output buffer and also updates type",
    "[messages]") {
  GIVEN(
      "A Message object constructed with StateSegment Taggedunion and a payload of size 252 "
      "bytes") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<
        PF::Application::StateSegment,
        PF::Application::MessageTypeValues,
        payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    WHEN("The index of payload tag is greater than the message descriptor array size") {
      constexpr auto message_descriptors = PF::Util::make_array<PF::Util::ProtobufDescriptor>(
          // array index should match the type code value
          PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>());

      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.flow = 60;
      test_message.payload.set(parameters_request);

      auto write_status = test_message.write(buffer, message_descriptors);

      THEN("The write status is equal to invalid type") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The Message type in the payload is Unrecognized") {
      test_message.payload.tag = PF::Application::MessageTypes::unknown;
      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.flow = 60;
      test_message.payload.value.parameters_request = parameters_request;

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to invalid type") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The output buffer cannot hold the payload data") {
      constexpr size_t size = 10UL;
      PF::Util::ByteVector<size> output_buffer;

      SensorMeasurements sensor_measurements;
      memset(&sensor_measurements, 0, sizeof(sensor_measurements));
      sensor_measurements.fio2 = 40;
      sensor_measurements.spo2 = 92;
      test_message.payload.set(sensor_measurements);

      auto write_status = test_message.write(output_buffer, BE::message_descriptors);

      THEN("The write status is equal to invalid length") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("The alarm limits data is written to a buffer of zero size") {
      constexpr size_t buffer_size = 0UL;
      PF::Util::ByteVector<buffer_size> buffer;
      test_message.payload.tag = PF::Application::MessageTypes::alarm_limits;
      auto status = test_message.write(buffer, BE::message_descriptors);

      THEN("The final status should be invalid_length") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("The sensor measurements data is written") {
      test_message.type = 4;
      SensorMeasurements sensor_measurements;
      memset(&sensor_measurements, 0, sizeof(sensor_measurements));
      sensor_measurements.flow = 60;

      test_message.payload.set(sensor_measurements);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The type field of message is unchanged after write method") {
        REQUIRE(test_message.type == 4);
      }
    }

    // sensor measurments
    WHEN("The payload is a sensor measurments message") {
      SensorMeasurements sensor_measurements;
      memset(&sensor_measurements, 0, sizeof(sensor_measurements));
      sensor_measurements.flow = 30;
      sensor_measurements.fio2 = 85;
      sensor_measurements.spo2 = 72;

      test_message.payload.set(sensor_measurements);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x02); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_sensor_measurements); }
    }

    // cycle measurments
    WHEN("The payload is a cycle measurements message") {
      CycleMeasurements cycle_measurements;
      memset(&cycle_measurements, 0, sizeof(cycle_measurements));
      cycle_measurements.ve = 300;
      cycle_measurements.rr = 10;

      test_message.payload.set(cycle_measurements);

      auto write_status = test_message.write(buffer, BE::message_descriptors);
      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x03); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_cycle_measurements); }
    }

    // parameters
    WHEN("The payload is a parameters message") {
      Parameters parameters;
      memset(&parameters, 0, sizeof(parameters));
      parameters.fio2 = 60;
      parameters.mode = VentilationMode_hfnc;
      parameters.ventilating = true;
      test_message.payload.set(parameters);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x04); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_parameters); }
    }

    // parameters request
    WHEN("The payload is a parameters request message") {
      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.fio2 = 80;
      parameters_request.mode = VentilationMode_hfnc;
      parameters_request.ventilating = true;

      test_message.payload.set(parameters_request);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x05); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_parameters_request); }
    }

    // alarm limits
    WHEN("The payload is a alarm limits message") {
      AlarmLimits alarm_limits = {};
      Range range = {};
      range.lower = 21;
      range.upper = 100;
      alarm_limits.has_fio2 = true;
      alarm_limits.fio2 = range;

      test_message.payload.set(alarm_limits);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x06); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_alarm_limits); }
    }

    // alarm limits
    WHEN("The alarm limits has_fio2 value is false") {
      AlarmLimits alarm_limits = {};
      Range range = {};
      range.lower = 21;
      range.upper = 100;
      alarm_limits.has_fio2 = false;
      alarm_limits.fio2 = range;

      test_message.payload.set(alarm_limits);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The fio2 field is not written to the buffer") {
        auto expected = std::string("\06", 1);
        REQUIRE(buffer == expected);
      }
    }

    // alarm limits request
    WHEN("The payload is a alarm limits request message") {
      AlarmLimitsRequest alarm_limits_request = {};
      Range range = {};
      range.lower = 50;
      range.upper = 92;
      alarm_limits_request.has_fio2 = true;
      alarm_limits_request.fio2 = range;

      test_message.payload.set(alarm_limits_request);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x07); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_alarm_limits_request); }
    }

    // alarm limits request
    WHEN("The alarm limits request has_fio2 value is false") {
      AlarmLimitsRequest alarm_limits_request = {};
      Range range = {};
      range.lower = 50;
      range.upper = 92;
      alarm_limits_request.has_fio2 = false;
      alarm_limits_request.fio2 = range;

      test_message.payload.set(alarm_limits_request);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The fio2 field is not written to the buffer") {
        auto expected = std::string("\07", 1);
        REQUIRE(buffer == expected);
      }
    }

    // field descriptor and payload values are different, yet message status is not invalid encoding
    // WHEN("Unkown message data is written") {
    //   test_message.payload.tag = PF::Application::MessageTypes::parameters;

    //   CycleMeasurements cycle_measurements;
    //   memset(&cycle_measurements, 0, sizeof(cycle_measurements));
    //   cycle_measurements.ve = 300;
    //   cycle_measurements.rr = 10;
    //   test_message.payload.value.cycle_measurements = cycle_measurements;

    //   auto write_status = test_message.write(buffer, BE::message_descriptors);

    //   THEN("The write status is equal to ok") {
    //     REQUIRE(write_status == PF::Protocols::MessageStatus::invalid_encoding);
    //   }
    // }
  }

  GIVEN(
      "A Message object constructed with StateSegment Taggedunion and a payload of size 126 "
      "bytes") {
    constexpr size_t payload_max_size = 126UL;
    using TestMessage = PF::Protocols::Message<
        PF::Application::StateSegment,
        PF::Application::MessageTypeValues,
        payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 126UL;
    PF::Util::ByteVector<buffer_size> buffer;

    WHEN("The sensor measurments message data is written") {
      SensorMeasurements sensor_measurements;
      memset(&sensor_measurements, 0, sizeof(sensor_measurements));
      sensor_measurements.flow = 30;
      sensor_measurements.fio2 = 85;
      sensor_measurements.spo2 = 72;

      test_message.payload.set(sensor_measurements);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x02); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_sensor_measurements); }
    }
  }

  GIVEN(
      "A Message object constructed with StateSegment Taggedunion and a payload of size 508 "
      "bytes") {
    constexpr size_t payload_max_size = 508UL;
    using TestMessage = PF::Protocols::Message<
        PF::Application::StateSegment,
        PF::Application::MessageTypeValues,
        payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 508UL;
    PF::Util::ByteVector<buffer_size> buffer;

    WHEN("The sensor measurments message data is written") {
      SensorMeasurements sensor_measurements;
      memset(&sensor_measurements, 0, sizeof(sensor_measurements));
      sensor_measurements.flow = 30;
      sensor_measurements.fio2 = 85;
      sensor_measurements.spo2 = 72;

      test_message.payload.set(sensor_measurements);

      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }

      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x02); }
      THEN("The output buffer is as expected") { REQUIRE(buffer == exp_sensor_measurements); }
    }
  }

  // fields and payload values are not of the same message type yet encoding is successful
  GIVEN("A TaggedUnion with a subset of message types") {
    constexpr size_t payload_max_size = 252UL;
    enum class MessageTypes : uint8_t {
      unknown = 0,
      parameters = 2,
      parameters_request = 3,
    };
    using TestTaggedUnion = PF::Util::TaggedUnion<PF::Application::StateSegmentUnion, MessageTypes>;
    using TestMessage = PF::Protocols::
        Message<TestTaggedUnion, PF::Application::MessageTypeValues, payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    WHEN("The cycle measurments message data is written") {
      constexpr auto message_descriptors = PF::Util::make_array<PF::Util::ProtobufDescriptor>(
          // array index should match the type code value
          PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>(),  // 0
          PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>(),  // 1
          PF::Util::get_protobuf_descriptor<SensorMeasurements>(),             // 2
          PF::Util::get_protobuf_descriptor<ParametersRequest>()               // 3
      );

      CycleMeasurements cycle_measurements;
      memset(&cycle_measurements, 0, sizeof(cycle_measurements));
      cycle_measurements.ve = 300;
      cycle_measurements.rr = 10;
      test_message.payload.value.cycle_measurements = cycle_measurements;

      test_message.payload.tag = MessageTypes::parameters;

      auto write_status = test_message.write(buffer, message_descriptors);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The buffer is as expected") {
        auto expected_buffer = std::string("\x02\x1D\x00\x00\x20\x41\x3D\x00\x00\x96\x43", 11);
        REQUIRE(buffer == expected_buffer);
      }
    }
  }
}

SCENARIO(
    "Protocols::The Messages class correctly parses the input buffer and updates type and payload "
    "fields",
    "[messages]") {
  GIVEN(
      "A Message object constructed with StateSegment Taggedunion and a payload of size 252 "
      "bytes") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<
        PF::Application::StateSegment,
        PF::Application::MessageTypeValues,
        payload_max_size>;
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

    WHEN("The first byte of input buffer is greater than descriptor array size") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto data = PF::Util::make_array<uint8_t>(0x08);  // SIGSEGV - Segmentation violation signal
      for (auto& input : data) {
        input_buffer.push_back(input);
      }

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The parse status is equal to invalid length") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The data from an invalid buffer is parsed") {
      constexpr size_t buffer_size = 253UL;
      PF::Util::ByteVector<buffer_size> buffer;
      auto data = PF::Util::make_array<uint8_t>(0x06);
      for (auto& input : data) {
        buffer.push_back(input);
      }
      buffer.resize(buffer_size);
      auto status = test_message.parse(buffer, BE::message_descriptors);

      THEN("The parse status is equal to invalid encoding") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_encoding);
      }
    }

    WHEN("The input buffer has invalid data for the payload fields") {
      constexpr size_t buffer_size = 253UL;
      PF::Util::ByteVector<buffer_size> buffer;
      auto data = std::string("\x02\x08\xa0\x10\x0A\x1D\x00\x00\xA0\x41", 10);
      PF::Util::convertStringToByteVector(data, buffer);

      auto status = test_message.parse(buffer, BE::message_descriptors);

      THEN("The parse status is equal to invalid encoding") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_encoding);
      }
    }

    // sensor measurements
    WHEN("The buffer of sensor measurments is parsed") {
      auto data = std::string("\x02\x08\x02\x10\x0A\x1D\x00\x00\xA0\x41", 10);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The type field of message class is equal to 2") { REQUIRE(test_message.type == 2); }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::sensor_measurements);
        REQUIRE(test_message.payload.value.sensor_measurements.cycle == 10);
        REQUIRE(test_message.payload.value.sensor_measurements.paw == 20);
        REQUIRE(test_message.payload.value.sensor_measurements.time == 2);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == data); }
    }

    // cycle measurements
    WHEN("The buffer of cycle measurements is parsed") {
      auto data = std::string("\x03\x25\x00\x00\xF0\x41\x35\x00\x00\x20\x41", 11);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The type field of message class is equal to 2") { REQUIRE(test_message.type == 3); }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::cycle_measurements);
        REQUIRE(test_message.payload.value.cycle_measurements.peep == 30);
        REQUIRE(test_message.payload.value.cycle_measurements.ip == 10);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == data); }
    }

    // parameters
    WHEN("The buffer of parameters is parsed") {
      auto data = std::string("\x04\x10\x06\x2D\x00\x00\x34\x42\x50\x01", 10);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The type field of message class is equal to 2") { REQUIRE(test_message.type == 4); }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters);
        REQUIRE(test_message.payload.value.parameters.vt == 45);
        REQUIRE(test_message.payload.value.parameters.ventilating == true);
        REQUIRE(test_message.payload.value.parameters.mode == VentilationMode_hfnc);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == data); }
    }

    // parameters request
    WHEN("The buffer of parameters request is parsed") {
      auto data = std::string("\x05\x3D\x00\x00\xA0\x41", 6);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The type field of message class is equal to 2") { REQUIRE(test_message.type == 5); }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(test_message.payload.value.parameters_request.ie == 20);
        REQUIRE(test_message.payload.value.parameters_request.time == 0);
        REQUIRE(test_message.payload.value.parameters_request.ventilating == false);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == data); }
    }

    // alarm limits
    WHEN("The buffer of alarm limits is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_alarm_limits, input_buffer);

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The type field of message class is equal to 2") { REQUIRE(test_message.type == 6); }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.lower == 21);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.upper == 100);
        REQUIRE(test_message.payload.value.alarm_limits.has_fio2 == true);
      }
      THEN("The input buffer is unchanged after parse") {
        REQUIRE(input_buffer == exp_alarm_limits);
      }
    }

    // alarm limits request
    WHEN("The buffer of alarm limits request is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_alarm_limits_request, input_buffer);

      auto parse_status = test_message.parse(input_buffer, BE::message_descriptors);

      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The type field of message class is equal to 2") { REQUIRE(test_message.type == 7); }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits_request);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.lower == 50);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.upper == 92);
        REQUIRE(test_message.payload.value.alarm_limits.has_fio2 == true);
      }
      THEN("The input buffer is unchanged after parse") {
        REQUIRE(input_buffer == exp_alarm_limits_request);
      }
    }
  }
}

SCENARIO(
    "Protocols::The Message class correctly writes to a buffer then parses from it", "[messages]") {
  GIVEN(
      "A Message object constructed with StateSegment Taggedunion and a payload of size 252 "
      "bytes") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<
        PF::Application::StateSegment,
        PF::Application::MessageTypeValues,
        payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    WHEN("The parameters request message type is written and then parsed from the buffer") {
      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.ventilating = true;
      parameters_request.fio2 = 40;
      parameters_request.mode = VentilationMode_hfnc;
      parameters_request.flow = 60;
      test_message.payload.set(parameters_request);

      PF::Util::ByteVector<buffer_size> buffer;

      // write
      auto write_status = test_message.write(buffer, BE::message_descriptors);

      THEN("The status of write function should be ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x05); }
      THEN("The output buffer is as expected") {
        auto expected =
            std::string("\x05\x10\x06\x45\x00\x00\x20\x42\x4D\x00\x00\x70\x42\x50\x01", 15);
        REQUIRE(buffer == expected);
      }

      // parse
      auto parse_status = test_message.parse(buffer, BE::message_descriptors);

      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(test_message.payload.value.parameters_request.fio2 == 40);
        REQUIRE(test_message.payload.value.parameters_request.flow == 60);
      }
      THEN("The type field of message class is equal to 5") { REQUIRE(test_message.type == 5); }
      THEN("The input buffer is as expected") {
        auto expected =
            std::string("\x05\x10\x06\x45\x00\x00\x20\x42\x4D\x00\x00\x70\x42\x50\x01", 15);
        REQUIRE(buffer == expected);
      }

      // write
      SensorMeasurements sensor_measurements;
      memset(&sensor_measurements, 0, sizeof(sensor_measurements));
      sensor_measurements.flow = 40;
      sensor_measurements.spo2 = 80;
      test_message.payload.set(sensor_measurements);

      auto status = test_message.write(buffer, BE::message_descriptors);

      THEN("The status of write function should be ok") {
        REQUIRE(status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") { REQUIRE(buffer[0] == 0x02); }
      THEN("The output buffer is as expected") {
        auto expected = std::string("\x02\x25\x00\x00\x20\x42\x3D\x00\x00\xA0\x42", 11);
        REQUIRE(buffer == expected);
      }
    }
  }
}

SCENARIO(
    "Protocols::The Message Receiver class correctly transforms messages into paylaods",
    "[messages]") {
  GIVEN("A MessageReceiver object is constructed with default parameters") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<
        PF::Application::StateSegment,
        PF::Application::MessageTypeValues,
        payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> buffer;

    PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

    WHEN("The input buffer size is smaller than message header size") {
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{
          BE::message_descriptors};
      PF::Util::ByteVector<buffer_size> input_buffer;

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be invalid_length") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("The type offset of the input buffer is invalid") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{
          BE::message_descriptors};

      auto data = std::string("\x00", 1);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be invalid_length") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The payload of the input buffer is invalid") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      // std::string("\x02\x08\x02\x10\x0A\x1D\x00\x00\xA0\x41", 10); original buffer
      auto data = std::string(
          "\x02\x08\xa0\x10\x0A\x1D\x00\x00\xA0\x41", 10);  // 3rd byte changed to random value
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be invalid encoding") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_encoding);
      }
    }

    WHEN("A MessageReceiver object is initialised with a smaller descriptors array") {
      constexpr auto message_descriptors = PF::Util::make_array<PF::Util::ProtobufDescriptor>(
          // array index should match the type code value
          PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>(),  // 0
          PF::Util::get_protobuf_descriptor<ParametersRequest>(),              // 1
          PF::Util::get_protobuf_descriptor<AlarmLimits>(),                    // 2
          PF::Util::get_protobuf_descriptor<AlarmLimitsRequest>(),             // 3
          PF::Util::get_protobuf_descriptor<Parameters>()                      // 4
      );
      constexpr size_t number_desc = 5;
      PF::Protocols::MessageReceiver<TestMessage, number_desc> receiver{message_descriptors};

      auto data = std::string("\x04\x4D\x00\x00\xF0\x41", 6);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters);
        REQUIRE(test_message.payload.value.parameters.flow == 30);
      }
    }

    WHEN("The message has some data prior to transform") {
      CycleMeasurements cycle_measurements;
      memset(&cycle_measurements, 0, sizeof(cycle_measurements));
      cycle_measurements.ve = 300;
      cycle_measurements.rr = 10;

      test_message.payload.set(cycle_measurements);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_parameters, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters);
        REQUIRE(test_message.payload.value.parameters.fio2 == 60);
        REQUIRE(test_message.payload.value.parameters.mode == VentilationMode_hfnc);
        REQUIRE(test_message.payload.value.parameters.ventilating == true);
      }
      THEN("The type field of message class is equal to 4") { REQUIRE(test_message.type == 4); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_parameters);
      }
    }

    WHEN("The input buffer is changed after transform is called on it") {
      auto parameters_data = std::string("\x04\x10\x06\x45\x00\x00\x70\x42\x50\x01", 10);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(parameters_data, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters);
        REQUIRE(test_message.payload.value.parameters.fio2 == 60);
        REQUIRE(test_message.payload.value.parameters.mode == VentilationMode_hfnc);
        REQUIRE(test_message.payload.value.parameters.ventilating == true);
      }
      THEN("The type field of message class is equal to 4") { REQUIRE(test_message.type == 4); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_parameters);
      }

      parameters_data.append("\x02", 1);

      THEN("The message payload values are unchanged") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters);
        REQUIRE(test_message.payload.value.parameters.fio2 == 60);
        REQUIRE(test_message.payload.value.parameters.mode == VentilationMode_hfnc);
        REQUIRE(test_message.payload.value.parameters.ventilating == true);
      }
    }

    // sensor measurements
    WHEN("The buffer of sensor measurements is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_sensor_measurements, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::sensor_measurements);
        REQUIRE(test_message.payload.value.sensor_measurements.flow == 30);
        REQUIRE(test_message.payload.value.sensor_measurements.fio2 == 85);
        REQUIRE(test_message.payload.value.sensor_measurements.spo2 == 72);
      }
      THEN("The type field of message class is equal to 3") { REQUIRE(test_message.type == 2); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_sensor_measurements);
      }
    }

    // cycle measurements
    WHEN("The buffer of cycle measurements is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_cycle_measurements, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::cycle_measurements);
        REQUIRE(test_message.payload.value.cycle_measurements.ve == 300);
        REQUIRE(test_message.payload.value.cycle_measurements.rr == 10);
      }
      THEN("The type field of message class is equal to 3") { REQUIRE(test_message.type == 3); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_cycle_measurements);
      }
    }

    // parameters
    WHEN("The buffer of parameters is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_parameters, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters);
        REQUIRE(test_message.payload.value.parameters.fio2 == 60);
        REQUIRE(test_message.payload.value.parameters.mode == VentilationMode_hfnc);
        REQUIRE(test_message.payload.value.parameters.ventilating == true);
      }
      THEN("The type field of message class is equal to 4") { REQUIRE(test_message.type == 4); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_parameters);
      }
    }

    // parameters request
    WHEN("The buffer of parameters request is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_parameters_request, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(test_message.payload.value.parameters_request.fio2 == 80);
        REQUIRE(test_message.payload.value.parameters_request.mode == VentilationMode_hfnc);
        REQUIRE(test_message.payload.value.parameters_request.ventilating == true);
      }
      THEN("The type field of message class is equal to 5") { REQUIRE(test_message.type == 5); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_parameters_request);
      }
    }

    // alarm limits
    WHEN("The buffer of alarm limits is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_alarm_limits, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.lower == 21);
        REQUIRE(test_message.payload.value.alarm_limits.fio2.upper == 100);
      }
      THEN("The type field of message class is equal to 6") { REQUIRE(test_message.type == 6); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_alarm_limits);
      }
    }

    // alarm limits request
    WHEN("The buffer of alarm limits request is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(exp_alarm_limits_request, input_buffer);

      auto transform_status = receiver.transform(input_buffer, test_message);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("The message payload values are as expected") {
        REQUIRE(test_message.payload.tag == PF::Application::MessageTypes::alarm_limits_request);
        REQUIRE(test_message.payload.value.alarm_limits_request.has_fio2 == true);
        REQUIRE(test_message.payload.value.alarm_limits_request.fio2.lower == 50);
        REQUIRE(test_message.payload.value.alarm_limits_request.fio2.upper == 92);
      }
      THEN("The type field of message class is equal to 7") { REQUIRE(test_message.type == 7); }
      THEN("The input buffer is unchanged after transform") {
        REQUIRE(input_buffer == exp_alarm_limits_request);
      }
    }
  }
}

SCENARIO(
    "Protocols::The Message Sender class correctly transforms payloads into messages",
    "[messages]") {
  GIVEN("A MessageSender object is constructed with default parameters") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<
        PF::Application::StateSegment,
        PF::Application::MessageTypeValues,
        payload_max_size>;
    TestMessage test_message;
    constexpr size_t buffer_size = 252UL;
    PF::Util::ByteVector<buffer_size> output_buffer;

    PF::Protocols::MessageSender<TestMessage, num_descriptors> sender{BE::message_descriptors};

    WHEN("The message type value is greater than descriptor size") {
      constexpr size_t num_descriptors = 1;
      constexpr auto message_descriptors = PF::Util::make_array<PF::Util::ProtobufDescriptor>(
          // array index should match the type code value
          PF::Util::get_protobuf_descriptor<PF::Util::UnrecognizedMessage>());

      PF::Protocols::MessageSender<TestMessage, num_descriptors> sender{message_descriptors};

      test_message.payload.tag = PF::Application::MessageTypes::sensor_measurements;

      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be invalid type") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The message type value is unknown") {
      test_message.payload.tag = PF::Application::MessageTypes::unknown;

      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be invalid type") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("The output buffer cannot hold message data") {
      constexpr size_t buffer_size = 0UL;
      PF::Util::ByteVector<buffer_size> output_buffer;

      test_message.payload.tag = PF::Application::MessageTypes::sensor_measurements;

      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be invalid type") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    // sensor measurments
    WHEN("The sensor measurments data from the message is written to the buffer") {
      SensorMeasurements sensor_measurements;
      memset(&sensor_measurements, 0, sizeof(sensor_measurements));
      sensor_measurements.flow = 30;
      sensor_measurements.fio2 = 85;
      sensor_measurements.spo2 = 72;
      test_message.payload.set(sensor_measurements);

      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(output_buffer[0] == 0x02);
      }
      THEN("The output buffer is as expected") {
        REQUIRE(output_buffer == exp_sensor_measurements);
      }
    }

    // cycle measurements
    WHEN("The cycle measurments data from the message is written to the buffer") {
      CycleMeasurements cycle_measurements;
      memset(&cycle_measurements, 0, sizeof(cycle_measurements));
      cycle_measurements.ve = 300;
      cycle_measurements.rr = 10;

      test_message.payload.set(cycle_measurements);

      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(output_buffer[0] == 0x03);
      }
      THEN("The output buffer is as expected") { REQUIRE(output_buffer == exp_cycle_measurements); }
    }

    // parameters
    WHEN("The parameters data from the message is written to the buffer") {
      Parameters parameters;
      memset(&parameters, 0, sizeof(parameters));
      parameters.fio2 = 60;
      parameters.mode = VentilationMode_hfnc;
      parameters.ventilating = true;
      test_message.payload.set(parameters);

      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(output_buffer[0] == 0x04);
      }
      THEN("The output buffer is as expected") { REQUIRE(output_buffer == exp_parameters); }
    }

    // parameters request
    WHEN("The parameters request data from the message is written to the buffer") {
      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.fio2 = 80;
      parameters_request.mode = VentilationMode_hfnc;
      parameters_request.ventilating = true;

      test_message.payload.set(parameters_request);

      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(output_buffer[0] == 0x05);
      }
      THEN("The output buffer is as expected") { REQUIRE(output_buffer == exp_parameters_request); }
    }

    // alarm limits
    WHEN("The alarm limits data from the message is written to the buffer") {
      AlarmLimits alarm_limits = {};
      Range range = {};
      range.lower = 21;
      range.upper = 100;
      alarm_limits.has_fio2 = true;
      alarm_limits.fio2 = range;

      test_message.payload.set(alarm_limits);
      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(output_buffer[0] == 0x06);
      }
      THEN("The output buffer is as expected") { REQUIRE(output_buffer == exp_alarm_limits); }
    }

    // alarm limits request
    WHEN("The alarm limits request data from the message is written to the buffer") {
      AlarmLimitsRequest alarm_limits_request = {};
      Range range = {};
      range.lower = 50;
      range.upper = 92;
      alarm_limits_request.has_fio2 = true;
      alarm_limits_request.fio2 = range;

      test_message.payload.set(alarm_limits_request);
      auto transform_status = sender.transform(test_message, output_buffer);

      THEN("The transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
      THEN("first byte in the output buffer is equal to the type") {
        REQUIRE(output_buffer[0] == 0x07);
      }
      THEN("The output buffer is as expected") {
        REQUIRE(output_buffer == exp_alarm_limits_request);
      }
    }
  }
}
