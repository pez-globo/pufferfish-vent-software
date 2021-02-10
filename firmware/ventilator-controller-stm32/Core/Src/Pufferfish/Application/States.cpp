/*
 * Application.tpp
 *
 *  Created on: May 26, 2020
 *      Author: Ethan Li
 */

#include "Pufferfish/Application/States.h"

// This macro is used to add a setter for a specified protobuf type with an associated
// union field and enum value. The use of a macro here complements the use of nanopb for
// generating types and code. We use a macro because it makes the code more maintainable here,
// while allowing us to ensure union tagging.
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define STATESEGMENT_TAGGED_SETTER(type, field) \
    template <>\
    template <>\
    void Util::TaggedUnion<Application::StateSegmentUnion, Application::MessageTypes>::set<type>(\
        const type &new_value) {\
      tag = Application::MessageTypes::field;\
      value.field = new_value; } // NOLINT(cppcoreguidelines-pro-type-union-access)
// clang-format on

// This macro is used to access a specified protobuf type from a union field and
// save it in the associated states field. We use a macro because it makes the code
// more maintainable here, while allowing us to ensure union tagging.
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define STATESEGMENT_GET_TAGGED(field, segment) \
  if ((segment).tag == MessageTypes::field) {\
    state_segments_.field = (segment).value.field; return; } // NOLINT(cppcoreguidelines-pro-type-union-access)
// clang-format on

namespace Pufferfish::Util {

// StateSegment
// Refer to States.h for justification of why we are using unions this way

STATESEGMENT_TAGGED_SETTER(SensorMeasurements, sensor_measurements)
STATESEGMENT_TAGGED_SETTER(CycleMeasurements, cycle_measurements)
STATESEGMENT_TAGGED_SETTER(Parameters, parameters)
STATESEGMENT_TAGGED_SETTER(ParametersRequest, parameters_request)
STATESEGMENT_TAGGED_SETTER(AlarmLimits, alarm_limits)
STATESEGMENT_TAGGED_SETTER(AlarmLimitsRequest, alarm_limits_request)

}  // namespace Pufferfish::Util

namespace Pufferfish::Application {

// States

const ParametersRequest &States::parameters_request() const {
  return state_segments_.parameters_request;
}

Parameters &States::parameters() {
  return state_segments_.parameters;
}

SensorMeasurements &States::sensor_measurements() {
  return state_segments_.sensor_measurements;
}

CycleMeasurements &States::cycle_measurements() {
  return state_segments_.cycle_measurements;
}

void States::input(const StateSegment &input) {
  STATESEGMENT_GET_TAGGED(sensor_measurements, input);
  STATESEGMENT_GET_TAGGED(cycle_measurements, input);
  STATESEGMENT_GET_TAGGED(parameters, input);
  STATESEGMENT_GET_TAGGED(parameters_request, input);
  STATESEGMENT_GET_TAGGED(alarm_limits, input);
  STATESEGMENT_GET_TAGGED(alarm_limits_request, input);
}

void States::output(MessageTypes type, StateSegment &output) const {
  switch (type) {
    case MessageTypes::sensor_measurements:
      output.set(state_segments_.sensor_measurements);
      return;
    case MessageTypes::cycle_measurements:
      output.set(state_segments_.cycle_measurements);
      return;
    case MessageTypes::parameters:
      output.set(state_segments_.parameters);
      return;
    case MessageTypes::parameters_request:
      output.set(state_segments_.parameters_request);
      return;
    case MessageTypes::alarm_limits:
      output.set(state_segments_.alarm_limits);
      return;
    case MessageTypes::alarm_limits_request:
      output.set(state_segments_.alarm_limits_request);
      return;
    default:
      return;
  }
}

}  // namespace Pufferfish::Application
