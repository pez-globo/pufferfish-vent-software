/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * AlarmsManager.cpp
 *
 *  Created on: Jun 23, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/AlarmsManager.h"

namespace Pufferfish {

AlarmManagerStatus AlarmsManager::add(AlarmStatus a) {
  int ind = static_cast<int>(a);

  if (ind < 0 || ind >= static_cast<int>(AlarmStatus::noAlarm)) {
    return AlarmManagerStatus::invalidAlarm;
  }

  mAlarmsCnt[ind]++;
  this->updateActive();

  return AlarmManagerStatus::ok;
}

AlarmManagerStatus AlarmsManager::remove(AlarmStatus a) {
  int ind = static_cast<int>(a);

  if (ind < 0 || ind >= static_cast<int>(AlarmStatus::noAlarm)) {
    return AlarmManagerStatus::invalidAlarm;
  }

  if (mAlarmsCnt[ind] == 0) {
    return AlarmManagerStatus::notSet;
  }

  mAlarmsCnt[ind]--;
  this->updateActive();

  return AlarmManagerStatus::ok;
}

void AlarmsManager::clearAll() {
  for (int i = 0; i < static_cast<int>(AlarmStatus::noAlarm); i++) {
    mAlarmsCnt[i] = 0;
  }

  this->updateActive();
}

AlarmManagerStatus AlarmsManager::update(uint32_t currentTime) {
  AlarmManagerStatus led_stat = mLED.update(currentTime);
  AlarmManagerStatus audi_stat = mAuditory.update(currentTime);

  if (led_stat != AlarmManagerStatus::ok) {
    return led_stat;
  }

  if (audi_stat != AlarmManagerStatus::ok) {
    return audi_stat;
  }

  return AlarmManagerStatus::ok;
}

AlarmStatus AlarmsManager::getActive() {
  return mActive;
}

void AlarmsManager::updateActive() {
  for (int i = 0; i < static_cast<int>(AlarmStatus::noAlarm); i++) {
    if (mAlarmsCnt[i]) {
      if (static_cast<AlarmStatus>(i) != mActive) {
        mLED.setAlarm(static_cast<AlarmStatus>(i));
        mAuditory.setAlarm(static_cast<AlarmStatus>(i));
      }

      mActive = static_cast<AlarmStatus>(i);
      return;
    }
  }

  mLED.setAlarm(AlarmStatus::noAlarm);
  mAuditory.setAlarm(AlarmStatus::noAlarm);
  mActive = AlarmStatus::noAlarm;
}

}
