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
#include "Pufferfish/Util/Array.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO(
    "Protocols::The write function correctly calculates length of internal payload and writes body "
    "with sequence, length and payload",
    "[Datagram]") {
  GIVEN("A Datagram constructed with an empty paylaod and sequence equal to 0") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;

    TestDatagram datagram(input_payload);

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN("the value returned by the length accessor method is equal to 0") {
        REQUIRE(datagram.length() == 0);
      }
      THEN("The output buffer is as expected") { auto expected = std::string("\x00\x00", 2); }
    }
  }

  GIVEN(
      "A Datagram constructed with paylaod buffer as '0x01 0x02 0x03 0x04 0x05' and sequence equal "
      "to 0") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x01\x02\x03\x04\x05", 5);
    PF::Util::convert_string_to_byte_vector(data, input_payload);

    TestDatagram datagram{input_payload};

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method is "
          "equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "the length accessor method returns a value equal to the size of the payload given in "
          "the constructor") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN(
          "The buffer returned by the paylaod accessor method is same as the paylaod buffer given "
          "in the constructor") {
        REQUIRE(datagram.payload() == data);
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
          "The body's payload section correctly stores the paylaod as '0x01 0x02 0x03 0x04 0x05'") {
        for (size_t i = 2; i < 7; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x03, 0x04, 0x05);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
        REQUIRE(output_buffer == expected_buffer);
      }
    }

    WHEN(
        "The paylaod given to the constructor is altered ('0x01 0x02 0x03 0x04 0x05 0x02') before "
        "write method is called") {
      input_payload.push_back(0x02);

      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method is "
          "equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "the length accessor method returns a value equal to the size of the payload given in "
          "the constructor") {
        REQUIRE(datagram.length() == 6);
      }
      THEN(
          "The buffer returned by the paylaod accessor method is same as the paylaod buffer given "
          "in the constructor") {
        auto expected = std::string("\x01\x02\x03\x04\x05\x02", 6);
        REQUIRE(datagram.payload() == expected);
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
          "The body's payload section correctly stores the paylaod as '0x01 0x02 0x03 0x04 0x05'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x03, 0x04, 0x05, 0x02);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\x00\x06\x01\x02\x03\x04\x05\x02", 8);
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }

  GIVEN(
      "A Datagram constructed with payload '0x12 0x23 0x34 0x45 0x56 0x67' and sequence equal to "
      "10") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x12\x23\x34\x45\x56\67", 6);
    PF::Util::convert_string_to_byte_vector(data, input_payload);

    uint8_t seq = 10;

    TestDatagram datagram{input_payload, seq};

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method is "
          "equal to 10") {
        REQUIRE(datagram.seq() == 10);
      }
      THEN(
          "the length accessor method returns a value equal to the size of the payload given in "
          "the constructor") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN(
          "The buffer returned by the paylaod accessor method is same as the paylaod buffer given "
          "in the constructor") {
        REQUIRE(datagram.payload() == data);
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
          "The body's payload section correctly stores the paylaod as '0x12 0x23 0x34 0x45 0x56 "
          "0x67'") {
        for (size_t i = 2; i < 8; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x12, 0x23, 0x34, 0x45, 0x56, 0x37);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\x0A\x06\x12\x23\x34\x45\x56\x37", 8);
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }

  GIVEN(
      "A Datagram constructed with paylaod '0x01 0x23 0x45 0x0a 0x4d 0x04 0x05' and a sequence "
      "ranging from 0 to 0xff") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x01\x23\x45\x0a\x4d\x04\x05", 7);
    PF::Util::convert_string_to_byte_vector(data, input_payload);

    uint8_t sequence = 0;
    for (int i = 0; i <= 255; ++i) {
      sequence = i;
    }

    TestDatagram datagram{input_payload, sequence};

    WHEN("The sequence, length and paylaod are written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write method reports ok status") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN(
          "After the write method is called, The value returned by the seq accessor method is "
          "equal to the input sequence") {
        REQUIRE(datagram.seq() == sequence);
      }
      THEN(
          "the length accessor method returns a value equal to the size of the payload given in "
          "the constructor") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN(
          "The buffer returned by the paylaod accessor method is same as the paylaod buffer given "
          "in the constructor") {
        REQUIRE(datagram.payload() == data);
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
          "The body's payload section correctly stores the paylaod as '0x01 0x23 0x45 0x0a 0x4d "
          "0x04 0x05'") {
        for (size_t i = 2; i < 9; ++i) {
          auto data = PF::Util::make_array<uint8_t>(0x01, 0x23, 0x45, 0x0a, 0x4d, 0x04, 0x05);
          REQUIRE(output_buffer.operator[](i) == data[i - 2]);
        }
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\xFF\x07\x01\x23\x45\x0A\x4D\x04\x05", 9);
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }

  GIVEN(
      "A Datagram constructed with payload '0x01 0x05 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08' and "
      "sequence equal to 0") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto payload = std::string("\x01\x05\x01\x02\x03\x04\x05\x06\x07\x08", 10);
    PF::Util::convert_string_to_byte_vector(payload, input_payload);
    TestDatagram datagram{input_payload};

    WHEN("The output buffer is too short and cannot hold the sequence, length and paylaod fields") {
      constexpr size_t output_buffer_size = 10UL;
      PF::Util::ByteVector<output_buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The write method reports out of bounds status") {
        REQUIRE(write_status == PF::IndexStatus::out_of_bounds);
      }
      THEN("The output buffer is as expected") { REQUIRE(output_buffer.empty() == true); }
    }
  }
}

