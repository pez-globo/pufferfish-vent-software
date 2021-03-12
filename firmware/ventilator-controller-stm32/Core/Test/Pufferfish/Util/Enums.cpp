/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Enums.cpp
 *
 *  Created on: July 14, 2020
 *      Author: Ethan Li
 *
 * Unit tests to confirm behavior of enums utilities
 *
 */
#include "Pufferfish/Util/Enums.h"

#include <algorithm>
#include <limits>

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

enum class TestEnum : uint8_t { zero = 0, two = 2, three = 3, seven = 7 };

SCENARIO("EnumValues includes defined values", "[enums]") {
  GIVEN("An EnumValues specialization covering all of a test enum") {
    using EnumValues = PF::Util::
        EnumValues<TestEnum, TestEnum::zero, TestEnum::two, TestEnum::three, TestEnum::seven>;

    WHEN("EnumValues is queried with an underlying value of the test enum") {
      std::array<uint8_t, 4> values{{0, 2, 3, 7}};

      for (uint8_t value : values) {
        THEN("EnumValues reports that it includes that value") {
          REQUIRE(EnumValues::includes(value));
        }
      }
    }
  }

  GIVEN("An EnumValues specialization covering part of a test enum") {
    using EnumValues = PF::Util::EnumValues<TestEnum, TestEnum::zero, TestEnum::two>;

    WHEN("EnumValues is queried with an underlying value of the test enum covered by EnumValues") {
      std::array<uint8_t, 2> values{{0, 2}};

      for (uint8_t value : values) {
        THEN("EnumValues reports that it includes that value") {
          REQUIRE(EnumValues::includes(value));
        }
      }
    }
  }
}

SCENARIO("EnumValues excludes undefined values", "[enums]") {
  GIVEN("An empty EnumValues specialization") {
    using EnumValues = PF::Util::EnumValues<TestEnum>;

    WHEN("EnumValues is queried with an underlying value of the test enum") {
      std::array<uint8_t, 4> values{{0, 2, 3, 7}};

      for (uint8_t value : values) {
        THEN("EnumValues reports that it does not include that value") {
          REQUIRE(!EnumValues::includes(value));
        }
      }
    }
  }

  GIVEN("An EnumValues specialization covering all of a test enum") {
    using EnumValues = PF::Util::
        EnumValues<TestEnum, TestEnum::zero, TestEnum::two, TestEnum::three, TestEnum::seven>;

    WHEN("EnumValues is queried with any value other than an underlying value of the test enum") {
      std::array<uint8_t, 4> values{{0, 2, 3, 7}};

      for (size_t i = 0; i < std::numeric_limits<uint8_t>::max(); ++i) {
        if (std::find(values.begin(), values.end(), i) == std::end(values)) {
          THEN("EnumValues reports that it does not include that value") {
            REQUIRE(!EnumValues::includes(i));
          }
        }
      }
    }
  }

  GIVEN("An EnumValues specialization covering part of a test enum") {
    using EnumValues = PF::Util::EnumValues<TestEnum, TestEnum::zero, TestEnum::two>;

    WHEN(
        "EnumValues is queried with an underlying value of the test enum not covered by "
        "EnumValues") {
      std::array<uint8_t, 2> values{{3, 7}};

      for (uint8_t value : values) {
        THEN("EnumValues reports that it does not include that value") {
          REQUIRE(!EnumValues::includes(value));
        }
      }
    }
  }
}
