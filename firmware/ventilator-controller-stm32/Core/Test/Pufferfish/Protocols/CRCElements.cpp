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

#include "Pufferfish/Protocols/CRCElements.h"
#include "Pufferfish/HAL/CRCChecker.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Protocols::CRCElement behaves correctly", "[CRCElement]") {
  GIVEN("A CRC element") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    TestCRCElementProps::PayloadBuffer buffer;
    TestCRCElement crc_element{buffer};
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Util::ByteVector<buffer_size> output_buffer;

    WHEN("data is written to it") {
      auto write_status = crc_element.write(output_buffer, crc32c);
      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }
    }

    WHEN("data is written to it and parsed back") {
      auto write_status = crc_element.write(output_buffer, crc32c);
      auto parse_status = crc_element.parse(output_buffer);
      THEN("the final statuses should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }
    }

  }

  GIVEN("A CRC element receiver") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    TestCRCElementProps::PayloadBuffer input_buffer;
    TestCRCElement crc_element{input_buffer};
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Protocols::CRCElementReceiver<buffer_size> crc_element_receiver{crc32c};

    WHEN("data is written to it") {
      auto write_status = crc_element.write(input_buffer, crc32c);
      auto status = crc_element_receiver.transform(input_buffer, crc_element);
      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::CRCElementReceiver<buffer_size>::Status::ok);
      }
    }
  }

  GIVEN("A CRC element sender") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    TestCRCElementProps::PayloadBuffer buffer;
    TestCRCElement crc_element{buffer};
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Protocols::CRCElementSender<buffer_size> crc_element_sender{crc32c};

    WHEN("data is written to it") {
      auto write_status = crc_element.write(buffer, crc32c);
      PF::Util::ByteVector<buffer_size> output_buffer;
      auto status = crc_element_sender.transform(buffer, output_buffer);
      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::CRCElementSender<buffer_size>::Status::ok);
      }
    }
  }

}
