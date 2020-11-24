/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Parse.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: Rohan Purohit
 *
 * Unit tests to confirm behavior of RingBuffer
 *  
 */
#include "Pufferfish/Util/COBS.h"
#include "Pufferfish/Test/Util.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Cobs encode and decode", "[COBS]") {
    GIVEN("A buffer") {
        constexpr size_t buffer_size = 254UL;
        PF::Util::ByteVector<buffer_size> input_buffer;
        PF::Util::ByteVector<buffer_size> output_buffer;

        auto data = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);
        auto expected_output = std::string("\x0C\x98\xDB\xE3\x55\x01\x05\x01\x02\x03\x04\x05", 12);

        PF::Util::convertStringToByteVector(data, input_buffer);

        WHEN("buffer is encoded") {
            auto value = PF::Util::encode_cobs(input_buffer.buffer(), input_buffer.size(), output_buffer.buffer());

            THEN("output should be as expected") {
                REQUIRE(value == expected_output.size());
            }

        }
    }

    GIVEN("An encoded buffer") {
        constexpr size_t buffer_size = 254UL;
        PF::Util::ByteVector<buffer_size> input_buffer;
        PF::Util::ByteVector<buffer_size> output_buffer;
        auto data = std::string("\x0C\x98\xDB\xE3\x55\x01\x05\x01\x02\x03\x04\x05", 12);
        auto expected_output = std::string("\x98\xdb\xe3\x55\x01\x05\x01\x02\x03\x04\x05", 11);

        WHEN("buffer is decoded") {
            PF::Util::convertStringToByteVector(data, input_buffer);

            auto value = PF::Util::decode_cobs(input_buffer.buffer(), input_buffer.size(), output_buffer.buffer());

            THEN("output should be as expected") {
                REQUIRE(value == expected_output.size());
            }
        }
    }
}