/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Chunks.cpp
 *
 *  Created on: Nov 3, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of chunk splitter
 *
 */

#include "Pufferfish/Protocols/States.h"

#include "Pufferfish/Application/States.h"
#include "Pufferfish/HAL/CRCChecker.h"
#include "Pufferfish/HAL/Mock/MockTime.h"
#include "Pufferfish/Protocols/Chunks.h"
#include "Pufferfish/Test/BackendDefs.h"
#include "Pufferfish/Util/Array.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

using StateOutputScheduleEntry =
    PF::Protocols::StateOutputScheduleEntry<PF::Application::MessageTypes>;
using States = PF::Application::States;
using StateSegment = PF::Application::StateSegment;
using MessageTypes = PF::Application::MessageTypes;
namespace BE = PF::Driver::Serial::Backend;

SCENARIO(
    "Protocols:: States input method correctly updates the internal current_time field of the "
    "synchroniser",
    "[states]") {
  GIVEN(
      "A StateSynchronizer object constructed with an output schedule array of multiple message "
      "types") {
    using BackendStateSynchronizer = PF::Protocols::
        StateSynchronizer<States, StateSegment, MessageTypes, BE::state_sync_schedule.size()>;

    const BackendStateSynchronizer::InputStatus input_ok =
        BackendStateSynchronizer::InputStatus::ok;
    const BackendStateSynchronizer::InputStatus input_invalid =
        BackendStateSynchronizer::InputStatus::invalid_type;

    StateSegment input_state;
    States states{};

    BackendStateSynchronizer synchronizer{states, BE::state_sync_schedule};

    PF::HAL::MockTime time;

    WHEN("the current time is written to it") {
      const uint32_t ctime = 0x12345678;

      auto input_status = synchronizer.input(ctime);

      THEN("the final status should be ok") { REQUIRE(input_status == input_ok); }
    }
  }
}

