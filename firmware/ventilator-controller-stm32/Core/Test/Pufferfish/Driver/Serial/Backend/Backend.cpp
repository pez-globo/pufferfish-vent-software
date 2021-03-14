/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Chunks.cpp
 *
 *  Created on: Nov 17, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of the backend
 *
 */

#include "Pufferfish/Driver/Serial/Backend/Backend.h"

#include "Pufferfish/HAL/CRCChecker.h"
#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

constexpr size_t payload_max_size = 252UL;
using TestMessage = PF::Protocols::Message<PF::Application::StateSegment, payload_max_size>;

TestMessage test_message;

SCENARIO("Serial::Backend behaves correctly", "[Backend]") {
  GIVEN("A Backend Reciever") {
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    Pufferfish::Driver::Serial::Backend::BackendReceiver backend_receiver{crc32c};

    auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05\x00", 12);

    PF::Driver::Serial::Backend::BackendReceiver::InputStatus input_status;

    WHEN("a valid body is parsed") {
      for (auto& ch : body) {
        input_status = backend_receiver.input(ch);
      }

      THEN("input status should be ok") {
        REQUIRE(
            input_status ==
            PF::Driver::Serial::Backend::BackendReceiver::InputStatus::output_ready);
      }
    }

    AND_WHEN("input status is output ready") {
      for (auto& ch : body) {
        input_status = backend_receiver.input(ch);
      }

      ParametersRequest parameters_request;
      parameters_request.fio2 = 40;
      parameters_request.flow = 60;
      parameters_request.ventilating = true;
      parameters_request.mode = VentilationMode_hfnc;
      test_message.payload.tag = PF::Application::MessageTypes::parameters_request;
      test_message.payload.set(parameters_request);

      auto output_status = backend_receiver.output(test_message);

      THEN("output status should be ok") {
        REQUIRE(
            output_status == PF::Driver::Serial::Backend::BackendReceiver::OutputStatus::available);
      }
    }
  }

  GIVEN("A backend sender") {
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    Pufferfish::Driver::Serial::Backend::BackendSender backend_sender{crc32c};

    constexpr size_t chunk_max_size = 256;

    PF::Util::ByteVector<chunk_max_size> chunk_buffer;

    ParametersRequest parameters_request;
    parameters_request.fio2 = 40;
    parameters_request.flow = 60;
    parameters_request.ventilating = true;
    parameters_request.mode = VentilationMode_hfnc;
    test_message.payload.tag = PF::Application::MessageTypes::parameters_request;
    test_message.payload.set(parameters_request);

    WHEN("data is given to sender") {
      auto status = backend_sender.transform(test_message, chunk_buffer);

      THEN("status should be ok") {
        REQUIRE(status == PF::Driver::Serial::Backend::BackendSender::Status::ok);
      }
    }
  }

  GIVEN("A backend object") {
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Application::States states{};

    PF::Driver::Serial::Backend::Backend backend{crc32c, states};

    auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05\x00", 12);
    auto input_data = PF::Util::make_array<uint8_t>(0x01, 0x00, 0x83, 0x01, 0x80, 0x00, 0x05);

    WHEN("data is written to it") {
      PF::Driver::Serial::Backend::Backend::Status input_status;
      // for(auto& ch : body) {
      //   status = backend.input(ch);
      // }
      for (uint8_t index = 0; index < 5; index++) {
        input_status = backend.input(input_data[index]);
      }

      THEN("status should be ok") {
        REQUIRE(input_status == PF::Driver::Serial::Backend::Backend::Status::waiting);
      }
    }
    AND_WHEN("BLAH") {
      PF::Driver::Serial::Backend::Backend::Status input_status;

      for (uint8_t index = 0; index < 3; index++) {
        input_status = backend.input(input_data[index]);
      }

      THEN("status should be ok") {
        REQUIRE(input_status == PF::Driver::Serial::Backend::Backend::Status::ok);
      }
    }

    WHEN("output from backend is taken") {
      constexpr size_t chunk_max_size = 256;
      PF::Util::ByteVector<chunk_max_size> chunk_buffer;
      PF::Driver::Serial::Backend::Backend::Status input_status;
      for (auto& ch : body) {
        auto input_status = backend.input(ch);
      }

      auto status = backend.output(chunk_buffer);

      THEN("output status should be ok") {
        REQUIRE(input_status == PF::Driver::Serial::Backend::Backend::Status::ok);
        REQUIRE(status == PF::Driver::Serial::Backend::Backend::Status::ok);
      }
    }
  }
}