SCENARIO(
    "Protocols:: The parse function correctly updates internal length member and constructor's "
    "paylaod buffer and sequence fields from the input_buffer",
    "[Datagram]") {
  GIVEN("A Datagram constructed with an empty paylaod and sequence equal to 0") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer payload;
    uint8_t seq = 0;
    TestDatagram datagram{payload, seq};

    WHEN("A body with a complete 2-byte header and an empty paylaod buffer is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;

      auto parse_status = datagram.parse(input_buffer);

      THEN("After the parse method is called, the seq accessor method returns 0") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("The length accessor method returns 0") { REQUIRE(datagram.length() == 0); }
      THEN("The parse method reports out of bounds status") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }
    }
  }

  GIVEN("A Datagram constructed with paylaod '0x01 0x02 0x03 0x04 0x05' and sequence equal to 0") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    auto data = std::string("\x01\x02\x03\x04\x05", 5);
    TestDatagramProps::PayloadBuffer payload;
    PF::Util::convert_string_to_byte_vector(data, payload);
    TestDatagram datagram{payload};

    WHEN(
        "A body with a complete 2-byte header, and a non-empty paylaod buffer consistent with the "
        "length field of the header is parsed") {
      auto body = std::string("\x01\x05\x11\x12\x13\x14\x15", 7);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input body's header") {
        REQUIRE(datagram.length() == 5);
      }
      THEN(
          "The payload returned from the payload accessor method is equal to the payload from the "
          "body") {
        auto expected_payload = std::string("\x11\x12\x13\x14\x15", 5);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == body); }
    }

    WHEN(
        "A body with a complete 2-byte header, and paylaod buffer as '0x11 0x12 0x13 0x14 0x15' "
        "consistent with the length field of the header, is altered after it's parsed") {
      auto body = std::string("\x01\x05\x11\x12\x13\x14\x15", 7);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      auto expected_payload = std::string("\x11\x12\x13\x14\x15", 5);
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input body's header") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input body's header") {
        REQUIRE(datagram.length() == 5);
      }
      THEN(
          "The payload buffer returned from the payload accessor method is equal to the payload "
          "from the body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == body); }

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
      PF::Util::ByteVector<buffer_size> input_buffer;
      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input body's header") {
        REQUIRE(datagram.length() == 1);
      }

      THEN("The paylaod buffer returned by the paylaod accessor method is equal to '0x00' ") {
        auto expected_payload = std::string("\x00", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The input buffer is unchanged after parse") {
        auto initial_buffer = std::string("\x00\x01\x00", 3);
        REQUIRE(input_buffer == initial_buffer);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, and a non-empty payload buffer where the length "
        "field of the header is inconsistent with the actual length of the payload buffer") {
      auto body = PF::Util::make_array<uint8_t>(0x00, 0x05, 0x01, 0x02, 0x03);

      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : body) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input body's header") {
        REQUIRE(datagram.length() == 5);
      }
      THEN(
          "The paylaod buffer returned by the paylaod accessor method is equal to the payload from "
          "the body") {
        auto expected_payload = std::string("\x01\x02\x03", 3);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The input buffer is unchanged after parse") {
        auto initial_buffer = std::string("\x00\x05\x01\x02\x03", 5);
        REQUIRE(input_buffer == initial_buffer);
      }
    }

    WHEN(
        "A body with sequence field of the header inconsistent with sequence given in the "
        "constructor of the datagram") {
      auto body = PF::Util::make_array<uint8_t>(0x05, 0x01, 0x01);

      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : body) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports ok status") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN(
          "After the parse method is called, The value returned by the seq accessor method is "
          "equal to the sequence field of the input body's header") {
        REQUIRE(datagram.seq() == 5);
      }
      THEN(
          "The value returned by the length accessor method is equal to the length field of the "
          "input body's header") {
        REQUIRE(datagram.length() == 1);
      }
      THEN(
          "The payload buffer returned from the payload accessor method is equal to the payload "
          "from the body") {
        auto expected_payload = std::string("\x01", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The input buffer is unchanged after parse") {
        auto initial_buffer = std::string("\x05\x01\x01", 3);
        REQUIRE(input_buffer == initial_buffer);
      }
    }

    WHEN("A body with an incomplete 2-byte header and an empty payload is parsed") {
      PF::Util::ByteVector<buffer_size> input_buffer;

      input_buffer.push_back(0x01);
      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse method reports out_of_bounds status") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }
      THEN(
          "The payload buffer returned from the payload accessor method is equal to the payload "
          "buffer given in the constructor") {
        auto expected = std::string("\x01\x02\x03\x04\x05", 5);
        REQUIRE(datagram.payload() == expected);
      }
    }

    WHEN("an input buffer with invalid payload is written") {
      // This cannot be tested as any payload, valid or invalid,
      // will always be returned as-is.
    }
  }
}

