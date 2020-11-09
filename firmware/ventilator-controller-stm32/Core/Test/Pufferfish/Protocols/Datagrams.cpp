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

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Protocols::Datagram behaves correctly", "[Datagram]") {
  GIVEN("A Datagram") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    TestDatagramProps::PayloadBuffer buffer;
    TestDatagram datagram{buffer};
    PF::Util::ByteVector<buffer_size> output_buffer;

    WHEN("data is written to it") {
      auto write_status = datagram.write(output_buffer);
      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }
    }

    WHEN("data is written to it and parsed back") {
      auto write_status = datagram.write(output_buffer);
      auto parse_status = datagram.parse(output_buffer);
      THEN("the final statuses should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }
    }

  }

  GIVEN("A Datagram receiver") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    TestDatagramProps::PayloadBuffer buffer;
    TestDatagram datagram{buffer};
    PF::Util::ByteVector<buffer_size> output_buffer;
    PF::Protocols::DatagramReceiver<buffer_size> datagram_receiver{};

    WHEN("data is written to it") {
      auto write_status = datagram.write(output_buffer);
      auto status = datagram_receiver.transform(output_buffer, datagram);
      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::DatagramReceiver<buffer_size>::Status::ok);
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

    WHEN("data is written to it") {
      auto write_status = datagram.write(buffer);
      PF::Util::ByteVector<buffer_size> output_buffer;
      auto status = datagram_sender.transform(buffer, output_buffer);
      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::DatagramSender<buffer_size>::Status::ok);
      }
    }
  }

}
