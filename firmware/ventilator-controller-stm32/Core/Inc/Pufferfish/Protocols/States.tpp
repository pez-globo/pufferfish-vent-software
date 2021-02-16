/*
 * Application.tpp
 *
 *  Created on: May 26, 2020
 *      Author: Ethan Li
 */

#pragma once

#include "Pufferfish/Util/Timeouts.h"
#include "States.h"

namespace Pufferfish::Protocols {

// StateSynchronizer

template <typename States, typename StateSegment, typename MessageTypes, size_t schedule_size>
void StateSynchronizer<States, StateSegment, MessageTypes, schedule_size>::input(uint32_t time) {
  current_time_ = time;
}

template <typename States, typename StateSegment, typename MessageTypes, size_t schedule_size>
typename StateSynchronizer<States, StateSegment, MessageTypes, schedule_size>::OutputStatus
StateSynchronizer<States, StateSegment, MessageTypes, schedule_size>::output(StateSegment &output) {
  if (!should_output()) {
    return OutputStatus::waiting;
  }

  if (all_states_.output(output_schedule_[current_schedule_entry_].type, output) !=
      States::OutputStatus::ok) {
    return OutputStatus::invalid_type;
  }
  current_schedule_entry_ = (current_schedule_entry_ + 1) % output_schedule_.size();
  current_schedule_entry_start_time_ = current_time_;
  return OutputStatus::ok;
}

template <typename States, typename StateSegment, typename MessageTypes, size_t schedule_size>
bool StateSynchronizer<States, StateSegment, MessageTypes, schedule_size>::should_output() const {
  return !Util::within_timeout(
      current_schedule_entry_start_time_,
      output_schedule_[current_schedule_entry_].delay,
      current_time_);
}

}  // namespace Pufferfish::Protocols
