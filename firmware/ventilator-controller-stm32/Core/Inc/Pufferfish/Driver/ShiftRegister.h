/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * ShiftRegister.h
 *
 *  Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {

class ShiftRegister {
 public:
  static const int BaudRate = 20;

  ShiftRegister(HAL::DigitalOutput &serialIn,
                HAL::DigitalOutput &serialClock,
                HAL::DigitalOutput &rClock,
                HAL::DigitalOutput &serialClear)
      : mSerialIn(serialIn),
        mSerialClock(serialClock),
        mRClock(rClock),
        mSerialClear(serialClear),
        mOutputReg(0) {}

  void setChannel(uint8_t chan, bool out);
  bool getChannel(uint8_t chan) const;
  void update();
  void clear();
 private:
  HAL::DigitalOutput &mSerialIn;
  HAL::DigitalOutput &mSerialClock;
  HAL::DigitalOutput &mRClock;
  HAL::DigitalOutput &mSerialClear;

  uint8_t mOutputReg;
};

}  // namespace Driver
}  // namespace Pufferfish

