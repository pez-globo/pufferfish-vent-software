/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * PulseGenerator.cpp
 *
 *  Created on: July 14, 2020
 *      Author: Ethan Li
 *
 * Unit tests to confirm behavior of pulse generation
 *
 */

#include "Pufferfish/Driver/Indicators/PulseGenerator.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("PWMGenerator generates a PWM signal", "[PulseGenerator]") {
  GIVEN("A PWMGenerator") {
    uint32_t pulse_period = 10;
    uint32_t pulse_width = 1;
    PF::Driver::Indicators::PWMGenerator pwm(pulse_period, pulse_width);
    REQUIRE(!pwm.output());

    WHEN("generation starts at 0") {
      uint8_t start_time = 128;
      pwm.start(start_time);

      THEN("signal starts with a rising edge") { REQUIRE(pwm.output()); }

      AND_THEN("signal stays constant until clock advances") {
        REQUIRE(pwm.output());
        pwm.update(start_time);
        REQUIRE(pwm.output());
      }

      AND_THEN("falling edge occurs") {
        pwm.update(start_time + pulse_width);
        REQUIRE(!pwm.output());
      }

      // FIXME: the remainder of this unit test needs to be implemented!
    }
  }
}
