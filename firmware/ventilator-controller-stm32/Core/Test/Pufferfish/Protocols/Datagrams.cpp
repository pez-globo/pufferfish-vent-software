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
  GIVEN("A Datagram with 0 bytes payload and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;

    TestDatagram datagram(input_payload);

    WHEN("payload of 0 bytes is written into the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The write status is equal to ok") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN("The length of output datagram is equal to 0") { REQUIRE(datagram.length() == 0); }
    }
  }

  GIVEN("A Datagram with internal payload and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x01\x02\x03\x04\x05", 5);
    PF::Util::convertStringToByteVector(data, input_payload);

    TestDatagram datagram{input_payload};

    WHEN("The datagram is written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write status is equal to ok") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN("The length returned by the length() method is equal to the input payload length") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN("The payload of datagram is equal to as expected") {
        REQUIRE(datagram.payload() == data);
      }
      THEN("The sequence returned by the seq() method is equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
        REQUIRE(output_buffer == expected_buffer);
      }
    }

    WHEN("The payload is altered after datagram initalisation and the write method is called") {
      input_payload.push_back(0x02);

      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write status is equal to ok") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN("The length returned by the length() method is equal to the input payload length") {
        REQUIRE(datagram.length() == 6);
      }
      THEN("The payload of datagram is equal to the altered payload") {
        auto expected = std::string("\x01\x02\x03\x04\x05\x02", 6);
        REQUIRE(datagram.payload() == expected);
      }
      THEN("The sequence returned by the seq() method is equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\x00\x06\x01\x02\x03\x04\x05\x02", 8);
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }

  GIVEN("A Datagram with internal payload and non-zero sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x12\x23\x34\x45\x56\67", 6);
    PF::Util::convertStringToByteVector(data, input_payload);

    uint8_t seq = 10;

    TestDatagram datagram{input_payload, seq};

    WHEN("The datagram is written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write status is equal to ok") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN("The length returned by the length() method is equal to the input payload length") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN("The payload of output buffer is equal to as expected") {
        REQUIRE(datagram.payload() == data);
      }
      THEN("The sequence returned by the seq() method is equal to 10") {
        REQUIRE(datagram.seq() == 10);
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\x0A\x06\x12\x23\x34\x45\x56\x37", 8);
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }

  GIVEN("A Datagram with internal payload and a sequence ranging from 0 to 255") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x01\x23\x45\x0a\x4d\x04\x05", 7);
    PF::Util::convertStringToByteVector(data, input_payload);

    uint8_t sequence = 0;
    for (int i = 0; i <= 255; ++i) {
      sequence = i;
    }

    TestDatagram datagram{input_payload, sequence};

    WHEN("The payload is written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write status is equal to ok") { REQUIRE(write_status == PF::IndexStatus::ok); }
      THEN("The length returned by the length() method is equal to the input payload length") {
        REQUIRE(datagram.length() == data.length());
      }
      THEN("The payload of output buffer is equal to as expected") {
        REQUIRE(datagram.payload() == data);
      }
      THEN(
          "The sequence returned by the seq() method is same as the sequence given in the "
          "input_payload") {
        REQUIRE(datagram.seq() == sequence);
      }
      THEN("The output buffer is as expected") {
        auto expected_buffer = std::string("\xFF\x07\x01\x23\x45\x0A\x4D\x04\x05", 9);
        REQUIRE(output_buffer == expected_buffer);
      }
    }
  }

  GIVEN("A Datagram with internal payload and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto payload = std::string("\x01\x05\x01\x02\x03\x04\x05\x06\x07\x08", 10);
    PF::Util::convertStringToByteVector(payload, input_payload);
    TestDatagram datagram{input_payload};

    WHEN("The output buffer cannot hold the input buffer data") {
      constexpr size_t output_buffer_size = 10UL;
      PF::Util::ByteVector<output_buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The write status is equal to out of bounds") {
        REQUIRE(write_status == PF::IndexStatus::out_of_bounds);
      }
    }
  }
}

