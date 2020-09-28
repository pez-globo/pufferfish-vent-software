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
#include "Pufferfish/Statuses.h"
#include "Pufferfish/HAL/STM32/Time.h"

namespace Pufferfish {
namespace Driver {
namespace Button {

/**
 * ButtonStatus enum class contains status of the button states
 */
enum class ButtonStatus{
  ok = 0, /// Ok if debounce is success
  notOk, /// notOk if current time exceeds sampling period
  unKnown /// Fault, if input state debouncing more the maximum debounce time limit
};

/**
 * EdgeState enum class contains state transition when button pressed
 */
enum class EdgeState{
    noEdge = 0, /// Button is in stable state
    risingEdge, /// Button triggered on rising edge
    fallingEdge /// Button triggered on falling edge
};

/**
 * Abstract class for Debounce calculation
 */
class Debouncer
{

public:

  Debouncer(){
  }
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
  bool timeValidCheck(uint32_t nowTime, uint32_t lastTime, uint32_t addFactor);
      static const uint32_t debounceTimeLimit =  2000;
      uint32_t samplingPeriod = 1;
      uint32_t lastSampleTime;
      uint8_t integrator;
      uint32_t lastTimeStable;
      const uint8_t maxIntegratorSamples = 100;
      bool mOutput;

};

/**
 * Abstract class for Edge state transition
 */
class EdgeDetector
{

public:

  EdgeDetector(){
  }
  /**
   * Checking switch state transition
   * @param state debounced output
   * @return rising edge on Low to High or falling edge on High to Low
   */
  void transform(bool input, EdgeState &output);
private:
  bool lastState;

};

/**
 * Abstract class for Membrane button input
 */
class Button
{
public:

  Button(HAL::DigitalInput &buttoninput, Debouncer &debounce)
    : mButtoninput(buttoninput),
      mDebounce(debounce) {
  }

  /**
   * Read button state
   * @param debounedOutput debounced output state
   * @param EdgeState rising edge on Low to High or falling edge on High to Low
   * @return rising edge on Low to High or falling edge on High to Low
   */
  ButtonStatus readState(bool &debounedOutput, EdgeState &switchStateChanged);

private:

  HAL::DigitalInput &mButtoninput;
  Debouncer &mDebounce;
  EdgeDetector mEdgeDetect;
};

}  // namespace MembraneButton
}  // namespace HAL
}  // namespace Pufferfish
