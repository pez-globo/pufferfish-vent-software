/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Chunks.cpp
 *
 *  Created on: Nov 17, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of the backend
 *
 */

#include "Pufferfish/Driver/Serial/Backend/Backend.h"
#include "Pufferfish/HAL/CRCChecker.h"
#include "Pufferfish/Test/Util.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Serial::Backend behaves correctly", "[Backend]") {
  GIVEN("A Backend instance") {
    PF::HAL::SoftCRC32 crc32c{PF::HAL::crc32c_params};
    PF::Application::States all_states;
    Pufferfish::Driver::Serial::Backend::BackendReceiver backend_receiver{crc32c};

    WHEN("a valid body is parsed") {
      THEN("the final statuses, payload and length should be ok") {
      }
    }
  }
}
