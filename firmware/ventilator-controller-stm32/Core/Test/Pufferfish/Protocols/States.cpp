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
#include "Pufferfish/Protocols/Chunks.h"
#include "Pufferfish/Application/States.h"
#include "Pufferfish/BackendDefs.h"
#include "Pufferfish/Util/Array.h"
#include "Pufferfish/HAL/Mock/MockTime.h"
#include "Pufferfish/HAL/CRCChecker.h"

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

    WHEN("the current time is written to it") {
      PF::HAL::MockTime time;
      const uint32_t ctime = 7867643;
      time.set_millis(ctime);
      auto current_time = time.millis();
      THEN("the final status should be ok") {
        REQUIRE(current_time == ctime);
      }
    }

    WHEN("data is written to it") {
      PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
      constexpr size_t buffer_size = 256;
      PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus status;
      for(uint8_t i = 1; i < 100; ++i) {
          status = chunks.input(val);
      }

    //   // BackendMessage message;
    //   // auto output_status = receiver.output(message);

    //   PF::HAL::MockTime time;
    //   const uint32_t ctime = 7867643;
    //   time.set_millis(ctime);
    //   auto current_time = time.millis();
    //   THEN("the final status should be ok") {
    //     REQUIRE(current_time == ctime);
    //   }
    }

  }

}