SCENARIO(
    "Protocols::Datagram Receiver correctly parses datagram bodies and performs consistency "
    "checking on them",
    "[DatagramReceiver]") {
  GIVEN(
      "A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 0, with "
      "output_datagram constructed with empty payload buffer") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    using TestDatagramReceiver = PF::Protocols::DatagramReceiver<buffer_size>;

    TestDatagramProps::PayloadBuffer payload;
    TestDatagram datagram{payload};

    TestDatagramReceiver datagram_receiver{};

    WHEN("A body with an incomplete 2-byte header and empty payload is given to the receiver") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      input_buffer.push_back(0x00);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid_parse status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_parse);
      }
      THEN(
          "The payload buffer returned by the payload accessor method of the output datagram "
          "contains the buffer given in the constructor") {
        auto payload = datagram.payload();
        REQUIRE(payload.empty() == true);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, and a non-empty paylaod buffer inconsistent with "
        "the length field of the header") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto body = std::string("\x00\x05\x01\x02\x03", 5);
      auto expected_payload = std::string("\x01\x02\x03", 3);

      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid length status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_length);
      }
      THEN(
          "After the transform method is called, The value of sequence returned by the seq "
          "accessor method of the output datagram is equal to the sequence in the body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value of length returned by the length accessor method of the output datagram is "
          "equal to the length field of the body's header") {
        REQUIRE(datagram.length() == 5);
      }
      THEN(
          "The value of length returned by the length accessor method of the output datagram is "
          "inconsistent with the actual length of the payload buffer from the body") {
        REQUIRE(datagram.length() != expected_payload.length());
      }
      THEN(
          "The payload buffer returned by the payload accessor method of the output datagram is "
          "equal to the payload from the body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("the payload buffer updated in the datagram is independent of the input body") {
        input_buffer.push_back(0x04);

        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "A body with value of the sequence in header is not equal to the sequence in the output "
        "datagram constructor") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto body = std::string("\x04\x03\x01\x02\x03", 5);
      auto expected_payload = std::string("\x01\x02\x03", 3);
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN(
          "After the transform method is called, The value of sequence returned by the seq "
          "accessor method of the output datagram is equal to the sequence in the body's header") {
        REQUIRE(datagram.seq() == 4);
      }
      THEN(
          "The value of length returned by the length accessor method of the output datagram is "
          "equal to the length field of the body's header") {
        REQUIRE(datagram.length() == 3);
      }
      THEN(
          "The payload buffer returned by the payload accessor method of the output datagram is "
          "equal to the payload from the body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("the payload buffer updated in the datagram is independent of the input body") {
        input_buffer.push_back(0x04);

        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "A body with a complete 2-byte header, a non-empty payload buffer consistent with the "
        "length field of the header is given to the receiver") {
      auto body = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
      auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN(
          "After the transform method is called, The value of sequence returned by the seq "
          "accessor method of the output datagram is equal to the sequence in the body's header") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN(
          "The value of length returned by the length accessor method of the output datagram is "
          "equal to the length field of the body's header") {
        REQUIRE(datagram.length() == 5);
      }
      THEN(
          "The payload buffer returned by the payload accessor method of the output datagram is "
          "equal to the payload from the body") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("the payload buffer updated in the datagram is independent of the input body") {
        input_buffer.push_back(0x04);

        REQUIRE(datagram.payload() == expected_payload);
      }
    }
  }

  GIVEN("A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 1") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    using TestDatagramReceiver = PF::Protocols::DatagramReceiver<buffer_size>;

    TestDatagramProps::PayloadBuffer payload;
    TestDatagram datagram{payload};

    TestDatagramReceiver datagram_receiver{};

    auto body = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
    auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

    PF::Util::ByteVector<buffer_size> input_buffer;
    PF::Util::convert_string_to_byte_vector(body, input_buffer);

    auto transform_status = datagram_receiver.transform(input_buffer, datagram);
    REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
    REQUIRE(datagram.payload() == expected_payload);
    REQUIRE(datagram.length() == expected_payload.length());

    WHEN("2 input buffers with a value of sequence in their headers as 1 and 2 respectively") {
      auto body = std::string("\x01\x03\x12\x13\x14", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to 1") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN("The output datagram length is equal to the input buffer payload length") {
        REQUIRE(datagram.length() == 3);
      }
      THEN("The output datagram payload is equal to the input buffer paylaod") {
        auto expected_payload = std::string("\x12\x13\x14", 3);
        REQUIRE(datagram.payload() == expected_payload);
      }

      auto data = std::string("\x02\x04\x01\x02\x03\x04", 6);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto final_transform = datagram_receiver.transform(buffer, datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_transform == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to 1") {
        REQUIRE(datagram.seq() == 2);
      }
      THEN("The output datagram payload is equal to the input buffer paylaod") {
        auto expected_payload = std::string("\x01\x02\x03\x04", 4);
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "The first input buffer has a sequence equal to 2, and the second buffer has a sequence "
        "equal to 3, and transform is called on both") {
      auto body = std::string("\x02\x03\x12\x13\x14", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 2);
      }

      auto data = std::string("\x03\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto final_status = datagram_receiver.transform(buffer, datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_status == TestDatagramReceiver::Status::ok);
      }
    }

    WHEN(
        "The first input buffer has a sequence equal to 0xff and second buffer has a sequence "
        "equal to 0, and transform is called on both") {
      auto body = std::string("\xff\x03\x12\x13\x14", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0xff);
      }

      auto data = std::string("\x00\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto final_status = datagram_receiver.transform(buffer, datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_status == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0);
      }
    }

    WHEN(
        "Three buffers are given as input with sequence numbers equal to 1, 10, 11 respectively, "
        "and transform is called on each") {
      auto body = std::string("\x01\x03\x12\x13\x14", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto first_transform = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(first_transform == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0x01);
      }

      auto data = std::string("\x10\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convert_string_to_byte_vector(data, buffer);

      auto second_transform = datagram_receiver.transform(buffer, datagram);
      THEN("The transform method reports invalid_sequence status") {
        REQUIRE(second_transform == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0x10);
      }

      auto input_data = std::string("\x11\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> final_buffer;
      PF::Util::convert_string_to_byte_vector(input_data, final_buffer);

      auto final_transform = datagram_receiver.transform(final_buffer, datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_transform == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0x11);
      }
    }
  }

  GIVEN("A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 0xff") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    using TestDatagramReceiver = PF::Protocols::DatagramReceiver<buffer_size>;

    TestDatagramProps::PayloadBuffer payload;
    TestDatagram datagram{payload};

    TestDatagramReceiver datagram_receiver{};

    auto body = std::string("\xff\x05\x01\x02\x03\x04\x05", 7);
    auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

    PF::Util::ByteVector<buffer_size> input_buffer;
    PF::Util::convert_string_to_byte_vector(body, input_buffer);

    auto transform_status = datagram_receiver.transform(input_buffer, datagram);
    REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
    REQUIRE(datagram.payload() == expected_payload);
    REQUIRE(datagram.length() == expected_payload.length());

    WHEN("The input buffer has a sequence equal to 0x00 is given to the receiver") {
      auto body = std::string("\x00\x03\x12\x13\x14", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convert_string_to_byte_vector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0x00);
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

      auto data = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::convert_string_to_byte_vector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;
      auto transform_status = datagram_sender.transform(payload, output_datagram);

      THEN("The transform method reports ok") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The output datagram is as expected") {
        auto expected_datagram = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
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
    }

    WHEN("The input paylaod is too large for the output buffer to hold") {
      constexpr size_t payload_size = 256;
      using TestDatagramProps = PF::Protocols::DatagramProps<payload_size>;
      using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

      TestDatagramProps::PayloadBuffer payload;
      for (size_t i = 0; i < buffer_size; ++i) {
        uint8_t val = 10;
        payload.push_back(val);
      }

      PF::Protocols::DatagramSender<payload_size> sender{};

      PF::Util::ByteVector<buffer_size> output_datagram;
      auto transform_status = sender.transform(payload, output_datagram);

      THEN("The transform method reports invalid length") {
        REQUIRE(
            transform_status ==
            PF::Protocols::DatagramSender<payload_size>::Status::invalid_length);
      }
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

  GIVEN("A Datagram sender of buffer size 254 bytes with next sequence equal to 0") {
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    WHEN("2 non-empty payloads of capacity 254 bytes is given to the sender") {
      auto data = std::string("\x01\x02\x03\x04\x05", 5);

      TestDatagramProps::PayloadBuffer payload;
      PF::Util::convert_string_to_byte_vector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;

      auto transform_status = datagram_sender.transform(payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The output datagram is as expected '0x00 0x05 0x01 0x02 0x03 0x04 0x05' ") {
        auto expected = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
        REQUIRE(output_datagram == expected);
      }

      auto input_data = std::string("\x12\x13\x14\x15\x16", 5);

      TestDatagramProps::PayloadBuffer input_payload;
      PF::Util::convert_string_to_byte_vector(input_data, input_payload);

      auto final_status = datagram_sender.transform(input_payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_status == TestDatagramSender::Status::ok);
      }
      THEN("The output datagram is as expected '\x01\x05\x12\x13\x14\x15\x16' ") {
        auto expected = std::string("\x01\x05\x12\x13\x14\x15\x16", 7);
        REQUIRE(output_datagram == expected);
      }
    }
  }
}
