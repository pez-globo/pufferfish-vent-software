/*
 * StateMachine.cpp
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 */

#include "Pufferfish/Driver/Serial/FDO2/Sensor.h"

#include "Pufferfish/Util/Timeouts.h"

// This macro is used to add a checker for the value of a specified request type with an associated
// union field and enum value. We use a macro because it makes the code more maintainable here,
// while allowing us to ensure union tagging.
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define RESPONSE_TAGGED_COMPARISON(type, field) \
  inline bool operator==(const Response &input_response, const type &expected_value) {\
    if (input_response.tag != CommandTypes::field) {\
      return false;\
    }\
    return input_response.value.field == expected_value; } // NOLINT(cppcoreguidelines-pro-type-union-access)
// clang-format on

namespace Pufferfish::Driver::Serial::FDO2 {

// StateMachine

StateMachine::Action StateMachine::update(uint32_t current_time, bool passed_check) {
  current_time_ = current_time;
  switch (next_action_) {
    case Action::request_version:
      start_request();
      next_action_ = Action::check_version;
      break;
    case Action::check_version:
      if (passed_check) {
        next_action_ = Action::start_broadcast;
      } else if (timed_out()) {
        next_action_ = Action::request_version;
      }
      break;
    case Action::start_broadcast:
      start_request();
      next_action_ = Action::check_broadcast;
      break;
    case Action::check_broadcast:
      if (passed_check) {
        next_action_ = Action::wait_measurement;
      } else if (timed_out()) {
        next_action_ = Action::start_broadcast;
      }
    case Action::wait_measurement:
      break;
  }
  return next_action_;
}

void StateMachine::start_request() {
  request_time_ = current_time_;
}

bool StateMachine::timed_out() const {
  return !Util::within_timeout(request_time_, response_timeout, current_time_);
}

RESPONSE_TAGGED_COMPARISON(Responses::Vers, vers)
RESPONSE_TAGGED_COMPARISON(Responses::Bcst, bcst)

// Sensor

InitializableState Sensor::setup() {
  if (retry_count_ > max_retries_setup) {
    return InitializableState::failed;
  }

  switch (next_action_) {
    case Action::request_version:
      device_.request_version();
      next_action_ = fsm_.update(time_.millis());
      return InitializableState::setup;
    case Action::check_version:
      return check_version(time_.millis());
    case Action::start_broadcast:
      device_.start_broadcast();
      next_action_ = fsm_.update(time_.millis());
      return InitializableState::setup;
    case Action::check_broadcast:
      return check_broadcast(time_.millis());
    case Action::wait_measurement:
      next_action_ = fsm_.update(time_.millis());
      return InitializableState::ok;
  }
  return InitializableState::failed;
}

InitializableState Sensor::output(uint32_t &po2) {
  if (next_action_ != Action::wait_measurement) {
    return InitializableState::failed;
  }

  Response response;
  device_.receive(response);
  // This is a tagged union access
  if (response.tag == CommandTypes::mraw) {
    po2 = response.value.mraw.po2;  // NOLINT(cppcoreguidelines-pro-type-union-access)
  }
  return InitializableState::ok;
}

bool Sensor::get_response(CommandTypes type, Response &response) {
  while (true) {
    if (device_.receive(response) != Device::Status::ok) {
      return false;
    }

    if (response.tag == type) {
      return true;
    }
  }
}

InitializableState Sensor::check_version(uint32_t current_time) {
  Response response;
  if (!get_response(CommandTypes::vers, response)) {
    next_action_ = fsm_.update(current_time);
    if (next_action_ != Action::check_version) {
      ++retry_count_;
    }
    return InitializableState::setup;
  }

  if (response == expected_vers) {
    next_action_ = fsm_.update(current_time, true);
  } else {
    next_action_ = fsm_.update(current_time);
    ++retry_count_;
  }
  return InitializableState::setup;
}

InitializableState Sensor::check_broadcast(uint32_t current_time) {
  Response response;
  if (!get_response(CommandTypes::bcst, response)) {
    next_action_ = fsm_.update(current_time);
    if (next_action_ != Action::check_broadcast) {
      ++retry_count_;
    }
    return InitializableState::setup;
  }

  if (response == expected_bcst) {
    next_action_ = fsm_.update(current_time, true);
  } else {
    next_action_ = fsm_.update(current_time);
    ++retry_count_;
  }
  return InitializableState::setup;
}

}  // namespace Pufferfish::Driver::Serial::FDO2
