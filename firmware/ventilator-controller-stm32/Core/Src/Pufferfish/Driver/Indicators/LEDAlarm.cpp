/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * LEDAlarm.cpp
 *
 *  Created on: Jun 23, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/Indicators/LEDAlarm.h"

namespace Pufferfish::Driver::Indicators {

AlarmManagerStatus LEDAlarm::update(uint32_t current_time) {
  // [0, mPeriod / 2) -> on, [mPeriod / 2, mPeriod) -> off
  uint32_t blink_duration = current_time - last_cycle_;

  // trimming the duration to be within [0, mPeriod)
  if (blink_duration > parameters_.period || reset_) {
    // NOTE: this is only accurate in the case that update() is being called
    //  fast enough (> 1 kHz). However, it guarantees that the "on" time will
    //  always be >= mPeriod / 2
    blink_duration = 0;
    last_cycle_ = current_time;
    reset_ = false;
  }

  if (blink_duration < (parameters_.period / 2) || parameters_.period == 0) {
    // on
    red_.write(parameters_.out_red);
    green_.write(parameters_.out_green);
    blue_.write(parameters_.out_blue);
  } else {
    // off
    red_.write(false);
    green_.write(false);
    blue_.write(false);
  }

  return AlarmManagerStatus::ok;
}

AlarmManagerStatus LEDAlarm::set_alarm(AlarmStatus a) {
  static constexpr Parameters high_priority{true, false, false, 476};
  static constexpr Parameters medium_priority{true, true, false, 16667};
  static constexpr Parameters low_priority{false, true, true, 0};
  static constexpr Parameters technical1{true, false, true, 2000};
  static constexpr Parameters technical2{false, true, false, 4000};
  static constexpr Parameters no_alarm{false, false, false, 0};

  reset_ = true;

  switch (a) {
    case AlarmStatus::high_priority:
      // red
      // 476 ms == 2.1 Hz
      parameters_ = high_priority;
      break;
    case AlarmStatus::medium_priority:
      // red + green = yellow
      // 1667 ms == 0.6 Hz
      parameters_ = medium_priority;
      break;
    case AlarmStatus::low_priority:
      // green + blue = cyan
      parameters_ = low_priority;
      break;
    case AlarmStatus::technical1:
      // blue + red = purple
      // 2000 ms == 0.5 Hz
      parameters_ = technical1;
      break;
    case AlarmStatus::technical2:
      // green
      // 4000 ms == 0.25 Hz
      parameters_ = technical2;
      break;
    case AlarmStatus::no_alarm:
      parameters_ = no_alarm;
      break;
    default:
      return AlarmManagerStatus::invalid_alarm;
  }

  return AlarmManagerStatus::ok;
}

}  // namespace Pufferfish::Driver::Indicators
