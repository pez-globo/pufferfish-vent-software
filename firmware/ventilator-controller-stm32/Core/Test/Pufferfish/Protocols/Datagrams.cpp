/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Chunks.cpp
 *
 *  Created on: Nov 3, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of chunk splitter
 *
 */

#include "Pufferfish/Protocols/Datagrams.h"

#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Protocols::Datagram behaves correctly", "[Datagram]") {
  GIVEN("A Datagram") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    WHEN("a valid body is parsed") {
      auto body = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);
      auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      TestDatagramProps::PayloadBuffer payload;
      TestDatagram datagram{payload, 0};
      auto parse_status = datagram.parse(input_buffer);
      THEN("the final statuses, payload and length should be ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
        REQUIRE(payload == expected_payload);
        REQUIRE(datagram.length() == expected_payload.length());
      }
    }

    WHEN("an invalid body is parsed") {
      auto body = std::string("\x01\x00\x01\x02\x03\x04\x05", 7);
      auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      TestDatagramProps::PayloadBuffer payload;
      TestDatagram datagram{payload, 0};
      auto parse_status = datagram.parse(input_buffer);
      THEN("the final statuses and payload should be ok, but length should not match") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
        REQUIRE(payload == expected_payload);
        REQUIRE(datagram.length() != expected_payload.length());
      }
    }

    WHEN("a valid payload is written") {
      auto payload_data = std::string("\x01\x02\x03\x04\x05", 5);
      auto expected_body = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);

      TestDatagramProps::PayloadBuffer payload;
      PF::Util::convertStringToByteVector(payload_data, payload);

      TestDatagram datagram{payload, 1};

      PF::Util::ByteVector<buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("the final statuses and output_buffer should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(output_buffer == expected_body);
      }
    }

    WHEN("an invalid payload is written") {
      // This cannot be tested as any payload, valid or invalid,
      // will always be returned as-is.
    }
  }

  GIVEN("A Datagram receiver") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    WHEN("a valida body is received") {
      auto body = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
      auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      auto convert_status = PF::Util::convertStringToByteVector(body, input_buffer);

      TestDatagramProps::PayloadBuffer payload;
      TestDatagram datagram{payload};

      PF::Protocols::DatagramReceiver<buffer_size> datagram_receiver{};
      auto receive_status = datagram_receiver.transform(input_buffer, datagram);
      THEN("the final status should be ok") {
        REQUIRE(convert_status == true);
        REQUIRE(receive_status == PF::Protocols::DatagramReceiver<buffer_size>::Status::ok);
        REQUIRE(payload == expected_payload);
        REQUIRE(datagram.length() == expected_payload.length());
      }
    }

    WHEN("an invalid body is received") {
      auto body = std::string("\x00\x00\x01\x02\x03\x04\x05", 7);
      auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      auto convert_status = PF::Util::convertStringToByteVector(body, input_buffer);

      TestDatagramProps::PayloadBuffer payload;
      TestDatagram datagram{payload};

      PF::Protocols::DatagramReceiver<buffer_size> datagram_receiver{};
      auto receive_status = datagram_receiver.transform(input_buffer, datagram);
      THEN("the final status should be an error code, and length should not match") {
        REQUIRE(convert_status == true);
        REQUIRE(
            receive_status == PF::Protocols::DatagramReceiver<buffer_size>::Status::invalid_length);
        REQUIRE(payload == expected_payload);
        REQUIRE(datagram.length() != expected_payload.length());
      }
    }
  }

  GIVEN("A Datagram sender") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    TestDatagramProps::PayloadBuffer buffer;
    TestDatagram datagram{buffer};
    PF::Util::ByteVector<buffer_size> output_buffer;
    PF::Protocols::DatagramSender<buffer_size> datagram_sender{};

    WHEN("a valida body is being sent") {
      auto payload_data = std::string("\x01\x02\x03\x04\x05", 5);
      auto expected_body = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);

      TestDatagramProps::PayloadBuffer payload;
      auto convert_status = PF::Util::convertStringToByteVector(payload_data, payload);

      PF::Util::ByteVector<buffer_size> output_buffer;
      PF::Protocols::DatagramSender<buffer_size> datagram_sender{};
      auto send_status = datagram_sender.transform(payload, output_buffer);

      THEN("the final status should be ok") {
        REQUIRE(convert_status == true);
        REQUIRE(send_status == PF::Protocols::DatagramSender<buffer_size>::Status::ok);
        REQUIRE(output_buffer == expected_body);
      }
    }
  }
}
