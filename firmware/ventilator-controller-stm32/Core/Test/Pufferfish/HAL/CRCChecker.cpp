/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Timeouts.cpp
 *
 *  Created on: July 14, 2020
 *      Author: Ethan Li
 *
 * Unit tests to confirm behavior of timeout checks
 *
 */

#include "Pufferfish/HAL/CRCChecker.h"

#include "Pufferfish/Util/Array.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("reflect should correctly reflect inputs", "[crc]") {
  GIVEN("uint8_t inputs") {
    WHEN("0 is input") {
      uint8_t input = 0b00000000;

      THEN("the reflection is correct") {
        uint8_t expected = 0b00000000;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("1 is input") {
      uint8_t input = 0b00000001;

      THEN("the reflection is correct") {
        uint8_t expected = 0b10000000;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("The reflection of 1 is input") {
      uint8_t input = 0b10000000;

      THEN("the reflection is correct") {
        uint8_t expected = 0b00000001;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("0b00100101 is input") {
      uint8_t input = 0b00100101;

      THEN("the reflection is correct") {
        uint8_t expected = 0b10100100;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("The reflection of 0b10100100 is input") {
      uint8_t input = 0b10100100;

      THEN("the reflection is correct") {
        uint8_t expected = 0b00100101;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }
  }
  GIVEN("uint32_t inputs") {
    WHEN("0 is input") {
      uint32_t input = 0b00000000000000000000000000000000;

      THEN("the reflection is correct") {
        uint32_t expected = 0b00000000000000000000000000000000;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("1 is input") {
      uint32_t input = 0b00000000000000000000000000000001;

      THEN("the reflection is correct") {
        uint32_t expected = 0b10000000000000000000000000000000;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("The reflection of 1 is input") {
      uint32_t input = 0b10000000000000000000000000000000;

      THEN("the reflection is correct") {
        uint32_t expected = 0b00000000000000000000000000000001;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("0b00100101 is input") {
      uint32_t input = 0b00000000000000000000000000100101;

      THEN("the reflection is correct") {
        uint32_t expected = 0b10100100000000000000000000000000;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("The reflection of 0b10100100 is input") {
      uint32_t input = 0b10100100000000000000000000000000;

      THEN("the reflection is correct") {
        uint32_t expected = 0b00000000000000000000000000100101;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("0b00001000000100000100001000100101 is input") {
      uint32_t input = 0b00001000000100000100001000100101;

      THEN("the reflection is correct") {
        uint32_t expected = 0b10100100010000100000100000010000;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }

    WHEN("The reflection of 0b00001000000100000100001000100101 is input") {
      uint32_t input = 0b10100100010000100000100000010000;

      THEN("the reflection is correct") {
        uint32_t expected = 0b00001000000100000100001000100101;
        REQUIRE(PF::HAL::reflect(input) == expected);
      }
    }
  }
}

SCENARIO("CRC8 should obtain correct checksums on test inputs", "[crc]") {
  GIVEN("The software CRC8 implementation with SFM3019 parameters") {
    PF::HAL::SoftCRC8 checker(0x31, 0xff, false, false, 0x00);

    WHEN("an empty sequence is input") {
      std::array<uint8_t, 0> input{};

      THEN("the checksum is correct") {
        uint8_t expected = 0xff;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }

    WHEN("the null byte is input") {
      std::array<uint8_t, 1> input = {{0x00}};

      THEN("the checksum is correct") {
        uint8_t expected = 0xac;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }

    WHEN("the one byte is input") {
      std::array<uint8_t, 1> input = {{0x01}};

      THEN("the checksum is correct") {
        uint8_t expected = 0x9d;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }

    WHEN("the standard test sequence is input") {
      auto input = PF::Util::make_array<uint8_t>(
          static_cast<uint8_t>('1'),
          static_cast<uint8_t>('2'),
          static_cast<uint8_t>('3'),
          static_cast<uint8_t>('4'),
          static_cast<uint8_t>('5'),
          static_cast<uint8_t>('6'),
          static_cast<uint8_t>('7'),
          static_cast<uint8_t>('8'),
          static_cast<uint8_t>('9'));

      THEN("the checksum is correct") {
        uint8_t expected = 0xf7;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }
  }
}

SCENARIO("CRC32C should obtain correct checksums on test inputs", "[crc]") {
  GIVEN("The software CRC32C implementation") {
    PF::HAL::SoftCRC32 checker(PF::HAL::crc32c_params);

    WHEN("an empty sequence is input") {
      std::array<uint8_t, 0> input{};

      THEN("the checksum is correct") {
        uint32_t expected = 0x00;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }

    WHEN("the null byte is input") {
      std::array<uint8_t, 1> input = {{0x00}};

      THEN("the checksum is correct") {
        uint32_t expected = 0x527d5351;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }

    WHEN("the one byte is input") {
      std::array<uint8_t, 1> input = {{0x01}};

      THEN("the checksum is correct") {
        uint32_t expected = 0xa016d052;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }

    WHEN("the standard test sequence is input") {
      auto input = PF::Util::make_array<uint8_t>(
          static_cast<uint8_t>('1'),
          static_cast<uint8_t>('2'),
          static_cast<uint8_t>('3'),
          static_cast<uint8_t>('4'),
          static_cast<uint8_t>('5'),
          static_cast<uint8_t>('6'),
          static_cast<uint8_t>('7'),
          static_cast<uint8_t>('8'),
          static_cast<uint8_t>('9'));

      THEN("the checksum is correct") {
        uint32_t expected = 0xe3069283;
        REQUIRE(checker.compute(input.data(), input.size()) == expected);
      }
    }
  }
}
