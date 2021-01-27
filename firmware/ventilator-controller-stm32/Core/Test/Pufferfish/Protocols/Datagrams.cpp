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

#include "Pufferfish/Util/Array.h"
#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Protocols::Datagram behaves correctly", "[Datagram]") {

// write function

  GIVEN("A Datagram with 0 bytes payload and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;

    TestDatagram datagram(input_payload);

    WHEN("payload of 0 bytes is written into the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;
      auto write_status = datagram.write(output_buffer);

      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }

      THEN("The length of output datagram is equal to 0") {
        REQUIRE(datagram.length() == 0);
      }

      THEN("The payload of output buffer is equal to 0") {
        auto expected = std::string("\x00");
        REQUIRE(datagram.payload() == expected);
      }
    }
  }

  GIVEN("A Datagram with valid payload and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);
    PF::Util::convertStringToByteVector(data, input_payload);

    TestDatagram datagram{input_payload};

    WHEN("The payload is written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }

      THEN("The length of output buffer payload is equal to input payload length") {
        REQUIRE(datagram.length() == data.length());
      }

      THEN("The payload of output buffer is equal to as expected") {
        REQUIRE(datagram.payload() == data);
      }

      THEN("output buffer sequence is equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
    }
  }

  GIVEN("A Datagram with valid payload and non-zero sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);
    PF::Util::convertStringToByteVector(data, input_payload);

    uint8_t seq = 10;

    TestDatagram datagram{input_payload, seq};

    WHEN("The payload is written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }

      THEN("The length of output buffer payload is equal to input payload length") {
        REQUIRE(datagram.length() == data.length());
      }

      THEN("The payload of output buffer is equal to as expected") {
        REQUIRE(datagram.payload() == data);
      }

      THEN("output buffer sequence is equal to 0") {
        REQUIRE(datagram.seq() == 10);
      }
    }
  }

  GIVEN("A Datagram with valid payload and 255 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer input_payload;
    auto data = std::string("\x01\x05\x01\x02\x03\x04\x05", 7);
    PF::Util::convertStringToByteVector(data, input_payload);

    uint8_t seq = 255;

    TestDatagram datagram{input_payload, seq};

    WHEN("The payload is written to the output buffer") {
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto write_status = datagram.write(output_buffer);
      THEN("The write status is equal to ok") {
        REQUIRE(write_status == PF::IndexStatus::ok);
      }

      THEN("The length of output buffer payload is equal to input payload length") {
        REQUIRE(datagram.length() == data.length());
      }

      THEN("The payload of output buffer is equal to as expected") {
        REQUIRE(datagram.payload() == data);
      }

      THEN("output buffer sequence is equal to 255") {
        REQUIRE(datagram.seq() == 255);
      }
    }
  }

  GIVEN("A Datagram with valid payload and 0 sequence") {
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

  // Parse function

  GIVEN("A Datagram with 0 byte payload and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    TestDatagramProps::PayloadBuffer payload;
    uint8_t seq = 0;
    TestDatagram datagram{payload, seq};

    WHEN("Input buffer of 0 payload, sequence and length is parsed") {
      auto data = std::string("\x00", 1);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(data, input_buffer);

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse status is equal to out of bounds") {
        REQUIRE(parse_status == PF::IndexStatus::out_of_bounds);
      }

      THEN("The length of datagram is equal to 0") {
        REQUIRE(datagram.length() == 0);
      }

      THEN("The sequence of datagram is equal to 0") {
        REQUIRE(datagram.seq() == 0);
      }
    }
  }

  GIVEN("A Datagram with payload of length 5 bytes and 0 sequence") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

    auto data = std::string("\x01\x02\x03\x04\x05", 5);
    TestDatagramProps::PayloadBuffer payload;
    PF::Util::convertStringToByteVector(data, payload);
    TestDatagram datagram{payload};

    WHEN("An Input buffer of valid payload, length and non zero sequence is parsed") {
      auto body = std::string("\x01\x05\x11\x12\x13\x14\x15", 7);
      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);

      THEN("The parse status is equal to ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }

      THEN("The payload of the datagram is equal to the payload from the input buffer") {
        auto expected_payload = std::string("\x11\x12\x13\x14\x15", 5);
        REQUIRE(datagram.payload() == expected_payload);
      }

      THEN("The length of the datagram is equal to the length of the payload from the input buffer") {
        REQUIRE(datagram.length() == 5);
      }

      THEN("The sequence of datagram is equal to sequence in the input buffer") {
        REQUIRE(datagram.seq() == 1);
      }
    }

    WHEN("An input buffer of 0 payload and length is parsed") {
      auto body = std::string("\x04\x00\x00", 3);

      PF::Util::ByteVector<buffer_size> input_buffer;
      PF::Util::convertStringToByteVector(body, input_buffer);

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse status is equal to ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }

      THEN("The payload of the datagram is equal to the initial payload") {
        auto expected_payload = std::string("\x00", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }

      THEN("The length of the datagram is equal to the length of the payload from the input buffer") {
        REQUIRE(datagram.length() == 0);
      }

      THEN("The sequence of datagram is equal to sequence in the input buffer") {
        REQUIRE(datagram.seq() == 4);
      }
    }

    WHEN("An input buffer of invalid length is parsed") {
      auto body = PF::Util::make_array<uint8_t>(0x04, 0x100, 0x01);

      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data: body) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse status is equal to ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }

      THEN("The payload of the datagram is equal to the initial payload") {
        auto expected_payload = std::string("\x01", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }

      THEN("The length of the datagram is equal to 0") {
        REQUIRE(datagram.length() == 0);
      }

      THEN("The sequence of datagram is equal to sequence in the input buffer") {
        REQUIRE(datagram.seq() == 4);
      }
    }

    WHEN("An input buffer of invalid sequence is parsed") {
      auto body = PF::Util::make_array<uint8_t>(0x100, 0x01, 0x01);

      PF::Util::ByteVector<buffer_size> input_buffer;

      for (auto& data: body) {
        input_buffer.push_back(data);
      }

      auto parse_status = datagram.parse(input_buffer);
      THEN("The parse status is equal to ok") {
        REQUIRE(parse_status == PF::IndexStatus::ok);
      }

      THEN("The payload of the datagram is equal to payload of the input buffer") {
        auto expected_payload = std::string("\x01", 1);
        REQUIRE(datagram.payload() == expected_payload);
      }

      THEN("The length of the datagram is equal to the length in the input buffer") {
        REQUIRE(datagram.length() == 1);
      }

      THEN("The sequence of datagram is equal 0") {
        REQUIRE(datagram.seq() == 0);
      }
    }

    WHEN("an input buffer with invalid payload is written") {
      // This cannot be tested as any payload, valid or invalid,
      // will always be returned as-is.
    }
   }
} 

