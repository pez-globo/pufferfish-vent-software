#pragma once

#include "Pufferfish/Protocols/Messages.h"
#include "Pufferfish/Protocols/States.h"
#include "Pufferfish/Util/Array.h"

namespace Pufferfish::Driver::Serial::Backend {

// States

constexpr auto message_descriptors = Util::make_array<Util::ProtobufDescriptor>(
    // array index should match the type code value
    Util::get_protobuf_descriptor<Util::UnrecognizedMessage>(),  // 0
    Util::get_protobuf_descriptor<Util::UnrecognizedMessage>(),  // 1
    Util::get_protobuf_descriptor<SensorMeasurements>(),         // 2
    Util::get_protobuf_descriptor<CycleMeasurements>(),          // 3
    Util::get_protobuf_descriptor<Parameters>(),                 // 4
    Util::get_protobuf_descriptor<ParametersRequest>(),          // 5
    Util::get_protobuf_descriptor<AlarmLimits>(),                // 6
    Util::get_protobuf_descriptor<AlarmLimitsRequest>()          // 7
);

// State Synchronization

using StateOutputScheduleEntry = Protocols::StateOutputScheduleEntry<Application::MessageTypes>;

constexpr auto state_sync_schedule = Util::make_array<const StateOutputScheduleEntry>(
    StateOutputScheduleEntry{10, Application::MessageTypes::sensor_measurements},
    StateOutputScheduleEntry{10, Application::MessageTypes::parameters},
    StateOutputScheduleEntry{10, Application::MessageTypes::alarm_limits},
    StateOutputScheduleEntry{10, Application::MessageTypes::sensor_measurements},
    StateOutputScheduleEntry{10, Application::MessageTypes::cycle_measurements},
    StateOutputScheduleEntry{10, Application::MessageTypes::alarm_limits_request},
    StateOutputScheduleEntry{10, Application::MessageTypes::sensor_measurements},
    StateOutputScheduleEntry{10, Application::MessageTypes::parameters_request},
    StateOutputScheduleEntry{10, Application::MessageTypes::cycle_measurements});
}  // namespace Pufferfish::Driver::Serial::Backend