SCENARIO(
    "Protocols::The States output method correctly updates output StateSegment tag and field "
    "parameters according to a schedule",
    "[states]") {
  constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
      StateOutputScheduleEntry{5, MessageTypes::parameters_request},
      StateOutputScheduleEntry{6, MessageTypes::parameters_request});

  using BackendStateSynchronizer = PF::Protocols::
      StateSynchronizer<States, StateSegment, MessageTypes, state_sync_schedule.size()>;

  const BackendStateSynchronizer::InputStatus input_ok = BackendStateSynchronizer::InputStatus::ok;
  const BackendStateSynchronizer::OutputStatus output_ok =
      BackendStateSynchronizer::OutputStatus::ok;
  const BackendStateSynchronizer::OutputStatus waiting =
      BackendStateSynchronizer::OutputStatus::waiting;

  States states{};

  StateSegment input_state;
  StateSegment output_state;

  GIVEN(
      "A StateSynchronizer object constructed with an output schedule array of single message "
      "type") {
    BackendStateSynchronizer synchronizer{states, state_sync_schedule};

    uint32_t time = 10;
    auto input_time = synchronizer.input(time);
    REQUIRE(input_time == input_ok);

    ParametersRequest parameters_request;
    memset(&parameters_request, 0, sizeof(parameters_request));
    parameters_request.ventilating = true;
    parameters_request.fio2 = 30;
    parameters_request.mode = VentilationMode_hfnc;
    input_state.set(parameters_request);

    auto input_segment = states.input(input_state);
    REQUIRE(input_segment == States::InputStatus::ok);
    REQUIRE(input_state.tag == MessageTypes::parameters_request);

    WHEN("The current time is greater than the delay of the 1st output schedule entry") {
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN(
          "The output StateSegment fields are set and match the parameters request of the "
          "all_states object") {
        REQUIRE(output_state.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
        REQUIRE(output_state.value.parameters_request.ventilating == true);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }
    }

    WHEN("The current time is less than the delay of the 2nd output schedule entry") {
      auto output_status = synchronizer.output(output_state);

      THEN("The initial output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 15;
      synchronizer.input(time);

      auto final_status = synchronizer.output(output_state);
      THEN("The final output status returns waiting") { REQUIRE(final_status == waiting); }
    }

    WHEN("The current time is greater than the delay of the 2nd output schedule entry") {
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN(
          "The output StateSegment fields are set and match the parameters request of the "
          "all_states object") {
        REQUIRE(output_state.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
        REQUIRE(output_state.value.parameters_request.ventilating == true);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }

      uint32_t time = 16;
      synchronizer.input(time);

      auto final_status = synchronizer.output(output_state);

      THEN("the outuput status returns ok") { REQUIRE(final_status == output_ok); }
      THEN(
          "The output StateSegment fields are set and match the parameters request of the "
          "all_states object") {
        REQUIRE(output_state.tag == PF::Application::MessageTypes::parameters_request);
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
        REQUIRE(output_state.value.parameters_request.ventilating == true);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }
    }

    WHEN("The output cycles back to the 0th index of the output schedule array") {
      // 0th index
      auto initial = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(initial == output_ok); }

      uint32_t time = 16;
      synchronizer.input(time);

      // 1st index
      auto first = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(first == output_ok); }

      // 0th index
      uint32_t final_time = 21;
      synchronizer.input(final_time);

      auto final = synchronizer.output(output_state);
      // as status returns ok, it is within_timeout, exactly equal to the 0 index delay
      THEN("The output status returns ok") { REQUIRE(final == output_ok); }
    }
  }

  GIVEN(
      "A StateSynchronizer object constructed with an output schedule array of multiple message "
      "types and a all_states object") {
    constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
        StateOutputScheduleEntry{1, MessageTypes::parameters_request},
        StateOutputScheduleEntry{2, MessageTypes::parameters},
        StateOutputScheduleEntry{3, MessageTypes::sensor_measurements},
        StateOutputScheduleEntry{4, MessageTypes::cycle_measurements});

    using BackendStateSynchronizer = PF::Protocols::
        StateSynchronizer<States, StateSegment, MessageTypes, state_sync_schedule.size()>;

    const BackendStateSynchronizer::InputStatus input_ok =
        BackendStateSynchronizer::InputStatus::ok;
    const BackendStateSynchronizer::OutputStatus output_ok =
        BackendStateSynchronizer::OutputStatus::ok;
    const BackendStateSynchronizer::OutputStatus waiting =
        BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

    BackendStateSynchronizer synchronizer{states, state_sync_schedule};

    // creation of all_states object
    ParametersRequest parameters_request;
    memset(&parameters_request, 0, sizeof(parameters_request));
    parameters_request.fio2 = 56;
    parameters_request.mode = VentilationMode_hfnc;
    input_state.set(parameters_request);
    auto input_pr = states.input(input_state);
    REQUIRE(input_pr == States::InputStatus::ok);

    Parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    parameters.flow = 60;
    parameters.ventilating = true;
    input_state.set(parameters);
    auto input_parameters = states.input(input_state);
    REQUIRE(input_parameters == States::InputStatus::ok);

    SensorMeasurements sensor_measurements;
    memset(&sensor_measurements, 0, sizeof(sensor_measurements));
    sensor_measurements.paw = 20;
    sensor_measurements.spo2 = 94;
    input_state.set(sensor_measurements);
    auto input_sm = states.input(input_state);
    REQUIRE(input_sm == States::InputStatus::ok);

    CycleMeasurements cycle_measurements;
    memset(&cycle_measurements, 0, sizeof(cycle_measurements));
    cycle_measurements.rr = 20;
    input_state.set(cycle_measurements);
    auto input_cm = states.input(input_state);
    REQUIRE(input_cm == States::InputStatus::ok);

    // input all_states
    auto input_segment = states.input(input_state);
    REQUIRE(input_segment == States::InputStatus::ok);

    uint32_t time = 1;
    auto input_time = synchronizer.input(time);
    REQUIRE(input_time == input_ok);

    WHEN("The current time is greater than the delay of the 1st output schedule entry") {
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters request of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }
    }

    WHEN("The current time is less than the delay of the 2nd output schedule entry") {
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 2;
      auto input_time = synchronizer.input(time);
      REQUIRE(input_time == input_ok);

      THEN("The ouptut status returns waiting") { REQUIRE(output_status == output_ok); }
    }

    WHEN("The current time is greater than the delay of the 2nd output schedule entry") {
      auto output_status = synchronizer.output(output_state);

      // same as previous when
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 3;
      auto input_time = synchronizer.input(time);
      REQUIRE(input_time == input_ok);

      auto final_output = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to parameters") {
        REQUIRE(output_state.tag == MessageTypes::parameters);
      }

      THEN(
          "The output StateSegment fields are set and match the parameters of the all_states "
          "object") {
        REQUIRE(output_state.value.parameters.flow == 60);
        REQUIRE(output_state.value.parameters.ventilating == true);
      }
    }

    WHEN("The current time is greater than the delay of the 3rd output schedule entry") {
      // skip to the 3rd element in the output schedule array
      synchronizer.output(output_state);

      uint32_t time2 = 3;
      auto input_2 = synchronizer.input(time2);
      synchronizer.output(output_state);

      uint32_t time3 = 6;
      auto input_3 = synchronizer.input(time3);
      REQUIRE(input_3 == input_ok);

      auto output_status = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to sensor_measurements") {
        REQUIRE(output_state.tag == MessageTypes::sensor_measurements);
      }

      THEN(
          "The output StateSegment fields are set and match the sensor measurements of the "
          "all_states object") {
        REQUIRE(output_state.value.sensor_measurements.paw == 20);
        REQUIRE(output_state.value.sensor_measurements.spo2 == 94);
      }
    }

    WHEN("The current time is greater than the delay of the 3rd output schedule entry") {
      // skip to the 4th element in the output schedule array
      synchronizer.output(output_state);

      uint32_t time2 = 3;
      auto input_2 = synchronizer.input(time2);
      synchronizer.output(output_state);

      uint32_t time3 = 6;
      auto input_3 = synchronizer.input(time3);
      REQUIRE(input_3 == input_ok);
      synchronizer.output(output_state);

      uint32_t time4 = 11;
      auto final_input = synchronizer.input(time4);
      REQUIRE(final_input == input_ok);

      auto output_status = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to cycle measurements") {
        REQUIRE(output_state.tag == MessageTypes::cycle_measurements);
      }

      THEN(
          "The output StateSegment fields are set and match the cycle measurements of the "
          "all_states object") {
        REQUIRE(output_state.value.cycle_measurements.rr == 20);
      }
    }

    WHEN(
        "The input all_state fields are changed , the output StateSegment fields remain "
        "unchanged") {
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters request of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }

      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.fio2 = 10;
      parameters_request.mode = VentilationMode_pc_ac;
      input_state.set(parameters_request);

      auto input_status = states.input(input_state);
      REQUIRE(input_status == States::InputStatus::ok);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN("The output StateSegment fields remain unchanged") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }
    }

    WHEN("The output cycles back to the 0th index of the output schedule array") {
      auto first = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(first == output_ok); }
      THEN("The tag of the output StateSegment object is equal to parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }

      uint32_t time2 = 3;
      auto input_2 = synchronizer.input(time2);
      REQUIRE(input_2 == input_ok);
      auto second = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(second == output_ok); }
      THEN("The tag of the output StateSegment object is equal to parameters") {
        REQUIRE(output_state.tag == MessageTypes::parameters);
      }

      uint32_t time3 = 6;
      auto input_3 = synchronizer.input(time3);
      REQUIRE(input_3 == input_ok);
      auto third = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(third == output_ok); }
      THEN("The tag of the output StateSegment object is equal to sensor_measurements") {
        REQUIRE(output_state.tag == MessageTypes::sensor_measurements);
      }

      uint32_t time4 = 11;
      auto input_4 = synchronizer.input(time4);
      REQUIRE(input_4 == input_ok);
      auto fourth = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(fourth == output_ok); }
      THEN("The tag of the output StateSegment object is equal to cycle_measurements") {
        REQUIRE(output_state.tag == MessageTypes::cycle_measurements);
      }

      uint32_t final_time = 12;
      auto final_input = synchronizer.input(final_time);
      REQUIRE(final_input == input_ok);
      auto final = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(final == output_ok); }

      THEN("The tag of the output StateSegment object is equal to parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
    }
  }

  GIVEN(
      "A StateSynchronizer object constructed with output schedule array from backend and a "
      "all_states object") {
    using BackendStateSynchronizer = PF::Protocols::
        StateSynchronizer<States, StateSegment, MessageTypes, BE::state_sync_schedule.size()>;

    const BackendStateSynchronizer::InputStatus input_ok =
        BackendStateSynchronizer::InputStatus::ok;
    const BackendStateSynchronizer::OutputStatus output_ok =
        BackendStateSynchronizer::OutputStatus::ok;
    const BackendStateSynchronizer::OutputStatus waiting =
        BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

    BackendStateSynchronizer synchronizer{states, BE::state_sync_schedule};

    // creation of all_states object
    ParametersRequest parameters_request;
    memset(&parameters_request, 0, sizeof(parameters_request));
    parameters_request.fio2 = 56;
    parameters_request.mode = VentilationMode_hfnc;
    input_state.set(parameters_request);
    auto input_pr = states.input(input_state);
    REQUIRE(input_pr == States::InputStatus::ok);

    Parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    parameters.flow = 60;
    parameters.ventilating = true;
    input_state.set(parameters);
    auto input_parameters = states.input(input_state);
    REQUIRE(input_parameters == States::InputStatus::ok);

    SensorMeasurements sensor_measurements;
    memset(&sensor_measurements, 0, sizeof(sensor_measurements));
    sensor_measurements.paw = 20;
    sensor_measurements.spo2 = 94;
    input_state.set(sensor_measurements);
    auto input_sm = states.input(input_state);
    REQUIRE(input_sm == States::InputStatus::ok);

    AlarmLimits alarm_limits = {};
    Range range = {};
    range.lower = 21;
    range.upper = 100;
    alarm_limits.has_fio2 = true;
    alarm_limits.fio2 = range;
    input_state.set(alarm_limits);
    auto input_al = states.input(input_state);
    REQUIRE(input_al == States::InputStatus::ok);

    AlarmLimitsRequest alarm_limits_request = {};
    alarm_limits_request.has_fio2 = true;
    alarm_limits_request.fio2 = range;
    input_state.set(alarm_limits_request);
    auto input_ar = states.input(input_state);
    REQUIRE(input_ar == States::InputStatus::ok);

    // input all_states
    auto input_segment = states.input(input_state);
    REQUIRE(input_segment == States::InputStatus::ok);

    uint32_t time = 10;
    auto input_time = synchronizer.input(time);
    REQUIRE(input_time == input_ok);

    WHEN("The current time is greater than the delay of the 1st output schedule entry") {
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to sensor_measurements") {
        REQUIRE(output_state.tag == MessageTypes::sensor_measurements);
      }

      THEN(
          "The output StateSegment fields are set and match the sensor measurements of the "
          "all_states object") {
        REQUIRE(output_state.value.sensor_measurements.paw == 20);
        REQUIRE(output_state.value.sensor_measurements.spo2 == 94);
      }
    }

    WHEN("The current time is greater than the delay of the 2nd output schedule entry") {
      // same as previous when
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 20;
      auto input_time = synchronizer.input(time);
      REQUIRE(input_time == input_ok);

      auto final_output = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The tag of the output StateSegment object is equal to parameters") {
        REQUIRE(output_state.tag == MessageTypes::parameters);
      }

      THEN(
          "The output StateSegment fields are set and match the parameters of the all_states "
          "object") {
        REQUIRE(output_state.value.parameters.flow == 60);
        REQUIRE(output_state.value.parameters.ventilating == true);
      }
    }

    WHEN("The current time is greater than the delay of the 3rd output schedule entry") {
      // same as previous when
      auto initial_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(initial_status == output_ok); }

      uint32_t time = 20;
      auto input_time = synchronizer.input(time);
      REQUIRE(input_time == input_ok);

      auto output_status = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t ftime = 30;
      auto final_time = synchronizer.input(ftime);
      REQUIRE(final_time == input_ok);

      auto final_output = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(final_output == output_ok); }

      THEN("The tag of the output StateSegment object is equal to alarm_limits") {
        REQUIRE(output_state.tag == MessageTypes::alarm_limits);
      }

      THEN(
          "The output StateSegment fields are set and match the alarm limits of the all_states "
          "object") {
        REQUIRE(output_state.value.alarm_limits.fio2.lower == 21);
        REQUIRE(output_state.value.alarm_limits.fio2.upper == 100);
      }
    }
  }
}