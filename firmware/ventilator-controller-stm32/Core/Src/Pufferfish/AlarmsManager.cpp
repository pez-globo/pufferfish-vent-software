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

AlarmManagerStatus Pufferfish::AlarmsManager::add(AlarmStatus a) {
  const int ind = static_cast<int>(a);

  if (ind < 0 || ind >= static_cast<int>(AlarmStatus::no_alarm)) {
    return AlarmManagerStatus::invalid_alarm;
  }

  alarms_cnt_[ind]++;
  this->update_active();

  return AlarmManagerStatus::ok;
}

AlarmManagerStatus Pufferfish::AlarmsManager::remove(AlarmStatus a) {
  const int ind = static_cast<int>(a);

  if (ind < 0 || ind >= static_cast<int>(AlarmStatus::no_alarm)) {
    return AlarmManagerStatus::invalid_alarm;
  }

  if (alarms_cnt_[ind] == 0) {
    return AlarmManagerStatus::not_set;
  }

  alarms_cnt_[ind]--;
  this->update_active();

  return AlarmManagerStatus::ok;
}

void AlarmsManager::clear_all() {
  for (int i = 0; i < static_cast<int>(AlarmStatus::no_alarm); i++) {
    alarms_cnt_[i] = 0;
  }

  this->update_active();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static,misc-unused-parameters)
AlarmManagerStatus Pufferfish::AlarmsManager::update(uint32_t current_time) {
  AlarmManagerStatus led_stat = led_.update(current_time);
  AlarmManagerStatus audi_stat = auditory_.update(current_time);

  if (led_stat != AlarmManagerStatus::ok) {
    return led_stat;
  }

  if (audi_stat != AlarmManagerStatus::ok) {
    return audi_stat;
  }

  return AlarmManagerStatus::ok;
}

AlarmStatus Pufferfish::AlarmsManager::get_active() {
  return active_;
}

void Pufferfish::AlarmsManager::update_active() {
  for (int i = 0; i < static_cast<int>(AlarmStatus::no_alarm); i++) {
    if (alarms_cnt_[i] > 0) {
      if (static_cast<AlarmStatus>(i) != active_) {
        led_.set_alarm(static_cast<AlarmStatus>(i));
        auditory_.set_alarm(static_cast<AlarmStatus>(i));
      }

      active_ = static_cast<AlarmStatus>(i);
      return;
    }
  }

  led_.set_alarm(AlarmStatus::no_alarm);
  auditory_.set_alarm(AlarmStatus::no_alarm);
  active_ = AlarmStatus::no_alarm;
}

}  // namespace Pufferfish
