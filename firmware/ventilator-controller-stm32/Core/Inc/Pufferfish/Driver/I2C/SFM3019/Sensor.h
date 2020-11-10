/*
 * SFM3019.h
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 *
 *  High-level measurement driver for the SFM3019.
 */

#pragma once

#include <cstddef>

#include "Device.h"
#include "Pufferfish/Driver/Initializable.h"
#include "Pufferfish/HAL/Interfaces/Time.h"
#include "Pufferfish/Types.h"

namespace Pufferfish::Driver::I2C::SFM3019 {

/**
 * State management for Sensirion SFM3019 flow sensor, without I/O.
 * This is basically a Moore machine, so the state consists of the
 * next action to take, along with the current time.
 */
class StateMachine {
 public:
  enum class Action { initialize, wait_warmup, check_range, measure, wait_measurement };

  [[nodiscard]] Action update(uint32_t current_time_us);

 private:
  static const uint32_t warming_up_duration_us = 30000;  // us
  static const uint32_t measuring_duration_us = 500;     // us

  Action next_action_ = Action::initialize;
  uint32_t wait_start_time_us_ = 0;
  uint32_t current_time_us_ = 0;

  void start_waiting();
  [[nodiscard]] bool finished_waiting(uint32_t timeout_us) const;
};

/**
 * High-level (stateful) driver for Sensirion SFM3019 flow sensor
 */
class Sensor : public Initializable {
 public:
  Sensor(Device &device, bool resetter, HAL::Time &time)
      : resetter(resetter), device_(device), time_(time) {}

  InitializableState setup() override;
  InitializableState output(float &flow);

 private:
  using Action = StateMachine::Action;

  static const uint32_t power_up_delay = 2;  // ms
  static const uint32_t product_number = 0x04020611;
  static const uint32_t read_conv_delay_us = 20;  // us
  static const int16_t scale_factor = 170;
  static const int16_t offset = -24576;
  static const uint16_t flow_unit =
      make_flow_unit(UnitPrefix::none, TimeBase::per_min, Unit::standard_liter_20deg);
  static const uint32_t averaging_window = 0;
  static constexpr float flow_min = -200;       // L/min
  static constexpr float flow_max = 200;        // L/min
  static const size_t max_retries_setup = 8;    // max retries for all setup steps combined
  static const size_t max_retries_measure = 8;  // max retries between valid outputs

  const bool resetter;

  Device &device_;
  StateMachine fsm_;
  Action next_action_ = Action::initialize;
  size_t retry_count_ = 0;

  uint32_t pn_ = 0;
  ConversionFactors conversion_{};
  Sample sample_{};

  HAL::Time &time_;

  InitializableState initialize(uint32_t current_time);
  InitializableState check_range(uint32_t current_time_us);
  InitializableState measure(uint32_t current_time_us, float &flow);
};

}  // namespace Pufferfish::Driver::I2C::SFM3019
