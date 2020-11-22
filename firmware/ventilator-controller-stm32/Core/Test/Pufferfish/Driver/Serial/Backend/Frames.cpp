/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Chunks.cpp
 *
 *  Created on: Nov 17, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of the backend
 *
 */
#include "Pufferfish/Driver/Serial/Backend/Frames.h"
#include "Pufferfish/HAL/CRCChecker.h"
#include "Pufferfish/Test/Util.h"
#include "Pufferfish/Util/Array.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Serial::Frames behaves correctly", "[Backend]") {
  GIVEN("A Backend instance") {
    constexpr size_t buffer_size = 254UL;
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};

    WHEN("a valid body is encoded") {
        auto body = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);
        auto expected_output = std::string("\x0C\x98\xDB\xE3\x55\x01\x05\x01\x02\x03\x04\x05", 12);

        PF::Util::ByteVector<buffer_size> input_buffer;
        PF::Util::ByteVector<buffer_size> output_buffer;

        PF::Util::convertStringToByteVector(body, input_buffer);

        PF::Driver::Serial::Backend::COBSEncoder cobs_encoder{};

        auto status = cobs_encoder.transform(input_buffer, output_buffer);

        THEN("output status should be ok") {
            REQUIRE(status == PF::Driver::Serial::Backend::FrameProps::OutputStatus::ok);
            REQUIRE(output_buffer == expected_output);
        }
    }

    WHEN("a valid body is decoded") {
        auto body = std::string("\x0C\x98\xDB\xE3\x55\x01\x05\x01\x02\x03\x04\x05", 12);
        auto expected_output = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);
        PF::Util::ByteVector<buffer_size> input_buffer;
        PF::Util::ByteVector<buffer_size> output_buffer;

        PF::Util::convertStringToByteVector(body, input_buffer);
        PF::Driver::Serial::Backend::COBSDecoder cobs_decoder{};

        auto status = cobs_decoder.transform(input_buffer, output_buffer);

        THEN("all status should be ok") {
            REQUIRE(status == PF::Driver::Serial::Backend::FrameProps::OutputStatus::ok);
            REQUIRE(output_buffer == expected_output);
        }
    }
  }

  GIVEN("A Frame Receiver") {
    constexpr size_t buffer_size = 254UL;
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Driver::Serial::Backend::FrameReceiver frame_receiver{};
    auto input_data = PF::Util::make_array<uint8_t>(0x01, 0x83, 0x01, 0x80, 0x00);

    WHEN("On input of 4 bytes of INPUT_DATA to FrameReceiver ") {
        PF::Driver::Serial::Backend::FrameProps::InputStatus input_status;

        for (uint8_t index = 0; index < 5; index++) {
            auto input_status = frame_receiver.input(input_data[index]);
        }
        THEN("input status should be ok") {
            REQUIRE(input_status == PF::Driver::Serial::Backend::FrameProps::InputStatus::ok);
        }
    }

    WHEN("input status is output ready") {
        for (uint8_t index = 0; index < 5; index++) {
            frame_receiver.input(input_data[index]);
        }
        constexpr size_t chunk_max_size = 256;

        PF::Util::ByteVector<chunk_max_size> chunkBuffer;

        auto output_status = frame_receiver.output(chunkBuffer);

        THEN("output status should be ok") {
            REQUIRE(output_status == Pufferfish::Driver::Serial::Backend::FrameProps::OutputStatus::ok);
        }
    }
  }

  GIVEN("A Frame Sender") {
    constexpr size_t chunk_size = 256UL;
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Driver::Serial::Backend::FrameSender frame_sender{};

    auto body = std::string("\x98\xDB\xE3\x55\x01\x05\x01\x02\x03\x04\x05", 11);
    auto expected_output = std::string("\x0C\x98\xDB\xE3\x55\x01\x05\x01\x02\x03\x04\x05\x00", 13);

    WHEN("data is written to it") {
        PF::Util::ByteVector<chunk_size> input_buffer;
        PF::Util::ByteVector<chunk_size> output_buffer;

        PF::Util::convertStringToByteVector(body, input_buffer);

        auto output_status = frame_sender.transform(input_buffer, output_buffer);
        
        auto value = PF::Util::convertByteVectorToHexString(output_buffer);

        THEN("output status should be ok") {
            REQUIRE(output_status == PF::Driver::Serial::Backend::FrameProps::OutputStatus::ok);
            REQUIRE(output_buffer == expected_output);
        }
    }
  }
}