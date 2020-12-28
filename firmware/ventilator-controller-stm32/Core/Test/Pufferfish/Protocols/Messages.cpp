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
#include "Pufferfish/Test/BackendDefs.h"
#include "Pufferfish/Protocols/Chunks.h"
#include "Pufferfish/Util/Vector.h"
#include "Pufferfish/Util/Array.h"
#include "nanopb/pb.h"
#include "Pufferfish/Application/mcu_pb.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;
namespace BE = PF::Driver::Serial::Backend;

static constexpr size_t num_descriptors = 7;

SCENARIO("Protocols::Message behaves correctly", "[messages]") {
  GIVEN("A protocol-buffer message") {
    constexpr size_t payload_max_size = 252UL;
    using TestMessage = PF::Protocols::Message<
      PF::Application::StateSegment,
      payload_max_size
    >;
    TestMessage test_message;

    WHEN("the regular parameters request data is written to the buffer") {
      constexpr size_t buffer_size = 252UL;
      PF::Util::ByteVector<buffer_size> buffer;

      ParametersRequest parameters_request;
      parameters_request.ventilating=true;
      parameters_request.fio2=40;
      parameters_request.mode=VentilationMode_hfnc;
      parameters_request.flow=60;
      test_message.payload.set(parameters_request);
      test_message.payload.tag= PF::Application::MessageTypes::parameters_request;

      auto status = test_message.write(buffer, BE::message_descriptors);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("the unknown data is written to the buffer") {
      constexpr size_t buffer_size = 252UL;
      test_message.payload.tag = PF::Application::MessageTypes::unknown;
      PF::Util::ByteVector<buffer_size> buffer;
      auto status = test_message.write(buffer, BE::message_descriptors);

      THEN("the final status should be invalid_type") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("the regular alarm limits data is written to a buffer of zero size") {
      constexpr size_t buffer_size = 0UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarm_limits;
      PF::Util::ByteVector<buffer_size> buffer;
      auto status = test_message.write(buffer, BE::message_descriptors);

      THEN("the final status should be invalid_length") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("the regular parameters request data is parsed from the buffer") {
      constexpr size_t buffer_size = 253UL;

      ParametersRequest parameters_request;
      parameters_request.ventilating=true;
      parameters_request.fio2=40;
      parameters_request.mode=VentilationMode_hfnc;
      parameters_request.flow=60;
      test_message.payload.set(parameters_request);
      test_message.payload.tag= PF::Application::MessageTypes::parameters_request;

      PF::Util::ByteVector<buffer_size> buffer;
      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::parameters_request);
      auto parse_status = test_message.parse(buffer, BE::message_descriptors);

      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::Protocols::MessageStatus::ok);
        REQUIRE(parse_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("data is parsed from an invalid buffer") {
      constexpr size_t buffer_size = 253UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarm_limits;
      PF::Util::ByteVector<buffer_size> buffer;
      buffer.resize(buffer_size);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::alarm_limits);
      auto status = test_message.parse(buffer, BE::message_descriptors);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::MessageStatus::invalid_encoding);
      }
    }

    WHEN("data is parsed with an invalid type") {
      constexpr size_t buffer_size = 253UL;
      test_message.payload.tag = PF::Application::MessageTypes::alarm_limits;
      PF::Util::ByteVector<buffer_size> buffer;
      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::unknown);
      auto parse_status = test_message.parse(buffer, BE::message_descriptors);

      THEN("the final status should be invalid_type") {
        REQUIRE(parse_status == PF::Protocols::MessageStatus::invalid_type);
      }
    }

    WHEN("the regular parameters data is parsed from the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageSender<TestMessage, num_descriptors> sender{BE::message_descriptors};

      Parameters parameters;
      parameters.fio2=60;
      parameters.flow=40;
      parameters.mode=VentilationMode_hfnc;
      parameters.rr=20;
      test_message.payload.set(parameters);
      test_message.payload.tag= PF::Application::MessageTypes::parameters;

      PF::Util::ByteVector<buffer_size> buffer;
      auto transform_status = sender.transform(test_message, buffer);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("the regular sensor measurments data is parsed from the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageSender<TestMessage, num_descriptors> sender{BE::message_descriptors};

      SensorMeasurements sensor_measurments;
      sensor_measurments.flow=30;
      sensor_measurments.spo2=92;
      sensor_measurments.fio2=50;
      test_message.payload.set(sensor_measurments);
      test_message.payload.tag= PF::Application::MessageTypes::sensor_measurements;

      PF::Util::ByteVector<buffer_size> buffer;
      auto transform_status = sender.transform(test_message, buffer);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("bad data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;

      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::unknown);

      auto transform_status = receiver.transform(buffer, test_message);
      THEN("the final status should be invalid_length") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::invalid_length);
      }
    }

    WHEN("the regular cycle measurments data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;
      CycleMeasurements cycleMeasurements;
      cycleMeasurements.ve = 300;
      test_message.payload.set(cycleMeasurements);
      test_message.payload.tag = PF::Application::MessageTypes::cycle_measurements;

      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::cycle_measurements);

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("sensor measurments data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;
      SensorMeasurements sensor;
      sensor.paw = 20;
      test_message.payload.set(sensor);
      test_message.payload.tag = PF::Application::MessageTypes::sensor_measurements;

      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::sensor_measurements);

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("cycle measurments data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;
      CycleMeasurements cycleMeasurements;
      cycleMeasurements.ve = 300;
      test_message.payload.set(cycleMeasurements);
      test_message.payload.tag = PF::Application::MessageTypes::cycle_measurements;

      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::cycle_measurements);

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }

    }

    WHEN("parameters data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;
      Parameters parameters;
      parameters.fio2 = 80;
      test_message.payload.set(parameters);
      test_message.payload.tag = PF::Application::MessageTypes::parameters;

      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::parameters);

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("parameters request data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};

      PF::Util::ByteVector<buffer_size> buffer;
      ParametersRequest parametersRequest;
      parametersRequest.fio2 = 60;
      test_message.payload.set(parametersRequest);
      test_message.payload.tag = PF::Application::MessageTypes::parameters_request;

      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::parameters_request);

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

    WHEN("alarm limits data is written into the buffer") {
      constexpr size_t buffer_size = 253UL;
      PF::Protocols::MessageReceiver<TestMessage, num_descriptors> receiver{BE::message_descriptors};
      Range range1;
      range1.lower=50;
      range1.upper=70;

      Range range2;
      range2.lower=30;
      range2.upper=60;

      PF::Util::ByteVector<buffer_size> buffer;
      AlarmLimits alarm_limits;
      alarm_limits.fio2=range1;
      alarm_limits.flow=range2;
      test_message.payload.set(alarm_limits);

      auto write_status = test_message.write(buffer, BE::message_descriptors);
      buffer[TestMessage::type_offset] = static_cast<uint8_t>(PF::Application::MessageTypes::alarm_limits);

      auto transform_status = receiver.transform(buffer, test_message);

      THEN("the final status should be ok") {
        REQUIRE(transform_status == PF::Protocols::MessageStatus::ok);
      }
    }

  }
}
