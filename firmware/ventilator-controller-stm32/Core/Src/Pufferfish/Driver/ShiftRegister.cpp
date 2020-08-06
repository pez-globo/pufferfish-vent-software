/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * ShiftRegister.h
 *
 *  Created on: Jun 31, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/Driver/ShiftRegister.h"

namespace Pufferfish {
namespace Driver {

void ShiftRegister::setChannel(uint8_t chan, bool out) {
  if (out) {
    mOutputReg |= (1u << chan);
  } else {
    mOutputReg &= ~(1u << chan);
  }
}

bool ShiftRegister::getChannel(uint8_t chan) const {
  return mOutputReg & (1u << chan);
}

void ShiftRegister::update() {
  // reset all pins to initial state
  mSerialIn.write(false);
  mSerialClear.write(false);
  mSerialClock.write(false);
  mRClock.write(false);
  HAL::delayMicros(ShiftRegister::BaudRate);

  for (int i = 7; i >= 0; i--) {
    mSerialIn.write(getChannel(i));
    HAL::delayMicros(ShiftRegister::BaudRate);
    mSerialClock.write(true);
    HAL::delayMicros(ShiftRegister::BaudRate);
    mSerialClock.write(false);
    HAL::delayMicros(ShiftRegister::BaudRate);
  }

  // set RClock to display all the LED at once
  mRClock.write(true);
  HAL::delayMicros(ShiftRegister::BaudRate);
  mRClock.write(false);
}

void ShiftRegister::clear() {
  mSerialClear.write(true);
  HAL::delayMicros(ShiftRegister::BaudRate);
  mSerialClear.write(false);
}

}  // namespace HAL
}  // namespace Pufferfish
