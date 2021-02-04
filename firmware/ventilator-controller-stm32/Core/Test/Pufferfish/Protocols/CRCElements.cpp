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

constexpr size_t buffer_size = 254UL;
using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
using TestCRCElementHeaderProps = PF::Protocols::CRCElementHeaderProps;
PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};

SCENARIO(
    "Protocols::CRCElement: The compute_body_crc function correctly computes crc from body",
    "[CRCElement]") {
  GIVEN("A CRC element initialized with an empty payload") {
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

  GIVEN("A CRC Element initialized with the payload '123456789'") {
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
}

SCENARIO(
    "Protocols::CRCElement: The write function correctly generates body from computed crc and "
    "payload",
    "[CRCElement]") {
  GIVEN("A crc element initalised with payload=[0x01, 0x02, 0x05]") {
    TestCRCElementProps::PayloadBuffer input_payload;
    auto data = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x05);
    for (auto& bytes : data) {
      input_payload.push_back(bytes);
    }

    TestCRCElement crc_element{input_payload};
    WHEN("The crc and payload are written to the output buffer") {
      TestCRCElementProps::PayloadBuffer output_buffer;
      auto expected = std::string("\xD7\x91\x15\xF6\x01\x02\x05");

      auto write_status = crc_element.write(output_buffer, crc32c);
      auto crc_compute = crc32c.compute(input_payload.buffer(), input_payload.size());

      THEN("crc computed in write method is same as crcchecker compute") {
        REQUIRE(write_status == PF::IndexStatus::ok);
        REQUIRE(crc_element.crc() == crc_compute);
      }

      THEN("The output buffer is as expected") { REQUIRE(output_buffer == expected); }
    }
  }

  GIVEN("crc element is initalised with 0 bytes") {
    TestCRCElementProps::PayloadBuffer input_payload;

    TestCRCElement crc_element{input_payload};

    WHEN("The crc and payload are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = crc_element.write(output_buffer, crc32c);
      auto crc_payload = crc_element.crc();
      THEN("CRC is equal to 0") { REQUIRE(crc_payload == 0); }
      THEN("The output buffer is as expected") {
        PF::Util::ByteVector<buffer_size> expected_buffer;
        auto input_data = PF::Util::make_array<uint8_t>(0x00, 0x00, 0x00, 0x00);
        for (auto& data : input_data) {
          expected_buffer.push_back(data);
        }
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }
}

SCENARIO(
    "Protocols::CRCElement: The parse function correctly updates internal crc and payload fields "
    "from the input buffer",
    "[CRCElement]") {
  GIVEN("A CRC Element initialized with an empty payload buffer with a capacity of 254 bytes") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};

    TestCRCElementProps::PayloadBuffer payload;

    TestCRCElement crc_element{payload};

    WHEN("a body with good crc and payload fields is parsed") {
      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0x98DBE355;

      auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);
      auto expected_payload = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = crc_element.parse(input_buffer);
      auto crc_check = crc_element.crc();
      const auto& payload_check = crc_element.payload();

      THEN("the parse status should be ok") { REQUIRE(parse_status == PF::IndexStatus::ok); }

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

      THEN("the parse status is ok") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN("The crc from the CRCElement is equal to the crc in the body/input_buffer") {
        uint32_t crc_body = 0x12345678;
        REQUIRE(crc_check == crc_body);
      }
      THEN("crc from the CRCElement is not equal to the expected crc for the given payload") {
        REQUIRE(crc_check != expected_crc);
      }
      THEN("payload from the CRCElement is equal to the expected payload from the body") {
        REQUIRE(payload_check == expected_payload);
      }
    }
  }
}

SCENARIO("Protocols::CRCElement: The crc element correctly writes and parses to a buffer") {
  GIVEN("A CRC Element initialized with a non empty payload buffer with a capacity of 254 bytes") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElement = PF::Protocols::CRCElement<TestCRCElementProps::PayloadBuffer>;
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};

    auto data = PF::Util::make_array<uint8_t>(0x01, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05);

    TestCRCElementProps::PayloadBuffer payload;
    for (auto& bytes : data) {
      payload.push_back(bytes);
    }

    TestCRCElement crc_element{payload};

    WHEN("The body is generated and then parsed") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = crc_element.write(output_buffer, crc32c);

      THEN("The status of write function returns ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }
      THEN("The crc from the crc element is equal to expected") {
        // Calculated using the Sunshine Online CRC Calculator
        uint32_t expected_crc = 0x98DBE355;
        REQUIRE(crc_element.crc() == expected_crc);
      }
      THEN("The payload of the crc element is as expected") {
        auto expected_payload = std::string("\x01\x05\x01\x02\x03\x04\x05");
        REQUIRE(crc_element.payload() == expected_payload);
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\x98\xDB\xE3\x55\x01\x05\x01\x02\x03\x04\x05");
        REQUIRE(output_buffer == expected_buffer);
      }

      auto parse_status = crc_element.parse(output_buffer);
      THEN("The status of parse function returns ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }
      THEN("The crc from the CRCElement is equal to the crc in the body") {
        uint32_t crc_body = 0x98DBE355;
        REQUIRE(crc_element.crc() == crc_body);
      }
      THEN("payload from the CRCElement is equal to the expected payload from the body") {
        auto expected_payload = std::string("\x01\x05\x01\x02\x03\x04\x05");
        REQUIRE(crc_element.payload() == expected_payload);
      }
    }
  }
}

