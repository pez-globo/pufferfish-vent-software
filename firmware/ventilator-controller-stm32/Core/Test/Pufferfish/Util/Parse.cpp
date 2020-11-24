/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Parse.cpp
 *
 *  Created on: Nov 23, 2020
 *      Author: Rohan Purohit
 *
 * Unit tests to confirm behavior of RingBuffer
 *
 */
#include "Pufferfish/Util/Parse.h"
#include <array>
#include <iostream>

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("A Buffer is Parsed", "[parse]") {
    GIVEN("A buffer") {
        std::array<uint8_t, 8> buffer{{0x4D, 0x80, 0x8D, 0x2C, 0x12, 0x14, 0x13, 0xA2}};
        WHEN("2 bytes are parsed") {
            auto value = PF::Util::parse_network_order<uint16_t>(buffer.data(), buffer.size());

            THEN("it should return the value in network order") {
                REQUIRE(value == 37983);
            }
        }

        WHEN("4 bytes are parsed") {
            auto value = PF::Util::parse_network_order<uint32_t>(buffer.data(), buffer.size());

            THEN("it should return the value in network order") {
                REQUIRE(value == 3466630239);
            }
        }

        WHEN("8 bytes are parsed") {
            auto value = PF::Util::parse_network_order<uint64_t>(buffer.data(), buffer.size());

            THEN("it should return the value in network order") {
                REQUIRE(value == 18446744072881214559);
            }
        }
    }
}