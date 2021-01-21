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
#include "Pufferfish/Util/Array.h"
#include "Pufferfish/Util/Endian.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Protocols::CRCElement behaves correctly", "[CRCElement]") {

  constexpr size_t buffer_size = 254UL;
  using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
  using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
  using TestCRCElementHeaderProps = PF::Protocols::CRCElementHeaderProps;
  PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};

  GIVEN("A crc element is initalised with 0 bytes") {
    TestCRCElementProps::PayloadBuffer input_payload;

    TestCRCElement crc_element{input_payload};

    WHEN("the crc is computed from the payload") {
      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0x00000000;

      TestCRCElementProps::PayloadBuffer output_payload;
      output_payload.resize(TestCRCElementHeaderProps::header_size + input_payload.size());
      output_payload.copy_from(
        input_payload.buffer(), input_payload.size(), TestCRCElementHeaderProps::payload_offset);

      auto computed_crc = crc_element.compute_body_crc(output_payload, crc32c);

      THEN("the computed crc is equal to the expected crc") {
        REQUIRE(computed_crc == expected_crc);
      }
    }
  }

  GIVEN("A crc element is initalised with payload consisting of random string") {
    TestCRCElementProps::PayloadBuffer input_payload;
    auto data = std::string("123456789");
    for (auto& ch : data) {
      input_payload.push_back(ch);
    }

    TestCRCElement crc_element{input_payload};

    WHEN("the crc is computed from the payload") {
      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0xe3069283;

      TestCRCElementProps::PayloadBuffer output_payload;
      output_payload.resize(TestCRCElementHeaderProps::header_size + input_payload.size());
      output_payload.copy_from(
        input_payload.buffer(), input_payload.size(), TestCRCElementHeaderProps::payload_offset);

      auto computed_crc = crc_element.compute_body_crc(output_payload, crc32c);

      THEN("the computed crc is equal to the expected crc") {
        REQUIRE(computed_crc == expected_crc);
      }
    }
  }

  GIVEN("A crc element is initalised with payload 0x00") {
    TestCRCElementProps::PayloadBuffer input_payload;
    input_payload.push_back(0x00);

    TestCRCElement crc_element{input_payload};

    WHEN("the crc is computed from the payload") {
      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0x527d5351;

      TestCRCElementProps::PayloadBuffer output_payload;

      output_payload.resize(TestCRCElementHeaderProps::header_size + input_payload.size());
      output_payload.copy_from(
        input_payload.buffer(), input_payload.size(), TestCRCElementHeaderProps::payload_offset);

      auto computed_crc = crc_element.compute_body_crc(output_payload, crc32c);

      THEN("the computed crc is equal to the expected crc") {
        REQUIRE(computed_crc == expected_crc);
      }
    }
  }

  GIVEN("A crc element is initalised with payload 0x01") {
    TestCRCElementProps::PayloadBuffer input_payload;
    input_payload.push_back(0x01);

    TestCRCElement crc_element{input_payload};

    WHEN("the crc is computed from the payload") {
      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0xa016d052;

      TestCRCElementProps::PayloadBuffer output_payload;

      output_payload.resize(TestCRCElementHeaderProps::header_size + input_payload.size());
      output_payload.copy_from(
        input_payload.buffer(), input_payload.size(), TestCRCElementHeaderProps::payload_offset);

      auto computed_crc = crc_element.compute_body_crc(output_payload, crc32c);

      THEN("the computed crc is equal to the expected crc") {
        REQUIRE(computed_crc == expected_crc);
      }
    }
  }

  GIVEN("A crc element initalised with payload=[0x01, 0x02, 0x05]") {
    TestCRCElementProps::PayloadBuffer input_payload;
    auto data = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x05);
    for (auto& bytes : data) {
      input_payload.push_back(bytes);
    }

    TestCRCElement crc_element{input_payload};
    WHEN("given payload data is written to it") {

      TestCRCElementProps::PayloadBuffer output_payload;

      auto write_status = crc_element.write(output_payload, crc32c);
      auto crc = crc_element.crc();
      auto crc_compute = crc32c.compute(input_payload.buffer(), input_payload.size());

      THEN("crc computed in write method is same as crcchecker compute") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(crc == crc_compute);
      }
    }
  }

  GIVEN("crc element is initalised with 0 bytes") {

    TestCRCElementProps::PayloadBuffer input_payload;

    TestCRCElement crc_element{input_payload};

    WHEN("given payload data is written to it") {
      auto body = std::string("\x6D\x55\x1f\x4c\x03\x04\x00\xed\x30\x00", 10);

      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0x6D551F4C;

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto write_status = crc_element.write(input_buffer, crc32c);
      auto crc_payload = crc_element.crc();
      THEN("CRC is equal to the crc computed from initial payload") {
        REQUIRE(crc_payload == 0); // is it supposed to be 0 ?
      }
    }
  }

  GIVEN("A CRC element is initalised with valid payload parameter") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};

    auto expected_payload = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);
    auto input_data = PF::Util::make_array<uint8_t>(0x01, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05);
    TestCRCElementProps::PayloadBuffer input_payload;

    for (auto& data : input_data) {
      input_payload.push_back(data);
    }

    TestCRCElement crc_element{input_payload};

    WHEN("a body with good crc and payload fields is parsed") {
      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0x98DBE355;

      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);
      auto expected_payload = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = crc_element.parse(input_buffer);
      auto crc_check = crc_element.crc();
      const auto &payload_check = crc_element.payload();

      THEN("the parse status should be ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }

      THEN("crc from the CRCElement is equal to the expected crc for the given payload") {
        REQUIRE(crc_check == expected_crc);
      }

      THEN("payload from the CRCElement is equal to the expected payload from the body") {
        REQUIRE(payload_check == expected_payload);
      }
    }

    WHEN("body less than size of 4 bytes is parsed") {
      auto body = std::string("\x98\xdb\xe3", 3);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = crc_element.parse(input_buffer);

      THEN("the parse status is out of bounds") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }
    }

    WHEN("body with invalid crc is parsed") {
      auto body = std::string("\x12\x34\x56\x78\x03\x04\x00\xed\x30\x00", 10);
      auto expected_payload = std::string("\x03\x04\x00\xed\x30\x00", 6);

      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0x6D551F4C;

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = crc_element.parse(input_buffer);
      auto crc_check = crc_element.crc();

      auto payload_check = crc_element.payload();

      THEN("the parse status is ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }

      THEN("crc from the CRCElement is not equal to the expected crc for the given payload") {
        REQUIRE(crc_check != expected_crc);
      }

      THEN("payload from the CRCElement is equal to the expected payload from the body") {
        REQUIRE(payload_check == expected_payload);
      }
    }
  }

  GIVEN("A CRC element receiver") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    using TestCRCElementReceiver = PF::Protocols::CRCElementReceiver<buffer_size>;
    PF::Util::ByteVector<buffer_size> input_buffer;

    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Protocols::CRCElementReceiver<buffer_size> crc_element_receiver{crc32c};

    TestCRCElementProps::PayloadBuffer input_payload;
    TestCRCElement crc_element{input_payload};

    WHEN("body with valid crc and payload is given to crc element receiver") {
      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      uint32_t expected_crc = 0x98dbe355;
      auto expected_payload = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);

      auto transform_status = crc_element_receiver.transform(input_buffer, crc_element);
      auto crc = crc_element.crc();
      auto payload = crc_element.payload();

      THEN("the transform status for crc element receiver is ok") {
        REQUIRE(transform_status == TestCRCElementReceiver::Status::ok);
      }

      THEN("the crc in output crc_element is equal to the crc given in the input_buffer") {
        REQUIRE(crc == expected_crc);
      }

      THEN("the payload in the output crc element is equal to the payload given in the input_buffer") {
        REQUIRE(payload == expected_payload);
      }
    }

    WHEN("body with 0 bytes is given to the crc element receiver") {
      TestCRCElementProps::PayloadBuffer input_payload;
      TestCRCElement crc_element{input_payload};

      PF::Util::ByteVector<buffer_size> input_buffer;

      auto transform_status = crc_element_receiver.transform(input_buffer, crc_element);
      auto crc = crc_element.crc();

      uint32_t expected_crc = 0x00000000;

      THEN("the transform status is equal to invalid parse") {
        REQUIRE(transform_status == TestCRCElementReceiver::Status::invalid_parse);
      }

      THEN("the crc in output crc_element is equal to the given crc in input_buffer") {
        REQUIRE(crc == expected_crc);
      }
    }

    WHEN("input buffer crc is not equal to the actual crc computed from payload") {
      auto body = std::string("\x12\x34\x56\x78\x01\x05\x01\x02\x03\x04\x05", 11);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      uint32_t expected_crc = 0x12345678;

      auto transform_status = crc_element_receiver.transform(input_buffer, crc_element);
      auto crc = crc_element.crc();

      THEN("the transform status is equal to invalid crc") {
        REQUIRE(transform_status == TestCRCElementReceiver::Status::invalid_crc);
      }

      THEN("the crc in output crc_element is equal to the invalid crc given in input buffer") {
        REQUIRE(crc == expected_crc);
      }
    }
  }

  GIVEN("A CRC element sender") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;

    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Protocols::CRCElementSender<buffer_size> crc_element_sender{crc32c};

    WHEN("input buffer with valid crc and payload is given") {
      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);

      TestCRCElementProps::PayloadBuffer input_payload;
      PF::Util::convertStringToByteVector(body, input_payload);

      PF::Util::ByteVector<buffer_size> output_buffer;

      auto transform_status = crc_element_sender.transform(input_payload, output_buffer);
      auto output = PF::Util::convertByteVectorToHexString(output_buffer);

      THEN("the transform status is ok") {
        REQUIRE(transform_status == PF::Protocols::CRCElementSender<buffer_size>::Status::ok);
      }
    }
  }
}
