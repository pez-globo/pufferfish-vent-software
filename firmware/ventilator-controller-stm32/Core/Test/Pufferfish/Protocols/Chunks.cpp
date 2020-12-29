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

#include "Pufferfish/Protocols/Chunks.h"

#include "Pufferfish/Util/Vector.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO(
    "Protocols::ChunkSplitter behaves correctly across chunk boundary(256 bytes)", "[chunks]") {
  GIVEN("A chunk splitter") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;

    WHEN("the input data exceeds bounds after output is called, without delimiter") {
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus status;
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
      }
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      auto final_status = chunks.input(val);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }
  }
}

SCENARIO(
    "Protocols::ChunkSplitter behaves correctly across chunk boundary(102 bytes)", "[chunks]") {
  GIVEN("A chunk splitter") {
    constexpr size_t buffer_size = 102;
    PF::Protocols::ChunkSplitter<buffer_size, char> chunks;

    WHEN("the input data is within bounds") {
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus status;
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
      }

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
      }
    }

    WHEN("the input data exceeds bounds without output being called") {
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus status;
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
      }

      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
      }

      THEN("the final status should be invalid_length") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }

    WHEN("the input data exceeds bounds after output is called, without delimiter") {
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus status;
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
      }
      PF::Util::Vector<char, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      auto final_status = chunks.input(val);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }

    WHEN("output is called on an input of invalid length") {
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus status;
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
      }
      auto final_status = chunks.input(val);
      PF::Util::Vector<char, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);

      THEN("the final status should be invalid_length") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::invalid_length);
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }
    }

    WHEN("the input data exceeds bounds after output is called, with delimiter") {
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus status;
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
      }
      auto delim_status = chunks.input(0);
      PF::Util::Vector<char, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      auto final_status = chunks.input(val);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
        REQUIRE(delim_status == PF::Protocols::ChunkInputStatus::output_ready);
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::ok);
      }
    }
  }
}

SCENARIO("Protocols::ChunkMerger behaves correctly(30 bytes)", "[chunks]") {
  GIVEN("A chunk merger") {
    constexpr size_t buffer_size = 30;
    PF::Protocols::ChunkMerger chunks;

    WHEN("the input data is within bounds") {
      uint8_t val = 128;
      PF::Util::Vector<char, buffer_size> buffer;
      PF::IndexStatus index_status;
      for (size_t i = 0; i < buffer_size - 1; ++i) {
        index_status = buffer.push_back(val);
      }
      PF::Protocols::ChunkOutputStatus status = chunks.transform<buffer_size, char>(buffer);

      THEN("the final status should be ok") {
        REQUIRE(index_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::ChunkOutputStatus::ok);
      }
    }
  }
}

SCENARIO("Protocols::ChunkMerger behaves correctly (256 bytes)", "[chunks]") {
  GIVEN("A chunk merger") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkMerger chunks;

    WHEN("the input data exceeds bounds") {
      uint8_t val = 128;
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::IndexStatus index_status;
      for (size_t i = 0; i < buffer_size; ++i) {
        index_status = buffer.push_back(val);
      }
      PF::Protocols::ChunkOutputStatus status = chunks.transform<buffer_size, uint8_t>(buffer);

      THEN("the final status should be invalid_length") {
        REQUIRE(index_status == PF::IndexStatus::ok);
        REQUIRE(status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }
    }
  }
}