SCENARIO("Protocols::Datagram Receiver behaves correctly", "[DatagramReceiver]") {
  GIVEN("A Datagram receiver of buffer size 254 bytes") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
    using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;
    using TestDatagramReceiver = PF::Protocols::DatagramReceiver<buffer_size>;

    TestDatagramProps::PayloadBuffer payload;
    TestDatagram datagram{payload};

    TestDatagramReceiver datagram_receiver{};

    WHEN("The input buffer is of invalid length") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto body = std::string("\x00", 1);
      PF::Util::convertStringToByteVector(body, input_buffer);
      
      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform status is equal to invalid parse") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_parse);
      }
    }

    WHEN("The input buffer length is not equal to the actual length of the payload") {
      PF::Util::ByteVector<buffer_size> input_buffer;
      auto body = std::string("\x00\x05\x01\x02\x03", 5);
      auto expected_payload = std::string("\x01\x02\x03", 3);

      PF::Util::convertStringToByteVector(body, input_buffer);
      
      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform status is equal to invalid parse") {
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

      THEN("The transform status is equal to invalid parse") {
        REQUIRE(transform_status == TestDatagramReceiver::Status::invalid_sequence);
      }

      THEN("The output datagram payload is equal to the input buffer paylaod") {
        REQUIRE(datagram.payload() == expected_payload);
      }

      THEN("The output datagram length is equal to the input buffer payload length") {
        REQUIRE(datagram.length() == expected_payload.length());
      }
    }

    WHEN("The input buffer has valid length, sequence and payload") {
      auto body = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
      auto expected_payload = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::ByteVector<buffer_size> input_buffer;
      auto convert_status = PF::Util::convertStringToByteVector(body, input_buffer);

      auto transform_status = datagram_receiver.transform(input_buffer, datagram);

      THEN("The transform status is equal to invalid parse") {
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
}

SCENARIO("Protocols::Datagram Sender behaves correctly", "[DatagramSender]") {
  GIVEN("A Datagram sender of buffer size 254 bytes") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramSender = PF::Protocols::DatagramSender<buffer_size>;
    TestDatagramSender datagram_sender{};

    WHEN("A valid payload is given as input to datagram sender") {
      using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
      TestDatagramProps::PayloadBuffer payload;

      auto data = std::string("\x01\x02\x03\x04\x05", 5);

      PF::Util::convertStringToByteVector(data, payload);

      PF::Util::ByteVector<buffer_size> output_datagram;
      auto transform_status = datagram_sender.transform(payload, output_datagram);

      THEN("The transform status is equal to ok") {
        REQUIRE(transform_status == TestDatagramSender::Status::ok);
      }

      THEN("The output datagram is as expected") {
        auto expected_datagram = std::string("\x00\x05\x01\x02\x03\x04\x05", 7);
        REQUIRE(output_datagram == expected_datagram);
      }
    }
  }

  GIVEN("A Datagram sender of buffer size 254 bytes") {
    constexpr size_t buffer_size = 254UL;
    using TestDatagramSender = PF::Protocols::DatagramSender<buffer_size>;
    TestDatagramSender datagram_sender{};

    WHEN("The output datagram is too small to fit input payload") {
      using TestDatagramProps = PF::Protocols::DatagramProps<buffer_size>;
      using TestDatagram = PF::Protocols::Datagram<TestDatagramProps::PayloadBuffer>;

      auto data = std::string("\x01\x02\x03\x04\x05\x06\x07\x08\x09", 9);

      TestDatagramProps::PayloadBuffer payload;
      PF::Util::convertStringToByteVector(data, payload);

      constexpr size_t datagram_buffer_size = 10UL;
      PF::Util::ByteVector<datagram_buffer_size> output_datagram;

      auto transform_status = datagram_sender.transform(payload, output_datagram);

      THEN("The transform status is equal to invalid length") {
        REQUIRE(transform_status == TestDatagramSender::Status::invalid_length);
      }
    }
  }
}
