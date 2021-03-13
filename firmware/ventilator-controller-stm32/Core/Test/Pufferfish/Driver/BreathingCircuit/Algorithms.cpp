/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Algorithms.cpp
 *
 *  Created on: Nov 19, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of BreathingCircuit Algorithms
 *
 */

#include "Pufferfish/Driver/BreathingCircuit/Algorithms.h"

#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("BreathingCircuit::PI behaves correctly", "[Algorithms]") {
  GIVEN("A PI object") {
    PF::Driver::BreathingCircuit::PI valve_test;

    WHEN("the current time is written to it") {
      auto measurement = GENERATE(0.1, 0.2, 0.3);
      auto setpoint = GENERATE(0.1, 0.2, 0.3);
      float actuation = 0;
      valve_test.transform(measurement, setpoint, actuation);

      THEN("the final time should be the same") {
        REQUIRE(actuation == Approx(0));
      }
    }

    WHEN("actuation is greater than 1") {
      auto measurement = GENERATE(0.1, 0.2, 0.3);
      auto setpoint = GENERATE(0.1, 0.2, 0.3);
      float actuation = 0;
      valve_test.transform(measurement, setpoint, actuation);
    }
  }
}
