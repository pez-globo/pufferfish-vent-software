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

using BackendStateSynchronizer = PF::Protocols::StateSynchronizer<
    PF::Application::States,
    PF::Application::StateSegment,
    PF::Application::MessageTypes,
    PF::Driver::Serial::Backend::state_sync_schedule.size()>;

SCENARIO("Protocols::State behaves correctly", "[Datagram]") {
  GIVEN("A Mock Time object") {
    PF::HAL::MockTime time;

    WHEN("the current time is written to it") {
      const uint32_t ctime = 0x12345678;
      time.set_millis(ctime);
      auto current_time = time.millis();
      THEN("the final time should be the same") { REQUIRE(current_time == ctime); }
    }
  }

  GIVEN("A State Synchronizer") {
    PF::Application::States states;

    WHEN("the current time is written to it") {
      const uint32_t ctime = 0x12345678;
      BackendStateSynchronizer synchronizer{
          states, PF::Driver::Serial::Backend::state_sync_schedule};
      auto input_status = synchronizer.input(ctime);
      THEN("the final status should be ok") {
        REQUIRE(input_status == BackendStateSynchronizer::InputStatus::ok);
      }
    }

    WHEN("single state is written to it") {
      PF::Application::StateSegment input;
      ParametersRequest parameters_request;
      parameters_request.ventilating = true;
      parameters_request.fio2 = 40;
      parameters_request.mode = VentilationMode_hfnc;
      parameters_request.flow = 60;
      input.tag = PF::Application::MessageTypes::parameters_request;

      BackendStateSynchronizer synchronizer{
          states, PF::Driver::Serial::Backend::state_sync_schedule};

      uint32_t ctime = 8;
      auto input_ctime_status1 = synchronizer.input(ctime);
      auto input_status = synchronizer.input(input);

      PF::Application::StateSegment output;
      output.tag = PF::Application::MessageTypes::parameters_request;
      auto output_status1 = synchronizer.output(output);

      ctime += 10;
      auto input_ctime_status2 = synchronizer.input(ctime);
      auto output_status2 = synchronizer.output(output);

      auto expected_output = states.parameters_request();

      THEN("the final status should be ok") {
        REQUIRE(input_ctime_status1 == BackendStateSynchronizer::InputStatus::ok);
        REQUIRE(input_status == BackendStateSynchronizer::InputStatus::ok);
        REQUIRE(output_status1 == BackendStateSynchronizer::OutputStatus::waiting);
        REQUIRE(input_ctime_status2 == BackendStateSynchronizer::InputStatus::ok);
        REQUIRE(output_status2 == BackendStateSynchronizer::OutputStatus::available);
      }
    }
  }
}
