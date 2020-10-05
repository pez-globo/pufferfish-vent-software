/// TestButton.cpp
/// Unit tests to confirm the behavior of Membrane Button checks.

// Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.

#include "catch2/catch.hpp"
#include "Pufferfish/HAL/Mock/MockDigitalInput.h"
#include "Pufferfish/HAL/Mock/MockTime.h"
#include "Pufferfish/Driver/Button/Button.h"

namespace PF = Pufferfish;

SCENARIO("TESTSCENARIO 1 : Valid debounce time", "[Button]") {
  GIVEN("Switch input state and current time") {
    bool input = false;
    bool output = false;
    uint32_t index = 0;
    uint32_t currentTime = 0;
    PF::Driver::Button::ButtonStatus button_status;
    PF::Driver::Button::Debouncer switch_input;
    WHEN("Switch input state change from low to high") {
      input = true;
      THEN("Integration time is less than max time") {
        for (currentTime = 0; currentTime < 100; currentTime++) {
          currentTime++;
          input = true;
          button_status = switch_input.transform(input, currentTime, output);
          REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
          REQUIRE(output == false);
        }
      }
    }
    AND_WHEN("Switch input state change from low to high and Integration is equal to max time") {
      input = true;
      currentTime = 0;
      for (uint32_t currentTime = 0; currentTime < 100; currentTime++) {
        button_status = switch_input.transform(input, currentTime, output);
      }
      REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
      REQUIRE(output == false);
      THEN("Integration time is equal to or greater than max time") {
        input = true;
        button_status = switch_input.transform(input, currentTime, output);
        REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
        REQUIRE(output == true);
      }
    }
    AND_WHEN("Switch input state change from low to high and again changed from high to low") {
      input = true;
      for (uint32_t currentTime = 0; currentTime <= 100; currentTime++) {
        button_status = switch_input.transform(input, currentTime, output);
      }
      REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
      REQUIRE(output == true);
      THEN("Input changed from active high to low and max integration time is greater than Time") {
        input = false;
        for (uint32_t currentTime = 101; currentTime < 150; currentTime++) {
          button_status = switch_input.transform(input, currentTime, output);
          REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
          REQUIRE(output == true);
        }
      }
    }
    AND_WHEN("State change from low to high and again changed from active low to high") {
      for (uint32_t currentTime = 0; currentTime < 150; currentTime++) {
        if (currentTime <= 100) {
          input = true;
          button_status = switch_input.transform(input, currentTime, output);
        } else {
          input = false;
          button_status = switch_input.transform(input, currentTime, output);
        }
      }
      REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
      REQUIRE(output == true);
      THEN("Switch state from active high to low and integration time is less than max Time") {
        input = true;
        for (uint32_t currentTime = 150; currentTime < 200; currentTime++) {
          button_status = switch_input.transform(input, currentTime, output);
          REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
          REQUIRE(output == true);
        }
      }
    }
  }
}

SCENARIO("Switch is debouncing Continuously more than maximum limit", "[Button]") {
  GIVEN("Switch input state and current time") {
    bool input = false;
    bool output = false;
    uint32_t currentTime = 0;
    uint32_t j = 0;
    PF::Driver::Button::Debouncer switch_input;
    PF::Driver::Button::ButtonStatus button_status;
    WHEN("Switch input state change from low to high continuously") {
      input = true;
      currentTime = 0;
      button_status = switch_input.transform(input, currentTime, output);
      REQUIRE(button_status == PF::Driver::Button::ButtonStatus::not_ok);
      REQUIRE(output == false);
      for (uint32_t i = 1; i < 2000; i++) {
        currentTime++;
        if (i < (j + 51)) {
          input = true;
          button_status = switch_input.transform(input, currentTime, output);
          REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
          REQUIRE(output == false);
        } else if (i >= (j + 51) && i < (j + 100)) {
          input = false;
          button_status = switch_input.transform(input, currentTime, output);
          REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
          REQUIRE(output == false);
        } else if (i == (j + 100)) {
          j = i;
          input = true;
          button_status = switch_input.transform(input, currentTime, output);
          REQUIRE(button_status == PF::Driver::Button::ButtonStatus::ok);
          REQUIRE(output == false);
        }
      }
      THEN("The current time is more than maximum debounce limit") {
        currentTime = 2000;
        input = true;
        button_status = switch_input.transform(input, currentTime, output);
        REQUIRE(button_status == PF::Driver::Button::ButtonStatus::unknown);
        REQUIRE(output == false);
      }
    }
  }
}

