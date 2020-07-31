/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * LEDAlarm.h
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
 * Drives a regulatory RGB LED indicator
 */
class LEDAlarm : public AlarmDevice {
 public:
  /**
   * Constructs a new LEDAlarm object
   * @param red     output for the red LED
   * @param green   output for the green LED
   * @param blue    output for the blue LED
   */
  LEDAlarm(HAL::DigitalOutput &red, HAL::DigitalOutput &green, HAL::DigitalOutput &blue)
      : mRed(red), mGreen(green), mBlue(blue) {}

  AlarmManagerStatus update(uint32_t currentTime) override;
  AlarmManagerStatus setAlarm(AlarmStatus a) override;
 private:
  HAL::DigitalOutput &mRed;
  HAL::DigitalOutput &mGreen;
  HAL::DigitalOutput &mBlue;

  bool mReset = false;
  bool mOutRed = false, mOutGreen = false, mOutBlue = false;
  uint32_t mPeriod = 0;
  uint32_t mLastCycle = 0;
};

}  // namespace Indicators
}  // namespace Driver
}  // namespace Pufferfish
