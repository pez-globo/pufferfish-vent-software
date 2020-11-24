/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Vector.cpp
 *
 *  Created on: Nov 23, 2020
 *      Author: Rohan Purohit
 *
 * Unit tests to confirm behavior of Vector
 *
 */
#include "Pufferfish/Util/Vector.h"
#include <iostream>

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Vector", "[vector]") {

    GIVEN("A vector") {
        int n = 5;
        std::vector<int> vector(n, 10);

        WHEN("empty vector is given") {
            std::vector<int> empty_vector;
            THEN("check if its empty") {
                REQUIRE(empty_vector.empty() == true);                
            }
        }
    }

    GIVEN("A vector") {
        constexpr size_t buffer_size = 254UL;
        PF::Util::ByteVector<buffer_size> input_buffer;
        auto data = std::string("\x98\xdb\xe3\x55");

        for (auto& ch : data) {
            input_buffer.push_back(ch);
        }

        WHEN("vector is given") {
            constexpr size_t new_size = 252UL;
            auto status = input_buffer.resize(new_size);
            THEN("resize works") {
                REQUIRE(status == Pufferfish::IndexStatus::ok);
                REQUIRE(input_buffer.size() == new_size);
            }
        }

        WHEN("new size greater") {
            constexpr size_t new_size = 256UL;
            auto status = input_buffer.resize(new_size);
            THEN("status is out of bounds") {
                REQUIRE(status == Pufferfish::IndexStatus::out_of_bounds);
            }
        }
    }

    GIVEN("A vector") {
        int n = 5;
        std::vector<int> vector(n, 10);

        WHEN("data is inserted to the end by push back") {
            vector.push_back(10);

            THEN("both should be equal") {
                REQUIRE(vector[5] == 10);
            }
        }
    }

}
