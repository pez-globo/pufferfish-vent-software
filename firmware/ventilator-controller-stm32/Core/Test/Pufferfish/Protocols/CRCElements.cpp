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
#include "Pufferfish/Test/Util.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Protocols::CRCElement behaves correctly", "[CRCElement]") {
  GIVEN("A CRC element") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};

    WHEN("when good data is given as input") {
      auto crc = std::string("\x98\xdb\xe3\x55");
      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05");
      auto expected_payload = std::string("\x01\x05\x01\x02\x03\x04\x05");
      uint32_t expected_crc = 0;
      PF::Util::read_ntoh(crc.c_str(), expected_crc);

      TestCRCElementProps::PayloadBuffer payload;
      TestCRCElement crc_element{payload};

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = crc_element.parse(input_buffer);
      auto crcCheck = crc_element.crc();
      auto &payloadCheck = crc_element.payload();
       THEN("the final status should be ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
        REQUIRE(crcCheck == expected_crc);
        REQUIRE(payloadCheck == expected_payload);
      }
    }

    WHEN("bad data is given as input"){
      auto crc = std::string("\x124Vx");
      auto body = std::string("\x12\x34\x56\x78\x03\x04\x00\xed\x30\x00");
      uint32_t expected_crc = 0;
      PF::Util::read_ntoh(crc.c_str(), expected_crc);
      auto expected_payload = std::string("\x03\x04\x00\xed\x30\x00");

      TestCRCElementProps::PayloadBuffer payload;
      TestCRCElement crc_element{payload};

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = crc_element.parse(input_buffer);
      auto crcCheck = crc_element.crc();
      auto payloadCheck = crc_element.payload();
      THEN("the final status should be ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
        REQUIRE(crcCheck != expected_crc);
        REQUIRE(payloadCheck == expected_payload);
      }
    }


    WHEN("data is written to it and parsed back") {
      auto crc = std::string("\x98\xdb\xe3\x55");
      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05");

      TestCRCElementProps::PayloadBuffer payload;
      TestCRCElement crc_element{payload};

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto write_status = crc_element.write(input_buffer, crc32c);
      auto parse_status = crc_element.parse(input_buffer);
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
    PF::Util::ByteVector<buffer_size> input_buffer;

    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Protocols::CRCElementReceiver<buffer_size> crc_element_receiver{crc32c};

    WHEN("good data is given as input to receiver") {
      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05");

      TestCRCElementProps::PayloadBuffer payload;
      TestCRCElement crc_element{payload};

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto write_status = crc_element.write(input_buffer, crc32c);
      auto status = crc_element_receiver.transform(input_buffer, crc_element);
      auto crcCheck = crc_element.crc();
      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::CRCElementReceiver<buffer_size>::Status::ok);
      }
    }

    WHEN("bad data is given as input to receiver") {
      // this is not tested as body for bad data is good!
    }
  }

  GIVEN("A CRC element sender") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;

    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Protocols::CRCElementSender<buffer_size> crc_element_sender{crc32c};

    WHEN("data is written to it") {
      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05");

      TestCRCElementProps::PayloadBuffer payload;
      TestCRCElement crc_element{payload};
      PF::Util::ByteVector<buffer_size> output_buffer;
      PF::Util::convertStringToByteVector(body, output_buffer);

      auto write_status = crc_element.write(payload, crc32c);
      auto status = crc_element_sender.transform(payload, output_buffer);

      THEN("the final status should be ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::CRCElementSender<buffer_size>::Status::ok);
      }
    }
  }

}
