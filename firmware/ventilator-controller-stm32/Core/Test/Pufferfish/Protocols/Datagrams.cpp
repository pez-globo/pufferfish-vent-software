/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Datagrams.cpp
 *
 *  Created on: Nov 3, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of Datagrams
 *
 */

#include "Pufferfish/Protocols/Datagrams.h"

#include <iostream>

#include "Pufferfish/Test/Util.h"
#include "Pufferfish/Util/Array.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO(
    "Protocols::The write function correctly calculates length of internal payload and writes body "
    "with sequence, length and payload",
    "[Datagram]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
  using TestDatagram = PF::Protocols::ConstructedDatagram<buffer_size>;

  PF::Util::ByteVector<buffer_size> output_buffer;
  TestDatagramProps::PayloadBuffer input_payload;

  GIVEN("A Datagram constructed with an empty paylaod and sequence equal to 0") {
    TestDatagram datagram(input_payload);

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 0);
      }
      THEN("The payload returned by the payload accessor method remains unchanged") {
        auto buffer = datagram.payload();
        REQUIRE(buffer.empty() == true);
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(output_buffer.operator[](0) == 0x00);
      }
      THEN(
          "The length field of the body's header matches the value returned by the length accessor "
          "method") {
        REQUIRE(output_buffer.operator[](1) == 0x00);
      }
      THEN("The body's payload section is empty") {
        REQUIRE(output_buffer.operator[](2) == 0);
        REQUIRE(output_buffer.operator[](3) == 0);
      }
      THEN("The output buffer is as expected '0x00 0x00' ") {
        auto expected = std::string("\x00\x00", 2);
      }
    }
  }

  GIVEN("A Datagram constructed with a non-empty payload buffer and sequence equal to 0") {
    // as payload buffer is a ByteVector of size 252
    int payload_size = GENERATE(0, 252);

    for (size_t i = 0; i < payload_size; ++i) {
      uint8_t val = 9;
      input_payload.push_back(val);
    }
    TestDatagram datagram{input_payload};
    REQUIRE(datagram.seq() == 0);
    REQUIRE(datagram.length() == payload_size);

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      auto write_status = datagram.write(output_buffer);

      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == payload_size);
      }
      THEN("The payload returned by the paylaod accessor method remains unchanged") {
        auto buffer = datagram.payload();
        for (size_t i = 0; i < payload_size; ++i) {
          uint8_t val = 9;
          REQUIRE(buffer.operator[](i) == val);
        }
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(output_buffer.operator[](0) == datagram.seq());
      }
      THEN(
          "The length field of the body's header matches the value returned by the length accessor "
          "method") {
        REQUIRE(output_buffer.operator[](1) == datagram.length());
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0xb7 0xe1 0x8d 0xaa 0x4d'") {
        for (size_t i = 2; i < payload_size + 1; ++i) {
          uint8_t val = 9;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
      THEN("The output buffer is as expected") {
        REQUIRE(output_buffer.operator[](0) == 0);
        REQUIRE(output_buffer.operator[](1) == payload_size);
        for (size_t i = 2; i < payload_size + 1; ++i) {
          uint8_t val = 9;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
    }
  }

  GIVEN("A Datagram constructed with a non-empty payload buffer and sequence equal to 0") {
    auto data = std::string("\xb7\xe1\x8d\xaa\x4d", 5);
    PF::Util::convert_string_to_byte_vector(data, input_payload);

    TestDatagram datagram{input_payload};

    REQUIRE(datagram.seq() == 0);
    REQUIRE(datagram.length() == 5);
    REQUIRE(datagram.payload() == data);

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      auto write_status = datagram.write(output_buffer);

      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN("The payload returned by the paylaod accessor method remains unchanged") {
        REQUIRE(datagram.payload() == data);
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(output_buffer.operator[](0) == datagram.seq());
      }
      THEN(
          "The length field of the body's header matches the value returned by the length accessor "
          "method") {
        REQUIRE(output_buffer.operator[](1) == datagram.length());
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0xb7 0xe1 0x8d 0xaa 0x4d'") {
        for (size_t i = 2; i < 7; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0xb7, 0xe1, 0x8d, 0xaa, 0x4d);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected '0x00 0x05 0xb7 0xe1 0x8d 0xaa 0x4d' ") {
        auto expected_buffer = std::string("\x00\x05\xb7\xe1\x8d\xaa\x4d", 7);
        REQUIRE(output_buffer == expected_buffer);
      }
    }

    WHEN(
        "The paylaod given to the constructor is altered ('0x01 0x02 0x03 0x04 0x05 0x02') before "
        "write method is called") {
      input_payload.push_back(0x02);

      THEN(
          "Before the write method is called, The value returned by the length accessor method is "
          "inconsistent with the size of the altered paylaod") {
        REQUIRE(datagram.length() == 5);
      }

      auto write_status = datagram.write(output_buffer);
      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the length accessor method returns a value equal to the size of the altered payload") {
        REQUIRE(datagram.length() == 6);
      }
      auto expected = std::string("\xb7\xe1\x8d\xaa\x4d\x02", 6);
      THEN(
          "The paylaod returned by the paylaod accessor method is same as the altered paylaod "
          "buffer") {
        REQUIRE(datagram.payload() == expected);
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(output_buffer.operator[](0) == datagram.seq());
      }
      THEN(
          "The length field of the body's header matches the value returned by the length accessor "
          "method") {
        REQUIRE(output_buffer.operator[](1) == datagram.length());
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0xb7 0xe1 0x8d 0xaa 0x4d "
          "0x02'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0xb7, 0xe1, 0x8d, 0xaa, 0x4d, 0x02);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected '0x00 0x06 0xb7 0xe1 0x8d 0xaa 0x4d 0x02'") {
        auto expected_buffer = std::string("\x00\x06\xb7\xe1\x8d\xaa\x4d\x02", 8);
        REQUIRE(output_buffer == expected_buffer);
      }
    }

    WHEN("The output buffer is too short and cannot hold the sequence, length and paylaod fields") {
      constexpr size_t output_buffer_size = 2UL;
      PF::Util::ByteVector<output_buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The write method reports out of bounds status") {
        REQUIRE(write_status == PF::IndexStatus::out_of_bounds);
      }
      THEN(
          "After the write method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 5);
      }
      THEN("The payload returned by the paylaod accessor method remains unchanged") {
        auto expected = std::string("\xb7\xe1\x8d\xaa\x4d", 5);
        REQUIRE(datagram.payload() == expected);
      }
      THEN("The output buffer is as expected") { REQUIRE(output_buffer.empty() == true); }
    }
  }

  GIVEN("A Datagram constructed with a non-empty paylaod buffer and non-zero sequence") {
    auto data = std::string("\xc2\x22\x10\xa6\x3a\xa5", 6);
    PF::Util::convert_string_to_byte_vector(data, input_payload);

    uint8_t seq = 10;
    TestDatagram datagram{input_payload, seq};

    REQUIRE(datagram.seq() == 10);
    REQUIRE(datagram.length() == 6);
    REQUIRE(datagram.payload() == data);

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);

      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 10);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN("The payload returned by the paylaod accessor method remains unchanged") {
        REQUIRE(datagram.payload() == data);
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(output_buffer.operator[](0) == datagram.seq());
      }
      THEN(
          "The length field of the body's header matches the value returned by the length accessor "
          "method") {
        REQUIRE(output_buffer.operator[](1) == datagram.length());
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0xc2 0x22 0x10 0xa6 0x3a "
          "0xa5'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0xc2, 0x22, 0x10, 0xa6, 0x3a, 0xa5);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected '0x0a 0x06 0xc2 0x22 0x10 0xa6 0x3a 0xa5' ") {
        auto expected_buffer = std::string("\x0A\x06\xc2\x22\x10\xa6\x3a\xa5", 8);
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }

  GIVEN(
      "A Datagram constructed with a non-empty paylaod buffer and a sequence ranging from 0 to "
      "0xff") {
    auto data = std::string("\x01\x23\x45\x0a\x4d\x04\x05", 7);
    PF::Util::convert_string_to_byte_vector(data, input_payload);

    uint8_t sequence = GENERATE(range(0, 256));

    TestDatagram datagram{input_payload, sequence};

    REQUIRE(datagram.seq() == sequence);
    REQUIRE(datagram.length() == 7);
    REQUIRE(datagram.payload() == data);

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == sequence);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN("The payload returned by the paylaod accessor method remains unchanged") {
        REQUIRE(datagram.payload() == data);
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(output_buffer.operator[](0) == datagram.seq());
      }
      THEN(
          "The length field of the body's header matches the value returned by the length accessor "
          "method") {
        REQUIRE(output_buffer.operator[](1) == datagram.length());
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0x01 0x23 0x45 0x0a 0x4d "
          "0x04 0x05'") {
        for (size_t i = 2; i < 9; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x01, 0x23, 0x45, 0x0a, 0x4d, 0x04, 0x05);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN(
          "The output buffer is as expected '(0x00-0xff) 0x07 0x01 0x23 0x45 0x0a 0x4d 0x04 "
          "0x05' ") {
        REQUIRE(output_buffer.operator[](0) == sequence);
        REQUIRE(output_buffer.operator[](1) == 7);
        for (size_t i = 2; i < 9; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x01, 0x23, 0x45, 0x0a, 0x4d, 0x04, 0x05);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
    }
  }
}

