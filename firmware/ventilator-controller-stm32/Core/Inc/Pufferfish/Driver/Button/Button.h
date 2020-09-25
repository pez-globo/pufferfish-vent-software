/// Button.h
/// Methods and classes for membrane buttons debounce calculation

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

#pragma once

#include "Pufferfish/HAL/HAL.h"
#include "Pufferfish/HAL/STM32/Time.h"
#include "Pufferfish/Statuses.h"

namespace Pufferfish {
namespace Driver {
namespace Button {

/**
 * ButtonStatus enum class contains status of the button states
 */
enum class ButtonStatus {
  ok = 0,  /// Ok if debounce is success
  not_ok,  /// notOk if current time exceeds sampling period
  unknown  /// Fault, if input state debouncing more the maximum debounce time limit
};

/**
 * EdgeState enum class contains state transition when button pressed
 */
enum class EdgeState {
  no_edge = 0,  /// Button is in stable state
  rising_edge,  /// Button triggered on rising edge
  falling_edge  /// Button triggered on falling edge
};

/**
 * Abstract class for Debounce calculation
 */
class Debouncer {
 public:
  Debouncer() = default;
  /**
   * Calculate the debounce time for input button
   * @param input button state High or Low
   * @param current_time
   * @param output state of the debounced output
   * @return ok on success, error code otherwise
   */
  ButtonStatus transform(bool input, uint32_t current_time, bool &output);

 private:
  /**
   * Check time overflow conditions
   * @param nowTime debouncer current time
   * @param lastTime debouncer last time
   * @param addFactor factor added to the last time
   * @return true or false
   */
  static bool time_valid_check(uint32_t now_time, uint32_t last_time, uint32_t add_factor);
  static const uint32_t debounce_time_limit = 2000;
  uint32_t sampling_period_ = 1;
  uint32_t last_sample_time_ = 0;
  uint8_t integrator_ = 0;
  uint32_t last_time_stable_ = 0;
  const uint8_t max_integrator_samples = 100;
  bool output_ = false;
};

/**
 * Abstract class for Edge state transition
 */
class EdgeDetector {
 public:
  EdgeDetector() = default;
  /**
   * Checking switch state transition
   * @param state debounced output
   * @return rising edge on Low to High or falling edge on High to Low
   */
  void transform(bool input, EdgeState &output);

 private:
  bool last_state_ = false;
};

/**
 * Abstract class for Membrane button input
 */
class Button {
 public:
  Button(HAL::DigitalInput &buttoninput, Debouncer &debounce)
      : button_input_(buttoninput), debounce_(debounce) {}

  /**
   * Read button state
   * @param debounedOutput debounced output state
   * @param EdgeState rising edge on Low to High or falling edge on High to Low
   * @return rising edge on Low to High or falling edge on High to Low
   */
  ButtonStatus read_state(bool &debouned_output, EdgeState &switch_state_changed);

 private:
  HAL::DigitalInput &button_input_;
  Debouncer &debounce_;
  EdgeDetector edge_detect_;
};

}  // namespace Button
}  // namespace Driver
}  // namespace Pufferfish
