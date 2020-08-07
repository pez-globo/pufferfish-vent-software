/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * AlarmsManager.h
 *
 *  Created on: Jun 23, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include "Pufferfish/Statuses.h"
#include "Pufferfish/Types.h"
#include "Pufferfish/Driver/Indicators/AlarmDevice.h"

namespace Pufferfish {

/**
 * Manages and prioritize all alarms raised by the system
 */
class AlarmsManager {
 public:
  /**
   * Constructs a new AlarmsManager object
   * @param led         LED alarm
   * @param auditory    Buzzer and Auditory alarm
   */
  AlarmsManager(Driver::Indicators::AlarmDevice &led, Driver::Indicators::AlarmDevice &auditory)
      :
      mAlarmsCnt{},
      mActive(AlarmStatus::noAlarm),
      mLED(led),
      mAuditory(auditory) {
  }

  /**
   * Adds an alarm to the currently active list of alarms
   * @param a   type of alarm to add
   * @return invalidAlarm if `a` is not a valid alarm, ok otherwise
   */
  AlarmManagerStatus add(AlarmStatus a);

  /**
   * Removes an alarm from the currently active list of alarms
   * @param a   type of alarm to remove
   * @return invalidAlarm if `a` is not a valid alarm,
   *    notSet if `a` is currently not active, ok otherwise
   */
  AlarmManagerStatus remove(AlarmStatus a);

  /**
   * Clears all the active alarm
   */
  void clearAll();

  /**
   * Performs necessary updates on the alarm indicators,
   *    must be called with frequency at least 500 Hz
   * @param currentTime current system time, in ms
   * @return ok if the update is successful, error code otherwise
   */
  AlarmManagerStatus update(uint32_t currentTime);

  /**
   * Get the alarm currently outputted by the system
   * @return the alarm currently displayed
   */
  AlarmStatus getActive();

 private:
  uint32_t mAlarmsCnt[(int) AlarmStatus::noAlarm];
  AlarmStatus mActive;
  bool mUpdated;

  Driver::Indicators::AlarmDevice &mLED;
  Driver::Indicators::AlarmDevice &mAuditory;

  void updateActive();
};

}