SCENARIO(
    "Protocols:: The parse function correctly updates internal length member and constructor's "
    "paylaod buffer and sequence fields from the input_buffer",
    "[Datagram]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
  using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

  TestDatagramProps::PayloadBuffer payload;
  PF::Util::ByteVector<buffer_size> input_buffer;

  GIVEN("A Datagram constructed with an empty paylaod and sequence equal to 0") {
    uint8_t seq = 0;
    TestDatagram datagram{payload, seq};

    REQUIRE(datagram.seq() == 0);
    REQUIRE(datagram.length() == 0);
    auto payload_buffer = datagram.payload();
    REQUIRE(payload_buffer.empty() == true);

    WHEN("An empty input_buffer body is parsed") {
      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse method reports out of bounds status") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 0);
      }
      THEN("The paylaod returned by the paylaod accessor method remains unchanged") {
        REQUIRE(payload_buffer.empty() == true);
      }
      THEN("The constructor's payload buffer remains unchanged") {
        REQUIRE(payload.empty() == true);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, and a non-empty paylaod buffer consistent with the "
        "length field of the header is parsed") {
      auto body = std::string("\x01\x05\xb5\x83\x6d\xf6\x1c\xf0\xb1\x58", 10);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.length() == 5);
      }
      auto expected_payload = std::string("\xb5\x83\x6d\xf6\x1c\xf0\xb1\x58", 8);
      THEN(
          "The payload returned from the payload accessor method is equal to the payload from the "
          "body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x01);
        auto expected_payload = std::string("\xb5\x83\x6d\xf6\x1c\xf0\xb1\x58\x01", 9);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }
  }

  GIVEN("A Datagram constructed with paylaod a non-empty paylaod buffer and sequence equal to 0") {
    auto data = std::string("\x7e\x9b\x20\x1b\xb9", 5);
    PF::Util::convert_string_to_byte_vector(data, payload);
    TestDatagram datagram{payload};

    REQUIRE(datagram.seq() == 0);
    REQUIRE(datagram.length() == 5);
    REQUIRE(datagram.payload() == data);

    WHEN("An empty input_buffer body is parsed") {
      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports out of bounds status") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 5);
      }
      THEN("The paylaod returned by the paylaod accessor method remains unchanged") {
        REQUIRE(datagram.payload() == data);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x02);
        auto expected = std::string("\x7e\x9b\x20\x1b\xb9\x02", 6);
        REQUIRE(datagram.payload() == expected);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == data);
      }
      THEN("The constructor's payload buffer remains unchanged") {
        REQUIRE(datagram.payload() == data);
      }
    }

    WHEN("A body with an incomplete 2-byte header and an empty payload buffer is parsed") {
      input_buffer.push_back(0x01);
      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports out of bounds status") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method remains "
          "unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 5);
      }
      THEN("The paylaod returned by the paylaod accessor method remains unchanged") {
        REQUIRE(datagram.payload() == data);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x02);
        auto expected = std::string("\x7e\x9b\x20\x1b\xb9\x02", 6);
        REQUIRE(datagram.payload() == expected);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == data);
      }
      THEN("The constructor's payload buffer remains unchanged") {
        REQUIRE(datagram.payload() == data);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, and a non-empty paylaod buffer consistent with the "
        "length field of the header is parsed") {
      auto body = std::string("\x01\x05\xaa\x10\x8d\xf1\x05\x2c\x67\x0d\xa2\x1a", 12);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.length() == 5);
      }
      auto expected_payload = std::string("\xaa\x10\x8d\xf1\x05\x2c\x67\x0d\xa2\x1a", 10);
      THEN(
          "The payload returned from the payload accessor method is equal to the payload from the "
          "input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x01);
        auto expected_payload = std::string("\xaa\x10\x8d\xf1\x05\x2c\x67\x0d\xa2\x1a\x01", 11);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, and a non empty payload buffer consistent with the "
        "length field of the header, is altered after it's parsed") {
      auto body = std::string("\x01\x05\x11\x12\x13\x14\x15", 7);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      auto expected_payload = std::string("\x11\x12\x13\x14\x15", 5);
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the original input_buffer body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "original input_buffer body's header") {
        REQUIRE(datagram.length() == 5);
      }
      THEN(
          "The payload buffer returned from the payload accessor method is equal to the original "
          "payload from the body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x01);
        auto expected_payload = std::string("\x11\x12\x13\x14\x15\x01", 6);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == expected_payload);
      }

      input_buffer.push_back(0x16);

      THEN(
          "After the input buffer is changed, The payload buffer returned from the payload "
          "accessor method is unchanged") {
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, and paylaod buffer as '0x00' consistent with the "
        "length field of the header is parsed") {
      auto input_data = PF::Util::make_array<uint8_t>(0x00, 0x01, 0x00);
      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.length() == 1);
      }
      auto expected_payload = std::string("\x00", 1);
      THEN(
          "The payload returned from the payload accessor method is equal to the payload from the "
          "input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x01);
        auto expected_payload = std::string("\x00\x01", 2);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "The parse method is called on a body with a complete 2-byte header, and a non-empty "
        "payload buffer where the length field of the header is inconsistent with the actual "
        "length of the payload buffer") {
      auto body = std::string("\x00\x05\x4b\xb6\x08\x37\x4f\xf9", 8);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.length() == 5);
      }
      auto expected_payload = std::string("\x4B\xB6\x08\x37\x4F\xF9", 6);
      THEN(
          "The value returned by the length accessor method is not equal to the size of the "
          "payload buffer in the input_buffer body") {
        REQUIRE(datagram.length() != expected_payload.size());
      }
      THEN(
          "The payload returned from the payload accessor method is equal to the payload from the "
          "input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x01);
        auto expected_payload = std::string("\x4B\xB6\x08\x37\x4F\xF9\x01", 7);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "The parse method is called on a body with sequence field of the header inconsistent with "
        "sequence given in the constructor of the datagram") {
      auto body = PF::Util::make_array<uint8_t>(0x05, 0x01, 0x01);

      for (auto& data : body) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 5);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.length() == 1);
      }
      auto expected_payload = std::string("\x01", 1);
      THEN(
          "The payload returned from the payload accessor method is equal to the payload from the "
          "input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload accessor method returns a reference to the payload buffer given in the "
          "Datagram constructor") {
        payload.push_back(0x01);
        auto expected_payload = std::string("\x01\x01", 2);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "the payload given in the Datagram constructor is independent of the input buffer body") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN("an input buffer with invalid payload is written") {
      // This cannot be tested as any payload, valid or invalid,
      // will always be returned as-is.
    }
  }
}

