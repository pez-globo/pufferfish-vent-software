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

using StateOutputScheduleEntry = PF::Protocols::StateOutputScheduleEntry<PF::Application::MessageTypes>;
using States = PF::Application::States;
using StateSegment = PF::Application::StateSegment;
using MessageTypes = PF::Application::MessageTypes;
namespace BE = PF::Driver::Serial::Backend;

SCENARIO("Protocols:: States input method correctly updates the internal state_segment field", "[states]") {
  GIVEN("A StateSynchronizer object constructed with an output schedule array of multiple message types") {
    using BackendStateSynchronizer = PF::Protocols::StateSynchronizer<
        States,
        StateSegment,
        MessageTypes,
        BE::state_sync_schedule.size()>;

    const BackendStateSynchronizer::InputStatus input_ok = BackendStateSynchronizer::InputStatus::ok;
    const BackendStateSynchronizer::InputStatus input_invalid = BackendStateSynchronizer::InputStatus::invalid_type;

    StateSegment input_state;
    States states{};

    BackendStateSynchronizer synchronizer{
        states, BE::state_sync_schedule};

    PF::HAL::MockTime time;

    WHEN("the current time is written to it") {
      const uint32_t ctime = 0x12345678;

      auto input_status = synchronizer.input(ctime);

      THEN("the final status should be ok") {
        REQUIRE(input_status == input_ok);
      }
    }

    WHEN("The tag of the input StateSegment is equal to parameters request") {
      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.ventilating = true;
      parameters_request.fio2 = 30;
      parameters_request.mode = VentilationMode_hfnc;
      input_state.set(parameters_request);

      auto input_status = synchronizer.input(input_state);

      THEN("The input status returns ok") {
        REQUIRE(input_status == input_ok);
      }
      THEN("The fields of the state object are updated") {
        auto parameters_request = states.parameters_request();
        REQUIRE(parameters_request.fio2 == 30);
        REQUIRE(parameters_request.mode == VentilationMode_hfnc);
      }
    }

    WHEN("The tag of the input StateSegment is equal to alarm limits request") {
      AlarmLimitsRequest alarm_limits_request = {};
      Range range = {};
      range.lower = 50;
      range.upper = 92;
      alarm_limits_request.has_fio2 = true;
      alarm_limits_request.fio2 = range;
      input_state.set(alarm_limits_request);

      auto input_status = synchronizer.input(input_state);

      THEN("The input status returns ok") {
        REQUIRE(input_status == input_ok);
      }
      THEN("The statesegment field values are updated") {
        auto alarm_limits_request = states.alarm_limits_request();
        REQUIRE(alarm_limits_request.fio2.lower == 50);
        REQUIRE(alarm_limits_request.fio2.upper == 92);
      }
    }

    WHEN("The input state is uninitalised") {
      auto input_status = synchronizer.input(input_state);

      THEN("input status returns invalid type") {
        REQUIRE(input_status == input_invalid);
      }
    }

    WHEN("The tag of the input state is of invalid message type") {
      Parameters parameters;
      memset(&parameters, 0, sizeof(parameters));
      parameters.fio2 = 60;
      parameters.flow = 40;
      parameters.mode = VentilationMode_hfnc;
      input_state.set(parameters);

      auto input_status = synchronizer.input(input_state);

      THEN("input status should be of invalid type") {
        REQUIRE(input_status == input_invalid);
      }
    }
  }
}

