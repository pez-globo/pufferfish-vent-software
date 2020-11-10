/*
 * Algorithms.h
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 *
 *  Control algorithms
 */

#pragma once

#include <cstdint>

namespace Pufferfish::Driver::BreathingCircuit {

class PI {
 public:
  void transform(float measurement, float setpoint, float &actuation);

 private:
  static constexpr float out_max = 1;
  static constexpr float out_min = 0;
  static constexpr float p_gain = 0.00001;
  static constexpr float i_gain = 0.0002;
  static constexpr float i_max = out_max / i_gain;
  static constexpr float i_min = out_min / i_gain;

  float error_ = 0;
  float error_integral_ = 0;
};

}  // namespace Pufferfish::Driver::BreathingCircuit