SCENARIO(
    "Protocols: Datagram correctly preserves data in roundtrip writing and parsing", "[Datagram]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
  using TestConstructedDatagram = PF::Protocols::ConstructedDatagram<buffer_size>;
  using TestParsedDatagram = PF::Protocols::ParsedDatagram<buffer_size>;

  PF::Util::ByteVector<buffer_size> output_buffer;
  GIVEN("A datagram constructed with an non-empty payload buffer and a non-zero sequence number") {
    TestDatagramProps::PayloadBuffer input_payload;
    auto body = std::string("\x61\x6a\x1a\x6a\x29\xcf\x01\x81\xbe\x9d", 10);
    PF::Util::convert_string_to_byte_vector(body, input_payload);
    TestConstructedDatagram datagram(input_payload, 1);
    TestParsedDatagram parsed_datagram(input_payload);

    WHEN(
        "A body with seq, length and payload is generated, parsed into a new ParsedDatagram "
        "object, and generated again with a ConstructedDatagram object") {
      PF::Util::ByteVector<buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The first write method reports ok status") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }
      THEN(
          "After the first write method is called, The value returned by the seq accessor method "
          "is "
          "equal to 0") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "the length accessor method returns a value equal to the size of the payload given in "
          "the constructor") {
        REQUIRE(datagram.length() == 10);
      }
      THEN(
          "The buffer returned by the paylaod accessor method is same as the paylaod buffer given "
          "in the constructor") {
        auto expected_payload = std::string("\x61\x6a\x1a\x6a\x29\xcf\x01\x81\xbe\x9d", 10);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(output_buffer.operator[](0) == datagram.seq());
      }
      THEN(
          "The length field of the body's header is equal to the size of the payload given in the "
          "constructor") {
        REQUIRE(output_buffer.operator[](1) == datagram.length());
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0x61 0x6a 0x1a 0x6a 0x29 "
          "0xcf 0x01 0x81 0xbe 0x9d'") {
        for (size_t i = 2; i < 10; ++i) {
          auto data = PF::Util::make_array<uint8_t>(
              0x61, 0x6a, 0x1a, 0x6a, 0x29, 0xcf, 0x01, 0x81, 0xbe, 0x9d);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected") {
        auto expected_output = std::string("\x01\x0a\x61\x6a\x1a\x6a\x29\xcf\x01\x81\xbe\x9d", 12);
        REQUIRE(output_buffer == expected_output);
      }

      // Parse
      auto body = std::string("\x01\x0a\xca\x9b\xc7\xf9\x5c\x10\xa1\x77\x23\x82", 12);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);
      auto parse_status = parsed_datagram.parse(input_buffer);

      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input body's header") {
        REQUIRE(parsed_datagram.seq() == 1);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input body's header") {
        REQUIRE(parsed_datagram.length() == 10);
      }
      THEN(
          "The paylaod buffer returned by the paylaod accessor method is equal to the payload from "
          "the body") {
        auto expected_payload = std::string("\xca\x9b\xc7\xf9\x5c\x10\xa1\x77\x23\x82", 10);
        REQUIRE(parsed_datagram.payload() == expected_payload);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == body); }

      // Write
      auto final_data = std::string("\x56\xd6\x42\xc5\xe1\xf0\x30\xe5\xc8\x4d", 10);
      TestDatagramProps::PayloadBuffer input_payload;
      PF::Util::convert_string_to_byte_vector(final_data, input_payload);
      TestConstructedDatagram write_datagram(input_payload);

      PF::Util::ByteVector<buffer_size> final_buffer;
      auto final_status = write_datagram.write(final_buffer);
      THEN("The second write method reports ok status") {
        REQUIRE(final_status == PF::IndexStatus::ok);
      }
      THEN(
          "After the write method is called, The value returned by the seq accessor method is "
          "equal to 0") {
        REQUIRE(write_datagram.seq() == 0);
      }
      THEN(
          "the length accessor method returns a value equal to the size of the payload given in "
          "the constructor") {
        REQUIRE(write_datagram.length() == 10);
      }
      THEN(
          "The buffer returned by the paylaod accessor method is same as the paylaod buffer given "
          "in the constructor") {
        auto expected_payload = std::string("\x56\xd6\x42\xc5\xe1\xf0\x30\xe5\xc8\x4d", 10);
        REQUIRE(write_datagram.payload() == expected_payload);
      }
      THEN(
          "The seq field of the body's header matches the value returned by the seq accessor "
          "method") {
        REQUIRE(final_buffer.operator[](0) == write_datagram.seq());
      }
      THEN(
          "The length field of the body's header is equal to the size of the payload given in the "
          "constructor") {
        REQUIRE(final_buffer.operator[](1) == write_datagram.length());
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0x56 0xd6 0x42 0xc5 0xe1 "
          "0xf0 0x30 0xe5 0xc8 0x4d'") {
        for (size_t i = 2; i < 10; ++i) {
          auto data = PF::Util::make_array<uint8_t>(
              0x56, 0xd6, 0x42, 0xc5, 0xe1, 0xf0, 0x30, 0xe5, 0xc8, 0x4d);
          REQUIRE(final_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected") {
        auto expected_output = std::string("\x00\x0a\x56\xd6\x42\xc5\xe1\xf0\x30\xe5\xc8\x4d", 12);
        REQUIRE(final_buffer == expected_output);
      }
    }
  }
}