SCENARIO("TESTSCENARIO 3 : Valid edge transition ", "[EdgeDetector::transform]") {
  GIVEN("Switch input state") {
    bool input = false;
    PF::Driver::Button::EdgeDetector edgeTransition;
    PF::Driver::Button::EdgeState status;
    WHEN("Input state changed from LOW to HIGH") {
      input = false;
      edgeTransition.transform(input, status);
      THEN("") {
        input = true;
        edgeTransition.transform(input, status);
        REQUIRE(status == PF::Driver::Button::EdgeState::rising_edge);
      }
    }
    WHEN("Input state changed from HIGH to LOW") {
      input = true;
      edgeTransition.transform(input, status);
      THEN("") {
        input = false;
        edgeTransition.transform(input, status);
        REQUIRE(status == PF::Driver::Button::EdgeState::falling_edge);
      }
    }
    WHEN("Input state changed from LOW to LOW") {
      input = false;
      edgeTransition.transform(input, status);
      THEN("") {
        input = false;
        edgeTransition.transform(input, status);
        REQUIRE(status == PF::Driver::Button::EdgeState::no_edge);
      }
    }
    WHEN("Input state changed from HIGH to HIGH") {
      input = false;
      edgeTransition.transform(input, status);
      THEN("") {
        input = false;
        edgeTransition.transform(input, status);
        REQUIRE(status == PF::Driver::Button::EdgeState::no_edge);
      }
    }
  }
}

SCENARIO("TESTSCENARIO 4 : Validate read_state ", "[Button]") {
  GIVEN("Switch debounced output") {
    PF::HAL::MockDigitalInput buttonInput;
    PF::Driver::Button::Debouncer switch_input;
    PF::Driver::Button::ButtonStatus status;
    PF::Driver::Button::EdgeDetector edgeTransition;
    PF::HAL::MockTime timeInput;
    bool debouned_output = false;
    PF::Driver::Button::Button testButton(buttonInput, switch_input, timeInput);
    PF::Driver::Button::EdgeState PrevEdgeState = PF::Driver::Button::EdgeState::no_edge;
    PF::Driver::Button::EdgeState testEdgeState = PF::Driver::Button::EdgeState::no_edge;
    WHEN("Current time is less than the last sample time") {
      buttonInput.set_read(true);
      timeInput.set_millis(0);
      status = testButton.read_state(debouned_output, testEdgeState);
      THEN("The output of the Edge transition same as previous") {
        REQUIRE(status == PF::Driver::Button::ButtonStatus::not_ok);
        REQUIRE(PrevEdgeState == testEdgeState);
      }
    }
    WHEN("Debounced_time is greater than max_integrator_samples on push") {
      buttonInput.set_read(false);
      for (uint32_t i = 1; i < 101; i++) {
        buttonInput.set_read(true);
        timeInput.set_millis(i);
        status = testButton.read_state(debouned_output, testEdgeState);
      }
      THEN("The risigning edge state detected") {
        REQUIRE(status == PF::Driver::Button::ButtonStatus::ok);
        REQUIRE(testEdgeState == PF::Driver::Button::EdgeState::rising_edge);
      }
    }
    WHEN("Debounced time is greater than the max_integrator_samples on Release") {
      buttonInput.set_read(false);
      for (uint32_t i = 1; i < 101; i++) {
        buttonInput.set_read(true);
        timeInput.set_millis(i);
        status = testButton.read_state(debouned_output, testEdgeState);
      }
      THEN("The risigning edge state detected") {
        REQUIRE(status == PF::Driver::Button::ButtonStatus::ok);
        REQUIRE(testEdgeState == PF::Driver::Button::EdgeState::rising_edge);
      }
      for (uint32_t i = 101; i < 201; i++) {
        buttonInput.set_read(false);
        timeInput.set_millis(i);
        status = testButton.read_state(debouned_output, testEdgeState);
      }
      THEN("The falling edge state detected") {
        REQUIRE(status == PF::Driver::Button::ButtonStatus::ok);
        REQUIRE(testEdgeState == PF::Driver::Button::EdgeState::falling_edge);
      }
    }
  }
}
