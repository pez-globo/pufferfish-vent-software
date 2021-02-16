/*
 * Application.h
 *
 *  Created on: May 26, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace Pufferfish::Protocols {

// State Synchronization

template <typename MessageTypes>
struct StateOutputScheduleEntry {
  uint32_t delay;
  MessageTypes type;
};

template <typename MessageTypes, size_t size>
using StateOutputSchedule = std::array<const StateOutputScheduleEntry<MessageTypes>, size>;

template <typename States, typename StateSegment, typename MessageTypes, size_t schedule_size>
class StateSynchronizer {
 public:
  enum class OutputStatus { ok = 0, waiting, invalid_type };

  StateSynchronizer(
      States &all_states, const StateOutputSchedule<MessageTypes, schedule_size> &schedule)
      : all_states_(all_states), output_schedule_(schedule) {}

  void input(uint32_t time);
  OutputStatus output(StateSegment &output);

 private:
  States &all_states_;
  const StateOutputSchedule<MessageTypes, schedule_size> &output_schedule_;
  uint32_t current_time_ = 0;
  size_t current_schedule_entry_ = 0;
  uint32_t current_schedule_entry_start_time_ = 0;

  [[nodiscard]] bool should_output() const;
};

}  // namespace Pufferfish::Protocols

#include "States.tpp"
