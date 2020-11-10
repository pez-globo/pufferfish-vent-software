/*
 * BreathingCircuit.tpp
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 */

#include "Pufferfish/Driver/BreathingCircuit/Algorithms.h"

namespace Pufferfish::Driver::BreathingCircuit {

// PI

void PI::transform(float measurement, float setpoint, float &actuation) {
  error_ = setpoint - measurement;
  error_integral_ += error_;
  if (error_integral_ < i_min) {
    error_integral_ = i_min;
  }
  if (error_integral_ > i_max) {
    error_integral_ = i_max;
  }

  actuation = error_ * p_gain + error_integral_ * i_gain;
  if (actuation < out_min) {
    actuation = out_min;
  }
  if (actuation > out_max) {
    actuation = out_max;
  }
}

}  // namespace Pufferfish::Driver::BreathingCircuit
