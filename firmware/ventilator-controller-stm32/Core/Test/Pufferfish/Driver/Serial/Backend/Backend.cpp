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
using TestMessage = PF::Protocols::Message<
  PF::Application::StateSegment,
  payload_max_size
>;

TestMessage test_message;

SCENARIO("Serial::Backend behaves correctly", "[Backend]") {
  GIVEN("A Backend Reciever") {
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    Pufferfish::Driver::Serial::Backend::BackendReceiver backend_receiver{crc32c};

    auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x00", 11);

    PF::Driver::Serial::Backend::BackendReceiver::InputStatus input_status;

    WHEN("a valid body is parsed") {
        for (auto& ch : body) {
          input_status = backend_receiver.input(ch);
        }

      THEN("the final statuses, payload and length should be ok") {
        REQUIRE(input_status == PF::Driver::Serial::Backend::BackendReceiver::InputStatus::output_ready);
      }
    }

    AND_WHEN("input status is output ready") {
        for (auto& ch : body) {
          input_status = backend_receiver.input(ch);
        }

        Alarms alarms;
        alarms.alarm_one = true;
        test_message.payload.tag = PF::Application::MessageTypes::alarms;
        test_message.payload.set(alarms);

        auto output_status = backend_receiver.output(test_message);

      THEN("output status should be ok") {
        REQUIRE(output_status == PF::Driver::Serial::Backend::BackendReceiver::OutputStatus::available);
      }
    }
  }

  GIVEN("A backend sender") {
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    Pufferfish::Driver::Serial::Backend::BackendSender backend_sender{crc32c};

    constexpr size_t chunk_max_size = 256;

    PF::Util::ByteVector<chunk_max_size> chunkBuffer;

    Alarms alarms;
    alarms.alarm_one = true;
    test_message.payload.tag = PF::Application::MessageTypes::alarms;
    test_message.payload.set(alarms);

    WHEN("data is given to sender") {
      auto status = backend_sender.transform(test_message, chunkBuffer);

      THEN("status should be ok") {
        REQUIRE(status == PF::Driver::Serial::Backend::BackendSender::Status::ok);
      }
    }
  }
}
