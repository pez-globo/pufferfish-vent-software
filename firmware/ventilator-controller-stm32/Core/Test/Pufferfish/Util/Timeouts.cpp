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

#include "Pufferfish/Util/Timeouts.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Timeouts fail across rollover for timestamp checks", "[timeouts]") {
  GIVEN("A timeout") {
    uint8_t timeout = 10;

    WHEN("the start time is small") {
      uint8_t start_time = 128;
      uint8_t deadline = start_time + timeout;
      REQUIRE(deadline == 138);

      THEN("timestamp checks succeed within timeout") {
        uint8_t end_time = 137;
        REQUIRE(end_time < deadline);
      }

      THEN("timestamp checks succeed outside timeout") {
        uint8_t end_time = 139;
        REQUIRE(end_time > deadline);
      }
    }

    WHEN("the start time is large") {
      uint8_t start_time = 250;
      uint8_t deadline = start_time + timeout;
      REQUIRE(deadline == 4);

      THEN("timestamp checks may fail within timeout") {
        uint8_t end_time = 251;
        REQUIRE(!(end_time < deadline));
      }

      THEN("timestamp checks succeed outside timeout") {
        auto end_time = static_cast<uint8_t>(261);
        REQUIRE(end_time == 5);
        REQUIRE(end_time > deadline);
      }
    }
  }
}

SCENARIO("Timeouts succeed across rollover for duration checks", "[timeouts]") {
  GIVEN("A timeout") {
    uint8_t timeout = 10;

    WHEN("the start time is small") {
      uint8_t start_time = 128;

      THEN("duration checks succeed within timeout") {
        uint8_t end_time = 137;
        uint8_t duration = end_time - start_time;
        REQUIRE(duration == 9);
        REQUIRE(duration < timeout);
      }

      THEN("duration checks succeed outside timeout") {
        uint8_t end_time = 139;
        uint8_t duration = end_time - start_time;
        REQUIRE(duration == 11);
        REQUIRE(duration > timeout);
      }
    }

    WHEN("the start time is large") {
      uint8_t start_time = 250;

      THEN("duration checks succeed within timeout before rollover") {
        uint8_t end_time = 251;
        uint8_t duration = end_time - start_time;
        REQUIRE(duration == 1);
        REQUIRE(duration < timeout);
      }

      THEN("duration checks succeed within timeout after rollover") {
        auto end_time = static_cast<uint8_t>(257);
        REQUIRE(end_time == 1);
        uint8_t duration = end_time - start_time;
        REQUIRE(duration == 7);
        REQUIRE(duration < timeout);
      }

      THEN("duration checks succeed outside timeout") {
        auto end_time = static_cast<uint8_t>(261);
        REQUIRE(end_time == 5);
        uint8_t duration = end_time - start_time;
        REQUIRE(duration == 11);
        REQUIRE(duration > timeout);
      }
    }
  }
}

SCENARIO("Util::within_timeout behaves correctly across rollover", "[timeouts]") {
  GIVEN("A timeout") {
    uint8_t timeout = 10;

    WHEN("the start time is small") {
      uint8_t start_time = 128;

      THEN("timeout checks succeed within timeout") {
        uint8_t end_time = 137;
        REQUIRE(PF::Util::within_timeout(start_time, timeout, end_time));
      }

      THEN("timeout checks succeed outside timeout") {
        uint8_t end_time = 139;
        REQUIRE(!PF::Util::within_timeout(start_time, timeout, end_time));
      }
    }

    WHEN("the start time is large") {
      uint8_t start_time = 250;

      THEN("timeout checks succeed within timeout before rollover") {
        uint8_t end_time = 251;
        REQUIRE(PF::Util::within_timeout(start_time, timeout, end_time));
      }

      THEN("timeout checks succeed within timeout after rollover") {
        auto end_time = static_cast<uint8_t>(257);
        REQUIRE(end_time == 1);
        REQUIRE(PF::Util::within_timeout(start_time, timeout, end_time));
      }

      THEN("timeout checks succeed outside timeout") {
        auto end_time = static_cast<uint8_t>(261);
        REQUIRE(end_time == 5);
        REQUIRE(!PF::Util::within_timeout(start_time, timeout, end_time));
      }
    }
  }
}
