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
#include "Pufferfish/Util/Array.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Create an array", "[array]") {
    GIVEN("A set of int values") {
        WHEN("An array is created") {
            auto data = PF::Util::make_array<uint8_t>(0x01, 0x81, 0x01, 0x00, 0x83, 0x01, 0x80, 0x01, 0x00, 0x82);
            THEN("check values") {
                REQUIRE(data[0] == 0x01);
            }
        }

        WHEN("An array is created") {
            auto data = PF::Util::make_array<uint8_t>(0x01, 0x81, 0x01, 0x00);
            THEN("size should be equal") {
                REQUIRE(data.size() == 4);
                REQUIRE(data.max_size() == 4);
            }
        }
    }

    GIVEN("A set of strings") {
        auto data = PF::Util::make_array<std::string>("a", "b", "c");

        WHEN("An array is created") {        
            THEN("array is created") {
                REQUIRE(data[0] == "a");
                REQUIRE(data.size() == 3);
                REQUIRE(data.max_size() == 3);
            }
        }
    }

}
