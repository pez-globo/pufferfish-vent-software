/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * AlarmDevice.h
 *
 *  Created on: Jul 9, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/Types.h"
#include "Pufferfish/Statuses.h"

namespace Pufferfish {
namespace Driver {
namespace Indicators {

/**
 * An abstract class for a alarm device
 */
class AlarmDevice {
 public:
  /**
   * Performs necessary updates on the alarm output,
   *    must be called with frequency at least 500 Hz
   * @param currentTime current system time, in ms
   * @return ok if the update is successful, error code otherwise
   */
  virtual AlarmManagerStatus update(uint32_t currentTime) = 0;

  /**
   * Sets the alarm to be output by the device
   * @param a   an alarm to be output by the device
   * @return invalidAlarm if `a` is not a valid alarm, ok otherwise
   */
  virtual AlarmManagerStatus setAlarm(AlarmStatus a) = 0;
};

}  // namespace Indicators
}  // namespace Driver
}  // namespace Pufferfish
