/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * AuditoryAlarm.cpp
 *
 *  Created on: Jun 23, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/Indicators/AuditoryAlarm.h"

namespace Pufferfish::Driver::Indicators {

AlarmManagerStatus AuditoryAlarm::update(uint32_t current_time) {
  reg_high_.write(parameters_.out_high);
  reg_med_.write(parameters_.out_med);
  reg_low_.write(parameters_.out_low);

  // [0, mBuzzerPulseDuty) -> on, [mBuzzerPulseDuty, mBuzzerPulsePeriod) -> off
  uint32_t pulse_duration = current_time - last_cycle_;

  // trimming the duration to be within [0, mBuzzerPulsePeriod)
  if (pulse_duration >= parameters_.buzzer_pulse_period || reset_) {
    // NOTE: this is only accurate in the case that update() is being called
    //  fast enough (> 1 kHz). However, it guarantees that the "on" time will
    //  always be >= mBuzzerPulseDuty
    pulse_duration = 0;
    last_cycle_ = current_time;
    reset_ = false;
    buzzer_switching_ = true;
  }

  if (pulse_duration < parameters_.buzzer_pulse_duty) {
    // on
    buzzer_.write(buzzer_switching_);
    buzzer_switching_ = !buzzer_switching_;
  } else {
    // off
    buzzer_.write(false);
  }

  return AlarmManagerStatus::ok;
}

AlarmManagerStatus AuditoryAlarm::set_alarm(AlarmStatus a) {
  static constexpr Parameters high_priority{true, true, true, 0, 0};
  static constexpr Parameters medium_priority{false, true, true, 0, 0};
  static constexpr Parameters low_priority{false, false, true, 0, 0};
  static constexpr Parameters technical1{false, false, false, 2000, 200};
  static constexpr Parameters technical2{false, false, false, 4000, 200};
  static constexpr Parameters no_alarm{false, false, false, 0, 0};
  reset_ = true;

  switch (a) {
    case AlarmStatus::high_priority:
      parameters_ = high_priority;
      break;
    case AlarmStatus::medium_priority:
      parameters_ = medium_priority;
      break;
    case AlarmStatus::low_priority:
      parameters_ = low_priority;
      break;
    case AlarmStatus::technical1:
      // technical1 = buzzer at 0.5 Hz with 10% duty cycle
      parameters_ = technical1;
      break;
    case AlarmStatus::technical2:
      // technical2 = buzzer at 0.25 Hz with 5% duty cycle
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