SCENARIO(
    "Protocols::Datagram Receiver correctly parses datagram bodies and performs consistency "
    "checking on them",
    "[DatagramReceiver]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
  using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
  using TestDatagramReceiver = PF::Protocols::DatagramReceiver<buffer_size>;

  TestDatagramProps::PayloadBuffer payload;
  TestDatagramReceiver datagram_receiver{};
  PF::Util::ByteVector<buffer_size> input_buffer;

  GIVEN(
      "A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 0, with "
      "output_datagram constructed with empty payload buffer") {
    TestDatagram datagram{payload};

    REQUIRE(datagram.seq() == 0);
    REQUIRE(datagram.length() == 0);
    auto payload_buffer = datagram.payload();
    REQUIRE(payload_buffer.empty() == true);

    WHEN("An empty input_buffer body is parsed") {
      auto transform_status = datagram_receiver.transform(input_buffer, datagram);
      THEN("The transform method reports invalid parse status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_parse);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method remains unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the output_datagram's length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 0);
      }
      THEN(
          "the paylaod returned by the output_datagram's paylaod accessor method remains "
          "unchanged") {
        REQUIRE(payload_buffer.empty() == true);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        payload.push_back(0x01);
        auto expected = std::string("\x01", 1);
        REQUIRE(datagram.payload() == expected);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x01);
        REQUIRE(payload_buffer.empty() == true);
      }
      THEN("the constructor's payload buffer remains unchanged") {
        REQUIRE(payload_buffer.empty() == true);
      }
    }

    WHEN("A body with an incomplete 2-byte header and empty payload is given to the receiver") {
      input_buffer.push_back(0x00);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid_parse status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_parse);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method remains unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the output_datagram's length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 0);
      }
      THEN(
          "the paylaod returned by the output_datagram's paylaod accessor method remains "
          "unchanged") {
        auto payload = datagram.payload();
        REQUIRE(payload.empty() == true);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        payload.push_back(0x01);
        auto expected = std::string("\x01", 1);
        REQUIRE(datagram.payload() == expected);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x01);
        auto payload = datagram.payload();
        REQUIRE(payload.empty() == true);
      }
      THEN("the constructor's payload buffer remains unchanged") {
        REQUIRE(payload.empty() == true);
      }
    }

    WHEN(
        "transform is called on a body with a complete 2-byte header, and a non-empty paylaod "
        "buffer inconsistent with the length field of the header") {
      auto body = std::string("\x00\x05\xaa\xd1\x64\xa8\x85\xf4", 8);

      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid length status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_length);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the output_datagram's length accessor method is equal to the "
          "length field of the input_buffer body's header") {
        REQUIRE(datagram.length() == 5);
      }
      auto expected_payload = std::string("\xaa\xd1\x64\xa8\x85\xf4", 6);
      THEN(
          "The value returned by the output_datagram's length accessor method is inconsistent with "
          "the actual length of the payload buffer from the body") {
        REQUIRE(datagram.length() != expected_payload.length());
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        auto expected_payload = std::string("\xaa\xd1\x64\xa8\x85\xf4\x02", 7);
        payload.push_back(0x02);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x04);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "transform is called on a body with a non-empty paylaod buffer and a complete 2-byte "
        "header where the sequence field of the header does not match sequence given in "
        "output_datagram's constructor") {
      auto body = std::string("\x04\x06\x5f\xee\x40\xeb\x41\x6e", 8);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 4);
      }
      THEN(
          "The value returned by the output_datagram's length accessor method is equal to the "
          "length field of the input_buffer body's header") {
        REQUIRE(datagram.length() == 6);
      }
      auto expected_payload = std::string("\x5f\xee\x40\xeb\x41\x6e", 6);
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        payload.push_back(0x02);
        auto expected_payload = std::string("\x5f\xee\x40\xeb\x41\x6e\x02", 7);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x04);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, a non-empty payload buffer consistent with the "
        "length field of the header is given to the receiver") {
      auto body = std::string("\x00\x05\xec\x1f\xa9\x9a\x75", 7);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the output_datagram's length accessor method is equal to the "
          "length field of the input_buffer body's header") {
        REQUIRE(datagram.length() == 5);
      }
      auto expected_payload = std::string("\xec\x1f\xa9\x9a\x75", 5);
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        payload.push_back(0x02);
        auto expected_payload = std::string("\xec\x1f\xa9\x9a\x75\x02", 6);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x04);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }
  }

  GIVEN(
      "A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 0, with "
      "output_datagram constructed with a non-empty payload buffer") {
    auto body = std::string("\x51\xb0\x6e\xf7\x86\x71\xcd\x00", 8);
    PF::Util::convert_string_to_byte_vector(body, payload);

    TestDatagram datagram{payload};
    REQUIRE(datagram.seq() == 0);
    REQUIRE(datagram.length() == 8);
    REQUIRE(datagram.payload() == body);

    WHEN("A body with an incomplete 2-byte header and empty payload is given to the receiver") {
      input_buffer.push_back(0x00);
      auto transform_status = datagram_receiver.transform(input_buffer, datagram);
      THEN("The transform method reports invalid_parse status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_parse);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method remains unchanged") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("the value returned by the output_datagram's length accessor method remains unchanged") {
        REQUIRE(datagram.length() == 8);
      }
      THEN(
          "the paylaod returned by the output_datagram's paylaod accessor method remains "
          "unchanged") {
        REQUIRE(datagram.payload() == body);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        payload.push_back(0x02);
        auto expected_payload = std::string("\x51\xb0\x6e\xf7\x86\x71\xcd\x00\x02", 9);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x02);
        REQUIRE(datagram.payload() == body);
      }
      THEN("the constructor's payload buffer remains unchanged") {
        REQUIRE(datagram.payload() == body);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, and a non-empty paylaod buffer inconsistent with "
        "the length field of the header") {
      auto body = std::string("\x00\x05\xaa\xd1\x64\xa8\x85\xf4", 8);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);
      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid length status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_length);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the output_datagram's length accessor method is equal to the "
          "length field of the input_buffer body's header") {
        REQUIRE(datagram.length() == 5);
      }
      auto expected_payload = std::string("\xaa\xd1\x64\xa8\x85\xf4", 6);
      THEN(
          "The value returned by the output_datagram's length accessor method is inconsistent with "
          "the actual length of the payload buffer from the body") {
        REQUIRE(datagram.length() != expected_payload.length());
      }
    }

    WHEN(
        "transform is called on a body with a non-empty paylaod buffer and a complete 2-byte "
        "header where the sequence field of the header does not match sequence given in "
        "output_datagram's constructor") {
      auto body = std::string("\x04\x06\x5f\xee\x40\xeb\x41\x6e", 8);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);
      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 4);
      }
      THEN(
          "The value returned by the output_datagram's length accessor method is equal to the "
          "length field of the input_buffer body's header") {
        REQUIRE(datagram.length() == 6);
      }
      auto expected_payload = std::string("\x5f\xee\x40\xeb\x41\x6e", 6);
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        payload.push_back(0x02);
        auto expected_payload = std::string("\x5f\xee\x40\xeb\x41\x6e\x02", 7);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x04);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, a non-empty payload buffer consistent with the "
        "length field of the header is given to the receiver") {
      auto body = std::string("\x00\x0c\x23\xce\xb3\x32\xca\x33\xa0\x97\x17\x2a\x2b\x85", 14);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);
      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the transform method is called, the value returned by the output_datagram's seq "
          "accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the output_datagram's length accessor method is equal to the "
          "length field of the input_buffer body's header") {
        REQUIRE(datagram.length() == 12);
      }
      auto expected_payload = std::string("\x23\xce\xb3\x32\xca\x33\xa0\x97\x17\x2a\x2b\x85", 12);
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The output_datagram's payload accessor method returns a reference to the payload given "
          "in its constructor") {
        payload.push_back(0x02);
        auto expected_payload =
            std::string("\x23\xce\xb3\x32\xca\x33\xa0\x97\x17\x2a\x2b\x85\x02", 13);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The data in the output_datagram's payload buffer is independent of the data in "
          "input_buffer") {
        input_buffer.push_back(0x04);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }
  }
}

