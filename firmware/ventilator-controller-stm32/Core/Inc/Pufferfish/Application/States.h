/*
 * Application.h
 *
 *  Created on: May 26, 2020
 *      Author: Ethan Li
 */

#pragma once

#include "Pufferfish/Util/TaggedUnion.h"
#include "mcu_pb.h"

namespace Pufferfish::Application {

// Type tags

enum class MessageTypes : uint8_t {
  unknown = 0,
  sensor_measurements = 2,
  cycle_measurements = 3,
  parameters = 4,
  parameters_request = 5,
  alarm_limits = 6,
  alarm_limits_request = 7
};

// Since nanopb is running dynamically, we cannot have extensive compile-time type-checking.
// It's not clear how we might use variants to replace this union, since the nanopb functions
// would need access to the underlying memory in the variant, which is not publicly accessible.
// For now the State Segment class is a simple tagged union, though we don't have any compiler
// type-checking with the tags - it's enforced with the templated functions.
union StateSegmentUnion;

using StateSegment = Util::TaggedUnion<StateSegmentUnion, MessageTypes>;

struct StateSegments {
  // Backend States
  SensorMeasurements sensor_measurements;
  CycleMeasurements cycle_measurements;
  Parameters parameters;
  ParametersRequest parameters_request;
  AlarmLimits alarm_limits;
  AlarmLimitsRequest alarm_limits_request;
};

class States {
 public:
  States() = default;

  [[nodiscard]] const ParametersRequest &parameters_request() const;
  Parameters &parameters();
  SensorMeasurements &sensor_measurements();
  CycleMeasurements &cycle_measurements();

  static constexpr bool should_input(MessageTypes type);

  void input(const StateSegment &input);
  void output(MessageTypes type, StateSegment &output) const;

 private:
  StateSegments state_segments_;
};

}  // namespace Pufferfish::Application

#include "States.tpp"
