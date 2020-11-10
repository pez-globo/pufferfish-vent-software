/*
 * Sensor.h
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 *
 *  High-level measurement driver for the FDO2.
 */

#pragma once

#include <cstddef>

#include "Device.h"
#include "Pufferfish/Driver/Initializable.h"
#include "Pufferfish/HAL/Interfaces/Time.h"
#include "Pufferfish/Types.h"

namespace Pufferfish::Driver::Serial::FDO2 {

class StateMachine {
 public:
  enum class Action {
    request_version,
    check_version,
    start_broadcast,
    check_broadcast,
    wait_measurement
  };

  [[nodiscard]] Action update(uint32_t current_time, bool passed_check = false);

 private:
  static const uint32_t response_timeout = 50;  // ms

  Action next_action_ = Action::request_version;
  uint32_t request_time_ = 0;
  uint32_t current_time_ = 0;

  void start_request();
  [[nodiscard]] bool timed_out() const;
};

/**
 * High-level (stateful) driver for FDO2 sensor
 */
class Sensor : public Initializable {
 public:
  explicit Sensor(Device &device, HAL::Time &time) : device_(device), time_(time) {}

  InitializableState setup() override;
  InitializableState output(uint32_t &po2);

 private:
  using Action = StateMachine::Action;

  static constexpr Responses::Vers expected_vers{8, 1, 341, 15};
  static constexpr Responses::Bcst expected_bcst{Device::broadcast_interval};
  static const size_t max_retries_setup = 100;  // max retries for all setup steps combined

  Device &device_;
  StateMachine fsm_;
  HAL::Time &time_;
  Action next_action_ = Action::request_version;
  size_t retry_count_ = 0;

  bool get_response(CommandTypes type, Response &response);
  InitializableState check_version(uint32_t current_time);
  InitializableState check_broadcast(uint32_t current_time);
};

}  // namespace Pufferfish::Driver::Serial::FDO2