SCENARIO(
    "Protocols::CRCElementReceiver: The crc element receiver correctly parses datagrams into "
    "payloads",
    "[CRCElementReceiver]") {
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
      auto payload = crc_element.payload();

      THEN("the transform status for crc element receiver is ok") {
        REQUIRE(transform_status == TestCRCElementReceiver::Status::ok);
      }

      THEN("the crc in output crc_element is equal to the crc given in the input_buffer") {
        REQUIRE(crc_element.crc() == expected_crc);
      }

      THEN(
          "the payload in the output crc element is equal to the payload given in the "
          "input_buffer") {
        REQUIRE(payload == expected_payload);
      }
    }

    WHEN("body with 0 bytes is given to the crc element receiver") {
      TestCRCElementProps::PayloadBuffer input_payload;
      TestCRCElement crc_element{input_payload};

      PF::Util::ByteVector<buffer_size> input_buffer;

      auto transform_status = crc_element_receiver.transform(input_buffer, crc_element);

      uint32_t expected_crc = 0x00000000;

      THEN("the transform status is equal to invalid parse") {
        REQUIRE(transform_status == TestCRCElementReceiver::Status::invalid_parse);
      }

      THEN("The crc from crc element is equal to 0") { REQUIRE(crc_element.crc() == expected_crc); }
    }

    WHEN(
        "The CRC stored in the input body is inconsistent with the payload stored in the input "
        "body") {
      auto body = std::string("\x12\x34\x56\x78\x01\x05\x01\x02\x03\x04\x05", 11);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      uint32_t expected_crc = 0x12345678;

      auto transform_status = crc_element_receiver.transform(input_buffer, crc_element);

      THEN("the transform status is equal to invalid crc") {
        REQUIRE(transform_status == TestCRCElementReceiver::Status::invalid_crc);
      }

      THEN("the crc in output crc_element is equal to the invalid crc given in input buffer") {
        REQUIRE(crc_element.crc() == expected_crc);
      }
    }
  }
}

SCENARIO(
    "Protocols::CRCElementSender: The crc element receiver correctly generates datagrams from "
    "payload",
    "[CRCElementSender]") {
  GIVEN("A CRC element sender of buffer size 254 bytes") {
    constexpr size_t buffer_size = 254UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElementSender = PF::Protocols::CRCElementSender<buffer_size>;

    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    TestCRCElementSender crc_element_sender{crc32c};

    WHEN("A payload is given along with a sufficiently large output buffer") {
      auto body = std::string("\x13\x03\x05\x06\x23", 5);

      // Calculated using the Sunshine Online CRC Calculator
      uint32_t expected_crc = 0x81FC3457;
      auto expected_output = std::string("\x81\xfc\x34\x57\x13\x03\x05\x06\x23", 9);

      TestCRCElementProps::PayloadBuffer input_payload;
      PF::Util::convertStringToByteVector(body, input_payload);

      PF::Util::ByteVector<buffer_size> output_buffer;

      auto transform_status = crc_element_sender.transform(input_payload, output_buffer);

      THEN("the transform status is ok") {
        REQUIRE(transform_status == TestCRCElementSender::Status::ok);
      }
      THEN("output buffer is as expected, equal to (crc + input_buffer)") {
        REQUIRE(output_buffer == expected_output);
      }
    }
  }

  GIVEN("A CRC element sender of buffer size 20 bytes") {
    constexpr size_t buffer_size = 20UL;
    using TestCRCElementProps = PF::Protocols::CRCElementProps<buffer_size>;
    using TestCRCElementSender = PF::Protocols::CRCElementSender<buffer_size>;

    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    TestCRCElementSender crc_element_sender{crc32c};

    WHEN("The output buffer is not large enough for the input payload") {
      auto body = std::string("\x81\xfc\x34\x57\x13\x03\x05\x06\x23\x01\x09", 11);
      constexpr size_t output_buffer_size = 10UL;

      TestCRCElementProps::PayloadBuffer input_payload;
      for (auto& data : body) {
        input_payload.push_back(data);
      }

      PF::Util::ByteVector<output_buffer_size> output_buffer;

      auto transform_status = crc_element_sender.transform(input_payload, output_buffer);

      THEN("the transform status is equal to invalid_length") {
        REQUIRE(transform_status == TestCRCElementSender::Status::invalid_length);
      }
    }
  }
}