SCENARIO("Protocols::States output method correctly updates output state tag and field parameters", "[states]") {
  GIVEN("A StateSynchronizer object constructed with an output schedule array of single message type") {
    constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
        StateOutputScheduleEntry{5, MessageTypes::parameters_request},
        StateOutputScheduleEntry{6, MessageTypes::parameters_request}
    );

    using BackendStateSynchronizer = PF::Protocols::StateSynchronizer<
        States,
        StateSegment,
        MessageTypes,
        state_sync_schedule.size()>;

    const BackendStateSynchronizer::InputStatus input_ok = BackendStateSynchronizer::InputStatus::ok;
    const BackendStateSynchronizer::OutputStatus available = BackendStateSynchronizer::OutputStatus::available;
    const BackendStateSynchronizer::OutputStatus waiting = BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

    BackendStateSynchronizer synchronizer{
        states, state_sync_schedule};
    
    WHEN("The current time is greater than output state scheduler delay") {
      uint32_t time = 10;
      auto input_time = synchronizer.input(time);
      THEN("The input time status should be ok") {
        REQUIRE(input_time == input_ok);
      }

      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.ventilating = true;
      parameters_request.fio2 = 30;
      parameters_request.mode = VentilationMode_hfnc;
      input_state.set(parameters_request);

      auto input_segment = synchronizer.input(input_state);
      THEN("The input segment status should be ok") {
        REQUIRE(input_segment == input_ok);
        REQUIRE(input_state.tag == MessageTypes::parameters_request);
      }

      auto output_status = synchronizer.output(output_state);

      THEN("The ouptut status should be available") {
        REQUIRE(output_status == available);
      }
      THEN("The tag of the output StateSegment object is equal to parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN("The output StateSegment fields are set") {
        REQUIRE(output_state.value.parameters_request.ventilating == true);
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }

      uint32_t new_time = 10;
      synchronizer.input(new_time);
      auto status = synchronizer.output(output_state);

      THEN("The ouptut status should be waiting") {
        REQUIRE(status == waiting);
      }

      uint32_t final_time = 16;
      synchronizer.input(final_time);
      auto final_status = synchronizer.output(output_state);

      THEN("The output status should be available") {
        REQUIRE(final_status == available);
      }
      THEN("output state values are same") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
        REQUIRE(output_state.value.parameters_request.ventilating == true);
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }
    }
  }

  GIVEN("A StateSynchronizer object constructed with an output schedule array of multiple message types") {
    constexpr auto state_sync_schedule = PF::Util::make_array<const StateOutputScheduleEntry>(
        StateOutputScheduleEntry{1, MessageTypes::parameters_request},
        StateOutputScheduleEntry{2, MessageTypes::alarm_limits_request}
    );

    using BackendStateSynchronizer = PF::Protocols::StateSynchronizer<
        States,
        StateSegment,
        MessageTypes,
        state_sync_schedule.size()>;

    const BackendStateSynchronizer::InputStatus input_ok = BackendStateSynchronizer::InputStatus::ok;
    const BackendStateSynchronizer::OutputStatus available = BackendStateSynchronizer::OutputStatus::available;
    const BackendStateSynchronizer::OutputStatus waiting = BackendStateSynchronizer::OutputStatus::waiting;

    States states{};

    StateSegment input_state;
    StateSegment output_state;

    BackendStateSynchronizer synchronizer{
        states, state_sync_schedule};

    WHEN("The current time is greater than output state scheduler delay") {
      uint32_t time = 1;
      auto input_time = synchronizer.input(time);
      THEN("The input time status should be ok") {
        REQUIRE(input_time == input_ok);
      }

      ParametersRequest parameters_request;
      memset(&parameters_request, 0, sizeof(parameters_request));
      parameters_request.ventilating = true;
      parameters_request.fio2 = 30;
      parameters_request.mode = VentilationMode_hfnc;
      input_state.set(parameters_request);

      auto input_segment = synchronizer.input(input_state);
      THEN("The input segment status should be ok") {
        REQUIRE(input_segment == input_ok);
        REQUIRE(input_state.tag == MessageTypes::parameters_request);
      }

      auto output_status = synchronizer.output(output_state);

      THEN("The ouptut status should be available") {
        REQUIRE(output_status == available);
      }
      THEN("The tag of the output StateSegment object is equal to parameters_request") {
        REQUIRE(output_state.tag == MessageTypes::parameters_request);
      }
      THEN("The output StateSegment fields are set") {
        REQUIRE(output_state.value.parameters_request.ventilating == true);
        REQUIRE(output_state.value.parameters_request.fio2 == 30);
        REQUIRE(output_state.value.parameters_request.mode == VentilationMode_hfnc);
      }

      uint32_t new_time = 2;
      synchronizer.input(new_time);
      auto status = synchronizer.output(output_state);

      THEN("The ouptut status should be waiting") {
        REQUIRE(status == waiting);
      }

      uint32_t final_time = 4;
      synchronizer.input(final_time);
      auto final_status = synchronizer.output(output_state);

      THEN("The output status should be available") {
        REQUIRE(final_status == available);
      }
      THEN("The tag of the output StateSegment is equal to alarm_limits_request") {
        REQUIRE(output_state.tag == MessageTypes::alarm_limits_request);
      }
      THEN("The output StateSegment fields are 0") {
        REQUIRE(output_state.value.alarm_limits_request.fio2.lower == 0);
        REQUIRE(output_state.value.alarm_limits_request.fio2.upper == 0);
      }
    }
  }
}
