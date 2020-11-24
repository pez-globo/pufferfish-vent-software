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
#include <cstddef>
#include "Pufferfish/Util/Bytes.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Get the position of a byte", "[bytes]") {

    GIVEN("A Byte"){
        const uint8_t number = 0x36;

        WHEN("a byte is fetched") {
            int value = PF::Util::get_byte<0>(number);
            THEN("values should be equal") {
                REQUIRE(value == 54);
            }
        }

        WHEN("a byte is set at 1") {
            auto value = PF::Util::set_byte<1, uint16_t>(number);
            THEN("set byte should work") {
                REQUIRE(value == 13824);
            }
        }
    }
}