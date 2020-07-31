/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * AuditoryAlarm.h
 *
 *  Created on: Jun 23, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"
#include "AlarmDevice.h"

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
  AuditoryAlarm(HAL::DigitalOutput &regHigh,
                HAL::DigitalOutput &regMed,
                HAL::DigitalOutput &regLow,
                HAL::DigitalOutput &buzzer)
      :
      mRegHigh(regHigh),
      mRegMed(regMed),
      mRegLow(regLow),
      mBuzzer(buzzer) {
  }

  AlarmManagerStatus update(uint32_t currentTime) override;
  AlarmManagerStatus setAlarm(AlarmStatus a) override;
 private:
  HAL::DigitalOutput &mRegHigh;
  HAL::DigitalOutput &mRegMed;
  HAL::DigitalOutput &mRegLow;
  HAL::DigitalOutput &mBuzzer;

  bool mReset = false;
  bool mOutHigh = false, mOutMed = false, mOutLow = false;
  uint32_t mBuzzerPulsePeriod = 0, mBuzzerPulseDuty = 0;
  bool mBuzzerSwitching = false;
  uint32_t mLastCycle = 0;
};

}  // namespace Indicators
}  // namespace Driver
}  // namespace Pufferfish
