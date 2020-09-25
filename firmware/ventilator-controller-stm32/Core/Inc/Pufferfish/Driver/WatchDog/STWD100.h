/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * STWD100.h
 *
 *      Author: Laxmanrao R
 *
 *  A driver for the STWD100 External watchdog
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {
namespace WatchDog {

/**
  * An abstract class for STWD100 WatchDog module
  */
class STWD100 {

public:
  /**
    * Constructor for STWD100 external WatchDog driver
    * @param enablePin for STWD100
    * @param inputPin for STWD100
    */
  STWD100 (HAL::DigitalOutput &enablePin, HAL::DigitalOutput &inputPin )
           :mEnablenPin(enablePin),mInputPin (inputPin){
  }

  /**
   * Enable watchdog timer
   */
  void enable(void);

  /**
   * Disable watchdog timer
   */
  void disable(void);

  /**
   * Reset watchdog timer when controller program experienced a freeze or hang
   */
  void toggleInput(void);

private:
  static const uint32_t resetTime = 2; /// TBD: resetTime as to be fixed
  HAL::DigitalOutput &mEnablenPin;
  HAL::DigitalOutput &mInputPin;
};

}  // namespace WatchDog
}  // namespace Driver
}  // namespace Pufferfish


