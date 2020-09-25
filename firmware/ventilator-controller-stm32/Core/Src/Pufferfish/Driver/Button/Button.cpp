/// Button.cpp
/// Methods for membaren buttons debounce calculation
/// The low-level driver for the membrane buttons needs a way to
/// detect when each button has been pressed
/// (consisting of a push down + a release).

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

#include <Pufferfish/Driver/Button/Button.h>

namespace Pufferfish::Driver::Button {

ButtonStatus Debouncer::transform(bool input, uint32_t current_time, bool &output) {
  if (Pufferfish::Driver::Button::Debouncer::time_valid_check(
          current_time, last_sample_time_, sampling_period_)) {
    return ButtonStatus::not_ok;
  }

  last_sample_time_ = current_time;

  /**
   * Update the integrator based on the input signal
   */
  if (static_cast<int>(input) == 0) {
    if (integrator_ > 0) {
      integrator_--;
    }
  } else if (integrator_ < max_integrator_samples) {
    integrator_++;
  }
  /**
   * Update the integrator based on the input signal
   */
  if (integrator_ == 0) {
    output_ = false;
    last_time_stable_ = current_time;
  } else if (integrator_ >= max_integrator_samples) {
    output_ = true;
    last_time_stable_ = current_time;
    integrator_ = max_integrator_samples; /* defensive code if integrator got corrupted */
  }
  /**
   * Report switch fault if debounce time exceeds the maximum limit
   */
  if (!Pufferfish::Driver::Button::Debouncer::time_valid_check(
          current_time, last_time_stable_, debounce_time_limit)) {
    return ButtonStatus::not_ok;
  }
  output = output_;

  return ButtonStatus::ok;
}

void EdgeDetector::transform(bool input, EdgeState &output) {
  if (input != last_state_) {
    /* Update the last state */
    last_state_ = input;
    /* check for state is changed */
    if (input) {
      /* return the EdgeState as rising edge */
      output = EdgeState::rising_edge;
    } else {
      /* return the EdgeState as falling edge */
      output = EdgeState::falling_edge;
    }
  } else {
    output = EdgeState::no_edge;
  }
}

ButtonStatus Button::read_state(bool &debouned_output, EdgeState &switch_state_changed) {
  bool input = button_input_.read();
  uint32_t ms_time = Pufferfish::HAL::millis();

  ButtonStatus status = debounce_.transform(input, ms_time, debouned_output);

  /* Debounce is not success */
  if (status != ButtonStatus::ok) {
    return status;
  }
  edge_detect_.transform(debouned_output, switch_state_changed);

  return status;
}

// This method returns
// "true" --> if nowTime - lastTime < addFactor
// "false" --> if nowTime - lastTime >= addFactor
bool Debouncer::time_valid_check(uint32_t now_time, uint32_t last_time, uint32_t add_factor) {
  return now_time - last_time < add_factor;
}
}  // namespace Pufferfish::Driver::Button
