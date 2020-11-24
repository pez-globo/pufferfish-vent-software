/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * RingBuffer.cpp
 *
 *  Created on: Nov 23, 2020
 *      Author: Rohan Purohit
 *
 * Unit tests to confirm behavior of RingBuffer
 *
 */
#include "Pufferfish/Util/RingBuffer.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("RingBuffer", "[ringbuffer]") {

    GIVEN("A Buffer") {
        constexpr uint32_t buffer_size = 32UL;

        PF::Util::RingBuffer<buffer_size> ring_buffer{};
        ring_buffer.write(10);

        WHEN("a buffer is read")  {
            uint8_t value = 0;
            auto status = ring_buffer.read(value);
            THEN("status should be ok") {
                REQUIRE(status == PF::BufferStatus::ok);
                REQUIRE(value == 10);
            }
        }

        WHEN("a buffer is peaked") {
            uint8_t value = 0;
            auto status = ring_buffer.peek(value);

            THEN("status should be ok") {
                REQUIRE(status == PF::BufferStatus::ok);
            }
        }

        WHEN("a buffer is written") {
            uint8_t value = 2;
            auto status = ring_buffer.write(value);
            THEN("status should be ok") {
                REQUIRE(status == PF::BufferStatus::ok);
            }
        }
    }

}