SCENARIO(
    "Protocols::Datagram Receiver correctly reports incrementing, rollover, and resetting of "
    "expected sequence number",
    "[DatagramReceiver]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
  using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
  using TestDatagramReceiver = PF::Protocols::DatagramReceiver<buffer_size>;
  TestDatagramProps::PayloadBuffer payload;
  TestDatagramReceiver datagram_receiver{};
  PF::Util::ByteVector<buffer_size> input_buffer;

  GIVEN("A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 1") {
    TestDatagram datagram{payload};

    auto body = std::string("\x00\x07\x48\x17\xb8\x67\x1c\x45\x28", 9);
    PF::Util::convert_string_to_byte_vector(body, input_buffer);

    auto expected_payload = std::string("\x48\x17\xb8\x67\x1c\x45\x28", 7);

    auto transform_status = datagram_receiver.transform(input_buffer, datagram);
    REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
    REQUIRE(datagram.seq() == 0);
    REQUIRE(datagram.payload() == expected_payload);
    REQUIRE(datagram.length() == expected_payload.length());

    WHEN("2 input buffers with a value of sequence in their headers as 1 and 2 respectively") {
      auto body = std::string("\x01\x04\xc8\x8d\xdf\x84", 6);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The first transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the first transform method is called, the value returned by the output_datagram's "
          "seq accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xc8\x8d\xdf\x84", 4);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto data = std::string("\x02\x05\x82\x93\xc8\x5f\x60", 7);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto final_transform = datagram_receiver.transform(buffer, datagram);
      THEN("The second transform method reports ok status") {
        REQUIRE(final_transform == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the second transform method is called, the value returned by the "
          "output_datagram's seq accessor method is equal to the sequence field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.seq() == 2);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\x82\x93\xc8\x5f\x60", 5);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "transform is called on two parseable datagram bodies as input_buffers, whose headers have "
        "sequence numbers as 2 & 3 respectively") {
      auto body = std::string("\x02\x06\xdc\x53\x54\xeb\x12\xbd", 8);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The first transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN(
          "After the first transform method is called, the value returned by the output_datagram's "
          "seq accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 2);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xdc\x53\x54\xeb\x12\xbd", 6);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto data = std::string("\x03\x07\x76\x36\xd3\x7b\xb4\x59\x20", 9);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto final_status = datagram_receiver.transform(buffer, datagram);
      THEN("The second transform method reports ok status") {
        REQUIRE(final_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the second transform method is called, the value returned by the "
          "output_datagram's seq accessor method is equal to the sequence field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.seq() == 3);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\x76\x36\xd3\x7b\xb4\x59\x20", 7);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "transform is called on two parseable datagram bodies as input_buffers, whose headers have "
        "sequence numbers as 0xff & 0 respectively") {
      auto body = std::string("\xff\x0a\x30\xbe\xbd\x5c\x64\xa9\xdc\xd2\xde\x4b", 12);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The first transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN(
          "After the first transform method is called, the value returned by the output_datagram's "
          "seq accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0xff);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\x30\xbe\xbd\x5c\x64\xa9\xdc\xd2\xde\x4b", 10);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto data = std::string("\x00\x09\xfc\x60\x67\x98\xa6\xf6\xac\xd3\x8e", 11);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto final_status = datagram_receiver.transform(buffer, datagram);
      THEN("The second transform method reports ok status") {
        REQUIRE(final_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the second transform method is called, the value returned by the "
          "output_datagram's seq accessor method is equal to the sequence field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xfc\x60\x67\x98\xa6\xf6\xac\xd3\x8e", 9);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "transform is called on three parseable datagram bodies as input_buffers, whose headers "
        "have sequence numbers as 1, 10 & 11 respectively") {
      auto body = std::string("\x01\x05\xf8\xa4\xdd\x84\x62", 7);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto first_transform = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(first_transform == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the first transform method is called, the value returned by the output_datagram's "
          "seq accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xf8\xa4\xdd\x84\x62", 5);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto data = std::string("\x0a\x05\xb5\xfb\xca\x82\xf2", 7);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto second_transform = datagram_receiver.transform(buffer, datagram);
      THEN("The second transform method reports invalid_sequence status") {
        REQUIRE(second_transform == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN(
          "After the second transform method is called, the value returned by the "
          "output_datagram's seq accessor method is equal to the sequence field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.seq() == 10);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xb5\xfb\xca\x82\xf2", 5);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto input_data = std::string("\x0b\x05\x8b\xf2\x59\x90\x48", 7);
      PF::Util::ByteVector<buffer_size> final_buffer;
      PF::Util::convert_string_to_byte_vector(input_data, final_buffer);

      auto final_transform = datagram_receiver.transform(final_buffer, datagram);
      THEN("The third transform method reports ok status") {
        REQUIRE(final_transform == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the third transform method is called, the value returned by the output_datagram's "
          "seq accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 11);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\x8b\xf2\x59\x90\x48", 5);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }
  }

  GIVEN("A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 0xff") {
    TestDatagram datagram{payload};
    auto body = std::string("\xfe\x06\xcd\x6a\xc2\x7f\xa1\x5b", 8);
    auto expected_payload = std::string("\xcd\x6a\xc2\x7f\xa1\x5b", 6);

    PF::Util::convert_string_to_byte_vector(body, input_buffer);

    auto transform_status = datagram_receiver.transform(input_buffer, datagram);
    REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
    REQUIRE(datagram.seq() == 0xfe);
    REQUIRE(datagram.payload() == expected_payload);
    REQUIRE(datagram.length() == expected_payload.length());

    WHEN(
        "transform is called on two parseable datagram bodies, whose headers have sequence numbers "
        "as 0xff & 0 respectively") {
      auto body = std::string("\xff\x06\xfa\x5b\x28\x1a\x4b\x9d", 8);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The first transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the first transform method is called, the value returned by the output_datagram's "
          "seq accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0xff);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xfa\x5b\x28\x1a\x4b\x9d", 6);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto input_data = std::string("\x00\x06\xab\xda\x26\x64\x47\x9f", 8);
      PF::Util::ByteVector<buffer_size> final_buffer;
      PF::Util::convert_string_to_byte_vector(input_data, final_buffer);

      auto final_transform = datagram_receiver.transform(final_buffer, datagram);
      THEN("The second transform method reports ok status") {
        REQUIRE(final_transform == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the second transform method is called, the value returned by the "
          "output_datagram's seq accessor method is equal to the sequence field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xab\xda\x26\x64\x47\x9f", 6);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "transform is called on two parseable datagram bodies, whose headers have sequence numbers "
        "as 0xff & 1 respectively") {
      auto body = std::string("\xff\x06\xdc\x48\x24\x93\xab\xbc", 8);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The first transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the first transform method is called, the value returned by the output_datagram's "
          "seq accessor method is equal to the sequence field of the input_buffer body's header") {
        REQUIRE(datagram.seq() == 0xff);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\xdc\x48\x24\x93\xab\xbc", 6);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto input_data = std::string("\x01\x06\x43\xb9\x66\x28\xdf\x3d", 8);
      PF::Util::ByteVector<buffer_size> final_buffer;
      PF::Util::convert_string_to_byte_vector(input_data, final_buffer);

      auto final_transform = datagram_receiver.transform(final_buffer, datagram);
      THEN("The second transform method reports invalid_sequence status") {
        REQUIRE(final_transform == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN(
          "After the second transform method is called, the value returned by the "
          "output_datagram's seq accessor method is equal to the sequence field of the "
          "input_buffer body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The paylaod returned by the output_datagram's paylaod accessor method is equal to the "
          "paylaod from the input_buffer body") {
        auto expected_payload = std::string("\x43\xb9\x66\x28\xdf\x3d", 6);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }
  }
}

SCENARIO(
    "Protocols::Datagram Sender correctly generates datagram bodies from payloads",
    "[DatagramSender]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramSender = PF::Protocols::DatagramSender<buffer_size>;
  TestDatagramSender datagram_sender{};

  GIVEN("A Datagram sender of buffer size 254 bytes with next sequence equal to 0") {
    WHEN("A non empty payload buffer is given to the datagram sender") {
      using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
      TestDatagramProps::PayloadBuffer payload;

      auto data = std::string("\xf9\x23\x4a\xd4\xe0", 5);

      PF::Util::convert_string_to_byte_vector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;
      auto transform_status = datagram_sender.transform(payload, output_datagram);

      THEN("The transform method reports ok") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The seq field of the body's header is equal to 0x00") {
        REQUIRE(output_datagram.operator[](0) == 0);
      }
      THEN("The length field of the body's header is equal to the size of the payload given") {
        REQUIRE(output_datagram.operator[](1) == 5);
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0xf9 0x23 0x4a 0xd4 0xe0'") {
        for (size_t i = 2; i < 7; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0xf9, 0x23, 0x4a, 0xd4, 0xe0);
          REQUIRE(output_datagram.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output datagram is as expected '0x00 0x05 0xf9 0x23 0x4a 0xd4 0xe0'") {
        auto expected_datagram = std::string("\x00\x05\xf9\x23\x4a\xd4\xe0", 7);
        REQUIRE(output_datagram == expected_datagram);
      }
    }

    WHEN("The output datagram cannot hold enough data") {
      using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
      using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

      auto data = std::string("\x01\x02\x03\x04\x05\x06\x07\x08\x09", 9);

      TestDatagramProps::PayloadBuffer payload;
      PF::Util::convert_string_to_byte_vector(data, payload);

      constexpr size_t datagram_buffer_size = 10UL;
      PF::Util::ByteVector<datagram_buffer_size> output_datagram;

      auto transform_status = datagram_sender.transform(payload, output_datagram);

      THEN("The transform method reports invalid length") {
        REQUIRE(transform_status == TestDatagramSender::Status::invalid_length);
      }
      THEN("The output datagram remains unchanged") { REQUIRE(output_datagram.empty() == true); }
    }

    WHEN("The input payload to the transform method is '0x00 0x00'") {
      using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
      using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

      auto data = std::string("\x00\x00", 2);

      TestDatagramProps::PayloadBuffer payload;
      PF::Util::convert_string_to_byte_vector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;

      auto transform_status = datagram_sender.transform(payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The output datagram is as expected '0x00 0x02 0x00 0x00' ") {
        auto expected = std::string("\x00\x02\x00\x00", 4);
        REQUIRE(output_datagram == expected);
      }
    }
  }
}

SCENARIO(
    "Protocols:: Datagram Sender correctly reports incrementing and rollover of expected sequence "
    "number",
    "[DatagramSender]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
  using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
  using TestDatagramSender = PF::Protocols::DatagramSender<buffer_size>;
  TestDatagramSender datagram_sender{};

  GIVEN("A Datagram sender of buffer size 254 bytes with next sequence equal to 1") {
    auto data = std::string("\x88\xf3\x52\xec\x5c\x31", 6);
    TestDatagramProps::PayloadBuffer payload;
    PF::Util::convert_string_to_byte_vector(data, payload);

    PF::Util::ByteVector<buffer_size> output_datagram;
    auto transform_status = datagram_sender.transform(payload, output_datagram);
    REQUIRE(transform_status == TestDatagramSender::Status::ok);

    WHEN("2 non-empty payloads of capacity 254 bytes is given to the sender") {
      auto transform_status = datagram_sender.transform(payload, output_datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The seq field of the body's header is equal to 0x00") {
        REQUIRE(output_datagram.operator[](0) == 1);
      }
      THEN("The length field of the body's header is equal to the size of the payload given") {
        REQUIRE(output_datagram.operator[](1) == 6);
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0x88 0xf3 0x52 0xec 0x5c "
          "0x31'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x88, 0xf3, 0x52, 0xec, 0x5c, 0x31);
          REQUIRE(output_datagram.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output datagram is as expected '0x01 0x06 0x88 0xf3 0x52 0xec 0x5c 0x31 ' ") {
        auto expected = std::string("\x01\x06\x88\xf3\x52\xec\x5c\x31", 8);
        REQUIRE(output_datagram == expected);
      }

      auto input_data = std::string("\x1d\xf5\x2a\xbc\x97\x2c", 6);

      TestDatagramProps::PayloadBuffer input_payload;
      PF::Util::convert_string_to_byte_vector(input_data, input_payload);

      auto final_status = datagram_sender.transform(input_payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_status == TestDatagramSender::Status::ok);
      }
      THEN("The seq field of the body's header is equal to 0x01") {
        REQUIRE(output_datagram.operator[](0) == 2);
      }
      THEN("The length field of the body's header is equal to the size of the payload given") {
        REQUIRE(output_datagram.operator[](1) == 6);
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0x1d 0xf5 0x2a 0xbc 0x97 "
          "0x2c'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x1d, 0xf5, 0x2a, 0xbc, 0x97, 0x2c);
          REQUIRE(output_datagram.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output datagram is as expected '0x02 0x06 0x1d 0xf5 0x2a 0xbc 0x97 0x2c' ") {
        auto expected = std::string("\x02\x06\x1d\xf5\x2a\xbc\x97\x2c", 8);
        REQUIRE(output_datagram == expected);
      }
    }
  }

  GIVEN("A Datagram sender of buffer size 254 bytes with next sequence equal to 0") {
    WHEN("2 non-empty payloads of capacity 254 bytes is given to the sender") {
      auto data = std::string("\xc0\x18\x65\xd1\x03\x5c", 6);

      TestDatagramProps::PayloadBuffer payload;
      PF::Util::convert_string_to_byte_vector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;

      auto transform_status = datagram_sender.transform(payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The seq field of the body's header is equal to 0x00") {
        REQUIRE(output_datagram.operator[](0) == 0);
      }
      THEN("The length field of the body's header is equal to the size of the payload given") {
        REQUIRE(output_datagram.operator[](1) == 6);
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0xc0 0x18 0x65 0xd1 0x03 "
          "0x5c'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0xc0, 0x18, 0x65, 0xd1, 0x03, 0x5c);
          REQUIRE(output_datagram.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output datagram is as expected '0x00 0x06 0xc0 0x18 0x65 0xd1 0x03 0x5c' ") {
        auto expected = std::string("\x00\x06\xc0\x18\x65\xd1\x03\x5c", 8);
        REQUIRE(output_datagram == expected);
      }

      auto input_data = std::string("\x6b\x05\xb9\xf3\xe5\xb6", 6);

      TestDatagramProps::PayloadBuffer input_payload;
      PF::Util::convert_string_to_byte_vector(input_data, input_payload);

      auto final_status = datagram_sender.transform(input_payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_status == TestDatagramSender::Status::ok);
      }
      THEN("The seq field of the body's header is equal to 0x01") {
        REQUIRE(output_datagram.operator[](0) == 1);
      }
      THEN("The length field of the body's header is equal to the size of the payload given") {
        REQUIRE(output_datagram.operator[](1) == 6);
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0x6b 0x05 0xb9 0xf3 0xe5 "
          "0xb6'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x6b, 0x05, 0xb9, 0xf3, 0xe5, 0xb6);
          REQUIRE(output_datagram.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output datagram is as expected '0x01 0x06 0x6b 0x05 0xb9 0xf3 0xe5 0xb6' ") {
        auto expected = std::string("\x01\x06\x6b\x05\xb9\xf3\xe5\xb6", 8);
        REQUIRE(output_datagram == expected);
      }
    }
  }

  GIVEN("A Datagram sender of buffer size 254 bytes with next sequence equal to 0xff") {
    auto data = std::string("\xa0\x47\x65\x6b\x20\xe9", 6);
    TestDatagramProps::PayloadBuffer payload;
    PF::Util::convert_string_to_byte_vector(data, payload);

    PF::Util::ByteVector<buffer_size> output_datagram;
    int i = 0;
    do {
      auto transform_status = datagram_sender.transform(payload, output_datagram);
      REQUIRE(transform_status == TestDatagramSender::Status::ok);
      i++;
    } while (i < 255);

    WHEN("2 non-empty payloads of capacity 254 bytes is given to the sender") {
      auto transform_status = datagram_sender.transform(payload, output_datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The seq field of the body's header is equal to 0xff") {
        REQUIRE(output_datagram.operator[](0) == 0xff);
      }
      THEN("The length field of the body's header is equal to the size of the payload given") {
        REQUIRE(output_datagram.operator[](1) == 6);
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0xa0 0x47 0x65 0x6b 0x20 "
          "0xe9'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0xa0, 0x47, 0x65, 0x6b, 0x20, 0xe9);
          REQUIRE(output_datagram.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output datagram is as expected '0xff 0x06 0xa0 0x47 0x65 0x6b 0x20 0xe9' ") {
        auto expected = std::string("\xff\x06\xa0\x47\x65\x6b\x20\xe9", 8);
        REQUIRE(output_datagram == expected);
      }

      auto input_data = std::string("\x51\xb5\x55\x69\x8f\x67", 6);

      TestDatagramProps::PayloadBuffer input_payload;
      PF::Util::convert_string_to_byte_vector(input_data, input_payload);

      auto final_status = datagram_sender.transform(input_payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_status == TestDatagramSender::Status::ok);
      }
      THEN("The seq field of the body's header is equal to 0x00") {
        REQUIRE(output_datagram.operator[](0) == 0);
      }
      THEN("The length field of the body's header is equal to the size of the payload given") {
        REQUIRE(output_datagram.operator[](1) == 6);
      }
      THEN(
          "The body's payload section correctly stores the paylaod as '0x51 0xb5 0x55 0x69 0x8f "
          "0x67'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x51, 0xb5, 0x55, 0x69, 0x8f, 0x67);
          REQUIRE(output_datagram.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output datagram is as expected '0x00 0x06 0x51 0xb5 0x55 0x69 0x8f 0x67' ") {
        auto expected = std::string("\x00\x06\x51\xb5\x55\x69\x8f\x67", 8);
        REQUIRE(output_datagram == expected);
      }
    }
  }
}