SCENARIO(
    "Protocols::The parse function correctly updates internal length, sequence and payload fields "
    "from input buffer",
    "[Datagram]") {
  GIVEN("A Datagram with 0 byte payload and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer payload;
    uint8_t seq = 0;
    TestDatagram datagram{payload, seq};

    WHEN("Input buffer of 0 payload, sequence and length is parsed") {
      auto input_data = PF::Util::make_array<uint8_t>(0x00);
      PF::Util::ByteVector<buffer_size> input_buffer;
      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse status is equal to out of bounds") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }
      THEN("The length returned by the length() method is equal to 0") {
        REQUIRE(datagram.length() == 0);
      }
      THEN("The sequence returned by the seq() method is equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
    }
  }

  GIVEN("A Datagram with payload of 0 sequence and a payload field of capacity 254 bytes") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    auto data = std::string("\x01\x02\x03\x04\x05", 5);
    TestDatagramProps::PayloadBuffer payload;
    PF::Util::convertStringToByteVector(data, payload);
    TestDatagram datagram{payload};

    WHEN("An Input buffer of non empty payload buffer, length and non zero sequence is parsed") {
      auto body = std::string("\x01\x05\x11\x12\x13\x14\x15", 7);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse status is equal to ok") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN("The payload of the datagram is equal to the payload from the input buffer") {
        auto expected_payload = std::string("\x11\x12\x13\x14\x15", 5);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The length of the datagram is equal to the length of the payload from the input "
          "buffer") {
        REQUIRE(datagram.length() == 5);
      }
      THEN(
          "The sequence returned by the seq() method is same as the sequence in the input buffer") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN("The input buffer is unchanged after parse") { REQUIRE(input_buffer == body); }
    }

    WHEN("The input buffer ['0x01 0x05 0x11 0x12 0x13 0x14 0x15'] is changed after its parsed") {
      auto body = std::string("\x01\x05\x11\x12\x13\x14\x15", 7);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse status is equal to ok") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      auto expected_payload = std::string("\x11\x12\x13\x14\x15", 5);
      THEN("The payload of datagram changes after parse method is called") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN(
          "The length of the datagram is equal to the length of the payload from the input "
          "buffer") {
        REQUIRE(datagram.length() == 5);
      }
      THEN("The sequence returned by the seq() method is equal to 1") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN("The input buffer is not modified by the parse method") {
        REQUIRE(input_buffer == body);
      }

      input_buffer.push_back(0x16);

      THEN("The payload returned by the payload() method is same as in the original buffer") {
        REQUIRE(datagram.payload() == expected_payload);
      }
    }

    WHEN(
        "An input buffer where the length field of the header is 0 and the payload field is empty "
        "is parsed") {
      auto input_data = PF::Util::make_array<uint8_t>(0x04, 0x00, 0x00);
      PF::Util::ByteVector<buffer_size> input_buffer;
      for (auto& data : input_data) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse status is equal to ok") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN("The payload of the datagram is equal to the initial payload") {
        auto expected_payload = std::string("\x00", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The length returned by the length() method is equal to 0") {
        REQUIRE(datagram.length() == 0);
      }
      THEN("The sequence returned by the seq() method is same as the sequence in input buffer") {
        REQUIRE(datagram.seq() == 4);
      }
      THEN("The input buffer is unchanged after parse") {
        auto initial_buffer = std::string("\x04\x00\x00", 3);
        REQUIRE(input_buffer == initial_buffer);
      }
    }

    WHEN(
        "The length header of the input buffer does not match the actual length of the payload "
        "field") {
      auto body = PF::Util::make_array<uint8_t>(0x04, 0x05, 0x01);

      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : body) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse status is equal to ok") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN("The payload of the datagram is equal to the initial payload") {
        auto expected_payload = std::string("\x01", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The length of the datagram is equal to 5") { REQUIRE(datagram.length() == 5); }
      THEN(
          "The sequence returned by the seq() method is same as the sequence in the input buffer") {
        REQUIRE(datagram.seq() == 4);
      }
    }

    WHEN("An input buffer of invalid sequence is parsed") {
      auto body = PF::Util::make_array<uint8_t>(0x100, 0x01, 0x01);

      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data : body) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse status is equal to ok") { REQUIRE(parse_status == PF::IndexStatus::ok); }
      THEN("The payload of the datagram is equal to payload of the input buffer") {
        auto expected_payload = std::string("\x01", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The length of the datagram is equal to the length in the input buffer") {
        REQUIRE(datagram.length() == 1);
      }
      THEN("The sequence returned by the seq() method is equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
    }

    WHEN("an input buffer with invalid payload is written") {
      // This cannot be tested as any payload, valid or invalid,
      // will always be returned as-is.
    }
  }
}

