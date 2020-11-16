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
#include "Pufferfish/Driver/Serial/Backend/Backend.h"
#include "Pufferfish/Util/Array.h"
#include "Pufferfish/HAL/Mock/MockTime.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

using BackendStateSynchronizer = PF::Protocols::StateSynchronizer<
    PF::Application::States,
    PF::Application::StateSegment,
    PF::Application::MessageTypes,
    PF::Driver::Serial::Backend::state_sync_schedule.size()>;

SCENARIO("Protocols::State behaves correctly", "[Datagram]") {
  GIVEN("A State Synchronizer") {
    PF::Application::States states;
    BackendStateSynchronizer synchronizer{states, PF::Driver::Serial::Backend::state_sync_schedule};

    WHEN("data is written to it") {
      // PF::HAL::MockTime time;
      // const uint32_t ctime = 7867643;
      // time.set_millis(ctime);
      // auto current_time = time.millis();
      // THEN("the final status should be ok") {
      //   REQUIRE(current_time == ctime);
      // }
    }

  }

}
