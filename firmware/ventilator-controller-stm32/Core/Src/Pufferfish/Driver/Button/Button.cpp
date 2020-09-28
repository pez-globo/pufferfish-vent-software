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

namespace Pufferfish {
namespace Driver {
namespace Button {


ButtonStatus Debouncer::transform(bool input, uint32_t currentTime, bool &output)
{
  if(this->timeValidCheck(currentTime, lastSampleTime, samplingPeriod)){
      return ButtonStatus::notOk;
   }

  lastSampleTime = currentTime;

  /**
   * Update the integrator based on the input signal
   */
  if (input == 0)
  {
    if (integrator > 0){
        integrator--;
    }
  }
  else if (integrator < maxIntegratorSamples){
      integrator++;
  }
  /**
   * Update the integrator based on the input signal
   */
  if (integrator == 0){
    mOutput = 0;
    lastTimeStable = currentTime;
  } else if (integrator >= maxIntegratorSamples) {
    mOutput = 1;
    lastTimeStable = currentTime;
    integrator = maxIntegratorSamples;  /* defensive code if integrator got corrupted */
  }
  /**
   * Report switch fault if debounce time exceeds the maximum limit
   */
    if(!this->timeValidCheck(currentTime, lastTimeStable, debounceTimeLimit)){
      return ButtonStatus::notOk;
   }
   output = mOutput;

 return ButtonStatus::ok;
}

void EdgeDetector::transform(bool input, EdgeState &output)
{
  if (input != lastState) {
    /* Update the last state */
    lastState = input;
    /* check for state is changed */
    if (input == true) {
      /* return the EdgeState as rising edge */
      output = EdgeState::risingEdge;
    } else {
      /* return the EdgeState as falling edge */
      output = EdgeState::fallingEdge;
    }
  }else{
    output = EdgeState::noEdge;
  }
}

ButtonStatus Button::readState(bool &debounedOutput, EdgeState &switchStateChanged){

  bool input = mButtoninput.read();
  uint32_t msTime = Pufferfish::HAL::millis();

  ButtonStatus status= mDebounce.transform(input, msTime, debounedOutput);
  
  /* Debounce is not success */
  if(status != ButtonStatus::ok) {
    return status;
  }
   mEdgeDetect.transform(debounedOutput, switchStateChanged);

  return  status;
}

// This method returns
// "true" --> if nowTime - lastTime < addFactor
// "false" --> if nowTime - lastTime >= addFactor
bool Debouncer::timeValidCheck(uint32_t nowTime, uint32_t lastTime, uint32_t addFactor)
{
  if(nowTime - lastTime < addFactor){
    return true;
  }
  return false;
}
}  // namespace Membrane
}  // namespace HAL
}  // namespace Pufferfish