SCENARIO(
    "Protocols::Datagram Receiver correctly parses datagrams into payloads", "[DatagramReceiver]") {
  GIVEN("A Datagram receiver of buffer size 254 bytes and expected sequence number equal to 0") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    using TestDatagramReceiver = PF::Protocols::DatagramReceiver<buffer_size>;

    TestDatagramProps::PayloadBuffer payload;
    TestDatagram datagram{payload};

    TestDatagramReceiver datagram_receiver{};

    WHEN("An input buffer of size less than 2 bytes is given to the receiver") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto body = std::string("\x00", 1);
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid_parse status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_parse);
      }
    }

    WHEN("The input buffer length is not equal to the actual length of the payload") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto body = std::string("\x00\x05\x01\x02\x03", 5);
      auto expected_payload = std::string("\x01\x02\x03", 3);

      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid length status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_length);
      }
      THEN("The output datagram payload is equal to the input buffer paylaod") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The output datagram length is not equal to the input buffer payload length") {
        REQUIRE(datagram.length() != expected_payload.length());
      }
    }

    WHEN("The input buffer sequence is not equal to 0") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto body = std::string("\x04\x03\x01\x02\x03", 5);
      auto expected_payload = std::string("\x01\x02\x03", 3);
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN("The output datagram payload is equal to the input buffer paylaod") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The output datagram length is equal to the input buffer payload length") {
        REQUIRE(datagram.length() == expected_payload.length());
      }
    }

    WHEN("The input buffer has a valid length, sequence and a non-empty payload") {
      auto body = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
      auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN("The output datagram payload is equal to the input buffer paylaod") {
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The output datagram length is equal to the input buffer payload length") {
        REQUIRE(datagram.length() == expected_payload.length());
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
    PF::Util::convertStringToByteVector(body, input_buffer);

    auto transform_status = datagram_receiver.transform(input_buffer, datagram);
    REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
    REQUIRE(datagram.payload() == expected_payload);
    REQUIRE(datagram.length() == expected_payload.length());

    WHEN("The input buffer has a valid length, sequence and a non-empty payload") {
      auto body = std::string("\x01\x03\x12\x13\x14", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to 1") {
        REQUIRE(datagram.seq() == 1);
      }
      THEN("The output datagram payload is equal to the input buffer paylaod") {
        auto expected_payload = std::string("\x12\x13\x14", 3);
        REQUIRE(datagram.payload() == expected_payload);
      }
      THEN("The output datagram length is equal to the input buffer payload length") {
        REQUIRE(datagram.length() == 3);
      }
    }

    WHEN(
        "The first input buffer has a sequence equal to 1, and the second buffer has a sequence "
        "equal to 3, and transform is called on both") {
      auto body = std::string("\x02\x03\x12\x13\x14", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 2);
      }

      auto data = std::string("\x03\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convertStringToByteVector(data, buffer);

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
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports invalid sequence status") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0xff);
      }

      auto data = std::string("\x00\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convertStringToByteVector(data, buffer);

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
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto first_transform = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform method reports ok status") {
        REQUIRE(first_transform == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0x01);
      }

      auto data = std::string("\x10\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> buffer;
      PF::Util::convertStringToByteVector(data, buffer);

      auto second_transform = datagram_receiver.transform(buffer, datagram);
      THEN("The transform method reports invalid_sequence status") {
        REQUIRE(second_transform == TestDatagramReceiver::Status::invalid_sequence);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0x10);
      }

      auto input_data = std::string("\x11\x03\x12\x13\x14", 5);
      PF::Util::ByteVector<buffer_size> final_buffer;
      PF::Util::convertStringToByteVector(input_data, final_buffer);

      auto final_transform = datagram_receiver.transform(final_buffer, datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(final_transform == TestDatagramReceiver::Status::ok);
      }
      THEN("The sequence returned by the seq() method is equal to the sequence in input buffer") {
        REQUIRE(datagram.seq() == 0x11);
      }
    }
  }
}

SCENARIO(
    "Protocols::Datagram Sender correctly generates datagrams from payloads", "[DatagramSender]") {
  constexpr size_t buffer_size = 254UL;
  using TestDatagramSender = PF::Protocols::DatagramSender<buffer_size>;
  TestDatagramSender datagram_sender{};

  GIVEN("A Datagram sender of buffer size 254 bytes with next sequence equal to 0") {
    WHEN("A valid payload is given as input to datagram sender") {
      using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
      TestDatagramProps::PayloadBuffer payload;

      auto data = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::convertStringToByteVector(data, payload);

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
      PF::Util::convertStringToByteVector(data, payload);

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
      PF::Util::convertStringToByteVector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;

      auto transform_status = datagram_sender.transform(payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The output datagram is as expected '\x00\x02\x00\x00' ") {
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
      PF::Util::convertStringToByteVector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;

      auto transform_status = datagram_sender.transform(payload, output_datagram);
      THEN("The transform method reports ok status") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }
      THEN("The output datagram is as expected '\x00\x05\x01\x02\x03\x04\x05' ") {
        auto expected = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
        REQUIRE(output_datagram == expected);
      }

      auto input_data = std::string("\x12\x13\x14\x15\x16", 5);

      TestDatagramProps::PayloadBuffer input_payload;
      PF::Util::convertStringToByteVector(input_data, input_payload);

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
