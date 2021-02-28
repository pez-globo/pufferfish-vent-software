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
    "Protocols::The States output method correctly updates output StateSegment tag and field "
    "parameters according to a schedule",
    "[states]") {
  constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
      StateOutputScheduleEntry{5, MessageTypes::parameters_request},
      StateOutputScheduleEntry{6, MessageTypes::parameters_request});

  using BackendStateSynchronizer = PF::Protocols::
      StateSynchronizer<States, StateSegment, MessageTypes, state_sync_schedule.size()>;

  const BackendStateSynchronizer::OutputStatus output_ok =
      BackendStateSynchronizer::OutputStatus::ok;
  const BackendStateSynchronizer::OutputStatus waiting =
      BackendStateSynchronizer::OutputStatus::waiting;

  States states{};

  StateSegment input_state;
  StateSegment output_state;

  GIVEN(
      "A StateSynchronizer object constructed with an output schedule array of single message type "
      "and a state object with values for parameters request") {
    ParametersRequest parameters_request;
    memset(&parameters_request, 0, sizeof(parameters_request));
    parameters_request.ventilating = true;
    parameters_request.fio2 = 30;
    parameters_request.mode = VentilationMode_hfnc;
    input_state.set(parameters_request);

    auto input_segment = states.input(input_state);
    REQUIRE(input_segment == States::InputStatus::ok);
    REQUIRE(input_state.tag == MessageTypes::parameters_request);

    BackendStateSynchronizer synchronizer{states, state_sync_schedule};

    uint32_t time = 10;
    synchronizer.input(time);

    WHEN(
        "The synchroniser is on the 0th element of the schedule with a delay of 5 and the "
        "current_time is 0") {
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
      // change state field values
      parameters_request.fio2 = 70;
      input_state.set(parameters_request);
      states.input(input_state);

      THEN("The Output StateSegment fields are unchanged") {
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
      }
    }

    WHEN(
        "The synchroniser is on the 0th element of the schedule, and the current_time(10) is less "
        "than the delay of the 2nd output schedule entry(6)") {
      auto output_status = synchronizer.output(output_state);

      THEN("The initial output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 15;
      synchronizer.input(time);

      auto final_status = synchronizer.output(output_state);
      THEN("The final output status returns waiting") { REQUIRE(final_status == waiting); }
    }

    WHEN(
        "The synchroniser is on the 0th element of the schedule and the current time is 10 and "
        "delay of the next entry is 6") {
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

      // 0th index
      uint32_t final_time = 21;
      synchronizer.input(final_time);

      auto final = synchronizer.output(output_state);
      // as status returns ok, it is within_timeout, exactly equal to the 0 index delay
      THEN("The output status returns ok") { REQUIRE(final == output_ok); }

      // change state field values
      parameters_request.fio2 = 70;
      input_state.set(parameters_request);
      states.input(input_state);

      THEN("The Output StateSegment fields are unchanged") {
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
      }
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

    const BackendStateSynchronizer::OutputStatus output_ok =
        BackendStateSynchronizer::OutputStatus::ok;
    const BackendStateSynchronizer::OutputStatus waiting =
        BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

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

    BackendStateSynchronizer synchronizer{states, state_sync_schedule};

    uint32_t time = 1;
    synchronizer.input(time);

    WHEN(
        "The synchroniser is on the 0th element of the schedule with a delay of 1 and the "
        "current_time is 0") {
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
      // Change parameters req field values
      parameters_request.fio2 = 90;
      input_state.set(parameters_request);
      states.input(input_state);

      THEN("The output StateSegment fields are unchagned") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
      }
    }

    WHEN(
        "The synchroniser is on the 0th element of the schedule, and the new current_time(2) is "
        "less than the delay of the 1st output schedule entry(2)") {
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 2;
      synchronizer.input(time);

      THEN("The ouptut status returns waiting") { REQUIRE(output_status == output_ok); }
    }

    WHEN(
        "The synchroniser is on the 0th element of the schedule, and the new current_time(3) is "
        "greater than the delay of the 2nd output schedule entry(2)") {
      auto output_status = synchronizer.output(output_state);

      // same as previous when
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 3;
      synchronizer.input(time);

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

      // Change parameters field values
      parameters.flow = 90;
      input_state.set(parameters);
      states.input(input_state);

      THEN("The output StateSegment fields are unchagned") {
        REQUIRE(output_state.value.parameters.flow == 60);
      }
    }

    WHEN(
        "The synchroniser is on the 1st element of the schedule, and the new current_time(6) is "
        "greater than the delay of the 3rd output schedule entry(3)") {
      // skip to the 3rd element in the output schedule array
      synchronizer.output(output_state);

      uint32_t time2 = 3;
      synchronizer.input(time2);
      synchronizer.output(output_state);

      uint32_t time3 = 6;
      synchronizer.input(time3);

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

      // change sensor measurements fields values
      sensor_measurements.paw = 30;
      input_state.set(sensor_measurements);
      states.input(input_state);

      THEN("The output StateSegment fields are unchagned") {
        REQUIRE(output_state.value.sensor_measurements.paw == 20);
      }
    }

    WHEN(
        "The synchroniser is on the 2nd element of the schedule, and the new current_time(11) is "
        "greater than the delay of the 4th output schedule entry(3)") {
      // skip to the 4th element in the output schedule array
      synchronizer.output(output_state);

      uint32_t time2 = 3;
      synchronizer.input(time2);
      synchronizer.output(output_state);

      uint32_t time3 = 6;
      synchronizer.input(time3);
      synchronizer.output(output_state);

      uint32_t time4 = 11;
      synchronizer.input(time4);

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

      // change cycle measurements fields values
      cycle_measurements.rr = 45;
      input_state.set(cycle_measurements);
      states.input(input_state);

      THEN("The output StateSegment fields are unchagned") {
        REQUIRE(output_state.value.cycle_measurements.rr == 20);
      }
    }

    WHEN(
        "The synchroniser is on the 3rd element of the schedule, and the new current_time(14) is "
        "greater than the delay of the 1st output schedule entry(3)") {
      synchronizer.output(output_state);

      uint32_t time2 = 3;
      synchronizer.input(time2);
      synchronizer.output(output_state);

      uint32_t time3 = 6;
      synchronizer.input(time3);
      synchronizer.output(output_state);

      uint32_t time4 = 11;
      synchronizer.input(time4);
      synchronizer.output(output_state);

      uint32_t ftime = 14;
      synchronizer.input(ftime);
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }
      THEN("The schedule goes back to the 0th element") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }

      THEN(
          "The output StateSegment fields are set and match the parameters request of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }

      // Change parameters req field values
      parameters_request.fio2 = 90;
      input_state.set(parameters_request);
      states.input(input_state);

      THEN("The output StateSegment fields are unchagned") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
      }
    }
  }

  GIVEN(
      "A StateSynchronizer object constructed with output schedule array from backend and a "
      "all_states object") {
    using BackendStateSynchronizer = PF::Protocols::
        StateSynchronizer<States, StateSegment, MessageTypes, BE::state_sync_schedule.size()>;

    const BackendStateSynchronizer::OutputStatus output_ok =
        BackendStateSynchronizer::OutputStatus::ok;
    const BackendStateSynchronizer::OutputStatus waiting =
        BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

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

    BackendStateSynchronizer synchronizer{states, BE::state_sync_schedule};

    uint32_t time = 10;
    synchronizer.input(time);

    WHEN(
        "The synchroniser is on the 0th element of the schedule with a delay of 1 and the "
        "current_time is 0") {
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

      // change sensor measurements field values
      sensor_measurements.spo2 = 90;
      input_state.set(sensor_measurements);
      states.input(input_state);

      THEN("The output StateSegment fields remain unchanged") {
        REQUIRE(output_state.value.sensor_measurements.spo2 == 94);
      }
    }

    WHEN(
        "The synchroniser is on the 1st element of the schedule with a delay of the next entry is "
        "10 and the new current_time is 20") {
      // same as previous when
      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t time = 20;
      synchronizer.input(time);

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

      // change parameter field values
      parameters.ventilating = false;
      input_state.set(parameters);
      states.input(input_state);

      THEN("The output StateSegment fields remain unchanged") {
        REQUIRE(output_state.value.parameters.ventilating == true);
      }
    }

    WHEN(
        "The synchroniser is on the 2nd element of the schedule with a delay of the next entry is "
        "10 and the new current_time is 30") {
      // same as previous when
      auto initial_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(initial_status == output_ok); }

      uint32_t time = 20;
      synchronizer.input(time);

      auto output_status = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(output_status == output_ok); }

      uint32_t ftime = 30;
      synchronizer.input(ftime);

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

      // Change alarm limits values
      range.lower = 30;
      range.upper = 90;

      alarm_limits.fio2 = range;
      input_state.set(alarm_limits);
      states.input(input_state);

      THEN("The output StateSegment fields remain unchanged") {
        REQUIRE(output_state.value.alarm_limits.fio2.lower == 21);
        REQUIRE(output_state.value.alarm_limits.fio2.upper == 100);
      }
    }
  }

  GIVEN(
      "A StateSynchronizer object constructed with output schedule array containing unknown "
      "message type") {
    constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
        StateOutputScheduleEntry{1, MessageTypes::unknown},
        StateOutputScheduleEntry{1, MessageTypes::parameters_request});

    using BackendStateSynchronizer = PF::Protocols::
        StateSynchronizer<States, StateSegment, MessageTypes, state_sync_schedule.size()>;

    const BackendStateSynchronizer::OutputStatus output_ok =
        BackendStateSynchronizer::OutputStatus::ok;
    const BackendStateSynchronizer::OutputStatus invalid_type =
        BackendStateSynchronizer::OutputStatus::invalid_type;
    const BackendStateSynchronizer::OutputStatus waiting =
        BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

    auto input_segment = states.input(input_state);

    BackendStateSynchronizer synchronizer{states, state_sync_schedule};

    WHEN("The output is called on scheduler array with unknown message type") {
      uint32_t ftime = 10;
      synchronizer.input(ftime);

      auto output_status = synchronizer.output(output_state);

      THEN("The output status returns invalid type") { REQUIRE(output_status == invalid_type); }
    }
  }

  GIVEN(
      "A StateSynchronizer object constructed with an output schedule array of 4 message "
      "types and a all_states object and on the 1st entry") {
    constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
        StateOutputScheduleEntry{10, MessageTypes::parameters_request},
        StateOutputScheduleEntry{10, MessageTypes::parameters},
        StateOutputScheduleEntry{10, MessageTypes::sensor_measurements},
        StateOutputScheduleEntry{10, MessageTypes::cycle_measurements});

    using BackendStateSynchronizer = PF::Protocols::
        StateSynchronizer<States, StateSegment, MessageTypes, state_sync_schedule.size()>;

    const BackendStateSynchronizer::OutputStatus output_ok =
        BackendStateSynchronizer::OutputStatus::ok;
    const BackendStateSynchronizer::OutputStatus waiting =
        BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

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

    states.input(input_state);

    BackendStateSynchronizer synchronizer{states, state_sync_schedule};

    uint32_t time = 10;
    synchronizer.input(time);

    auto output_status = synchronizer.output(output_state);
    // 1st entry
    REQUIRE(output_status == output_ok);
    REQUIRE(output_state.tag == MessageTypes::parameters_request);
    REQUIRE(output_state.value.parameters_request.fio2 == 56);
    REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);

    WHEN("The new current time is greater than all the timeouts") {
      uint32_t final_time = 100;
      synchronizer.input(final_time);

      auto status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(status == output_ok); }
      THEN("Only one schedule entry is advanced and the tag of the output state is parameters") {
        REQUIRE(output_state.tag == MessageTypes::parameters);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters.flow == 60);
        REQUIRE(output_state.value.parameters.ventilating == true);
      }
    }

    WHEN("The State segment fields for the next entry are changed before its timeout") {
      uint32_t final_time = 20;
      synchronizer.input(final_time);

      parameters.flow = 80;
      parameters.ventilating = false;
      input_state.set(parameters);

      states.input(input_state);

      auto status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(status == output_ok); }
      THEN(
          "The output StateSegment fields are set and match the parameters of the changed field "
          "values") {
        REQUIRE(output_state.value.parameters.flow == 80);
        REQUIRE(output_state.value.parameters.ventilating == false);
      }
    }
  }

  GIVEN(
      "A StateSynchronizer object constructed with an output schedule array of 4 message "
      "types and a all_states object") {
    constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
        StateOutputScheduleEntry{10, MessageTypes::parameters_request},
        StateOutputScheduleEntry{10, MessageTypes::parameters},
        StateOutputScheduleEntry{10, MessageTypes::sensor_measurements},
        StateOutputScheduleEntry{10, MessageTypes::cycle_measurements});

    using BackendStateSynchronizer = PF::Protocols::
        StateSynchronizer<States, StateSegment, MessageTypes, state_sync_schedule.size()>;

    const BackendStateSynchronizer::OutputStatus output_ok =
        BackendStateSynchronizer::OutputStatus::ok;
    const BackendStateSynchronizer::OutputStatus waiting =
        BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

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

    states.input(input_state);

    BackendStateSynchronizer synchronizer{states, state_sync_schedule};

    // Intent: make 40 seconds pass equal to the timeout for the last entry
    WHEN("the synchronizer is on the 0th entry and its clock advances by 40 seconds") {
      uint32_t time = 40;
      synchronizer.input(time);

      auto status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(status == output_ok); }
      THEN("Only one schedule entry is advanced and the tag of the output state is parameters") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }
    }

    // Intent: advance the synchronizer clock in-between the timeouts of the entries
    WHEN("The synchronizer is on the 0th entry and its clock advances in between the timeouts") {
      // 1st entry
      uint32_t time = 15;
      synchronizer.input(time);

      auto first_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(first_status == output_ok); }
      THEN(
          "Only one schedule entry is advanced and the tag of the output state is "
          "parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }

      // 2nd entry
      uint32_t second_time = 25;
      synchronizer.input(second_time);

      auto second_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(second_status == output_ok); }
      THEN("Only one schedule entry is advanced and the tag of the output state is parameters") {
        REQUIRE(output_state.tag == MessageTypes::parameters);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters.flow == 60);
        REQUIRE(output_state.value.parameters.ventilating == true);
      }

      // 3rd entry
      uint32_t third_time = 35;
      synchronizer.input(third_time);

      auto third_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(third_status == output_ok); }
      THEN(
          "Only one schedule entry is advanced and the tag of the output state is "
          "sensor_measurements") {
        REQUIRE(output_state.tag == MessageTypes::sensor_measurements);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.sensor_measurements.paw == 20);
        REQUIRE(output_state.value.sensor_measurements.spo2 == 94);
      }

      // 4th entry
      uint32_t final_time = 45;
      synchronizer.input(final_time);

      auto final_status = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(final_status == output_ok); }
      THEN(
          "Only one schedule entry is advanced and the tag of the output state is "
          "cycle_measurements") {
        REQUIRE(output_state.tag == MessageTypes::cycle_measurements);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.cycle_measurements.rr == 20);
      }
    }

    WHEN(
        "The synchroniser is on the 0th entry, advances till the last entry and resets back to 0th "
        "entry") {
      // 1st entry
      uint32_t time = 10;
      synchronizer.input(time);

      auto first_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(first_status == output_ok); }
      THEN(
          "Only one schedule entry is advanced and the tag of the output state is "
          "parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }

      // 2nd entry
      uint32_t second_time = 20;
      synchronizer.input(second_time);

      auto second_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(second_status == output_ok); }
      THEN("Only one schedule entry is advanced and the tag of the output state is parameters") {
        REQUIRE(output_state.tag == MessageTypes::parameters);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters.flow == 60);
        REQUIRE(output_state.value.parameters.ventilating == true);
      }

      // 3rd entry
      uint32_t third_time = 30;
      synchronizer.input(third_time);

      auto third_status = synchronizer.output(output_state);

      THEN("The output status returns ok") { REQUIRE(third_status == output_ok); }
      THEN(
          "Only one schedule entry is advanced and the tag of the output state is "
          "sensor_measurements") {
        REQUIRE(output_state.tag == MessageTypes::sensor_measurements);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.sensor_measurements.paw == 20);
        REQUIRE(output_state.value.sensor_measurements.spo2 == 94);
      }

      // 4th entry
      uint32_t fourth_time = 40;
      synchronizer.input(fourth_time);

      auto fourth_status = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(fourth_status == output_ok); }
      THEN(
          "Only one schedule entry is advanced and the tag of the output state is "
          "cycle_measurements") {
        REQUIRE(output_state.tag == MessageTypes::cycle_measurements);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.cycle_measurements.rr == 20);
      }

      // 0th entry
      uint32_t final_time = 80;
      synchronizer.input(final_time);

      auto final_status = synchronizer.output(output_state);
      THEN("The output status returns ok") { REQUIRE(final_status == output_ok); }
      THEN(
          "Only one schedule entry is advanced and the tag of the output state is "
          "parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN(
          "The output StateSegment fields are set and match the parameters of the "
          "all_states object") {
        REQUIRE(output_state.value.parameters_request.fio2 == 56);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }
    }
  }
}