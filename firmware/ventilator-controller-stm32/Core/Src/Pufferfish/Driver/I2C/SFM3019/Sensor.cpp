/*
 * StateMachine.cpp
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 */

#include "Pufferfish/Driver/I2C/SFM3019/Sensor.h"

#include <cmath>

#include "Pufferfish/HAL/STM32/Time.h"
#include "Pufferfish/Util/Timeouts.h"

namespace Pufferfish::Driver::I2C::SFM3019 {

// StateMachine

StateMachine::Action StateMachine::update(uint32_t current_time_us) {
  current_time_us_ = current_time_us;
  switch (next_action_) {
    case Action::initialize:
      next_action_ = Action::wait_warmup;
      start_waiting();
      break;
    case Action::wait_warmup:
      if (finished_waiting(warming_up_duration_us)) {
        next_action_ = Action::check_range;
      } else {
        next_action_ = Action::wait_warmup;
      }
      break;
    case Action::check_range:
    case Action::measure:
      next_action_ = Action::wait_measurement;
      start_waiting();
      break;
    case Action::wait_measurement:
      if (finished_waiting(measuring_duration_us)) {
        next_action_ = Action::measure;
      } else {
        next_action_ = Action::wait_measurement;
      }
      break;
  }
  return next_action_;
}

void StateMachine::start_waiting() {
  wait_start_time_us_ = current_time_us_;
}

bool StateMachine::finished_waiting(uint32_t timeout_us) const {
  return !Util::within_timeout(wait_start_time_us_, timeout_us, current_time_us_);
}

// Sensor

InitializableState Sensor::setup() {
  float flow = NAN;
  return output(flow);
}

InitializableState Sensor::output(float &flow) {
  switch (next_action_) {
    case Action::initialize:
      return initialize(HAL::micros());
    case Action::wait_warmup:
      next_action_ = fsm_.update(HAL::micros());
      return InitializableState::setup;
    case Action::check_range:
      return check_range(HAL::micros());
    case Action::measure:
      return measure(HAL::micros(), flow);
    case Action::wait_measurement:
      next_action_ = fsm_.update(HAL::micros());
      return InitializableState::ok;
    default:
      break;
  }
  return InitializableState::failed;
}

InitializableState Sensor::initialize(uint32_t current_time_us) {
  if (retry_count_ > max_retries_setup) {
    return InitializableState::failed;
  }

  retry_count_ = 0;
  // Reset the device
  if (resetter) {
    while (device_.reset() != I2CDeviceStatus::ok) {
      ++retry_count_;
      if (retry_count_ > max_retries_setup) {
        return InitializableState::failed;
      }
    }
  }

  // Wait for power-up
  HAL::delay(2);

  // Read product number
  while (device_.serial_number(pn_) != I2CDeviceStatus::ok || pn_ != product_number) {
    ++retry_count_;
    if (retry_count_ > max_retries_setup) {
      return InitializableState::failed;
    }
  }

  // TODO(lietk12): implement the conversion retrieval

  // TODO(lietk12): implement the configuring averaging

  while (device_.start_measure() != I2CDeviceStatus::ok) {
    ++retry_count_;
    if (retry_count_ > max_retries_setup) {
      return InitializableState::failed;
    }
  }

  next_action_ = fsm_.update(current_time_us);
  retry_count_ = 0;  // reset retries to 0 for measuring
  return InitializableState::setup;
}

InitializableState Sensor::check_range(uint32_t current_time_us) {
  if (device_.read_sample(sample_, conversion_.scale_factor, conversion_.offset) ==
          I2CDeviceStatus::ok &&
      sample_.flow >= flow_min && sample_.flow <= flow_max) {
    next_action_ = fsm_.update(current_time_us);
    return InitializableState::ok;
  }

  ++retry_count_;
  if (retry_count_ > max_retries_setup) {
    return InitializableState::failed;
  }

  return InitializableState::ok;
}

InitializableState Sensor::measure(uint32_t current_time_us, float &flow) {
  if (device_.read_sample(sample_, conversion_.scale_factor, conversion_.offset) ==
      I2CDeviceStatus::ok) {
    retry_count_ = 0;  // reset retries to 0 for next measurement
    flow = sample_.flow;
    next_action_ = fsm_.update(current_time_us);
    return InitializableState::ok;
  }

  ++retry_count_;
  if (retry_count_ > max_retries_measure) {
    return InitializableState::failed;
  }

  return InitializableState::ok;
}

}  // namespace Pufferfish::Driver::I2C::SFM3019