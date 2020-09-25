/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * AuditoryAlarm.h
 *
 *  Created on: Jun 23, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "AlarmDevice.h"
#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {
namespace Indicators {

/**
 * Drives both regulatory and non-regulatory buzzer
 */
class AuditoryAlarm : public AlarmDevice {
 public:
  /**
   * Constructs a new AuditoryAlarm object
   * @param regHigh output for the regulatory high alarm pin
   * @param regMed  output for the regulatory medium alarm pin
   * @param regLow  output for the regulatory low alarm pin
   * @param buzzer  output for the general purpose buzzer pin
   */
  AuditoryAlarm(
      HAL::DigitalOutput &reg_high,
      HAL::DigitalOutput &reg_med,
      HAL::DigitalOutput &reg_low,
      HAL::DigitalOutput &buzzer)
      : reg_high_(reg_high), reg_med_(reg_med), reg_low_(reg_low), buzzer_(buzzer) {}

  AlarmManagerStatus update(uint32_t current_time) override;
  AlarmManagerStatus set_alarm(AlarmStatus a) override;

 private:
  struct Parameters {
    bool out_high;
    bool out_med;
    bool out_low;
    uint32_t buzzer_pulse_period;
    uint32_t buzzer_pulse_duty;
  };

  HAL::DigitalOutput &reg_high_;
  HAL::DigitalOutput &reg_med_;
  HAL::DigitalOutput &reg_low_;
  HAL::DigitalOutput &buzzer_;

  bool reset_ = false;
  Parameters parameters_{false, false, false, 0, 0};
  bool buzzer_switching_ = false;
  uint32_t last_cycle_ = 0;
};

}  // namespace Indicators
}  // namespace Driver
}  // namespace Pufferfish
