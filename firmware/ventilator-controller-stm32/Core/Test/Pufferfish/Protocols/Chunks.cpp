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

#include "Pufferfish/Util/Array.h"
#include "Pufferfish/Util/Vector.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO(
    "Protocols::ChunkSplitter behaves correctly across chunk boundary(102 bytes)", "[chunks]") {
  GIVEN("A char chunk splitter with internal buffer equal to 102 bytes") {
    constexpr size_t buffer_size = 102;
    PF::Protocols::ChunkSplitter<buffer_size, char> chunks;

    WHEN("the input data exceeds bounds after output is called, without delimiter") {
      uint8_t val = 128;
      PF::Protocols::ChunkInputStatus input_status;
      for (size_t i = 0; i < buffer_size; ++i) {
        input_status = chunks.input(val);
        THEN("the input status is equal to ok") {
          REQUIRE(input_status == PF::Protocols::ChunkInputStatus::ok);
        }
      }

      PF::Util::Vector<char, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to waiting") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
      THEN("the output buffer is empty") { REQUIRE(buffer.empty() == true); }

      auto final_status = chunks.input(val);
      THEN("the final input status is equal to invalid_length") {
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }
  }
}

SCENARIO(
    "Protocols::ChunkSplitter behaves correctly across chunk boundary(256 bytes)", "[chunks]") {
  GIVEN("A chunk splitter of buffer size equal to 256 bytes, that is filled with input data") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("the input data exceeds bounds without output being called") {
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
        THEN("the final input status is equal to invalid_length") {
          REQUIRE(status == PF::Protocols::ChunkInputStatus::invalid_length);
        }
      }
    }
  }

  GIVEN("A chunk splitter of buffer size equal to 256 bytes, that is filled with input data") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("the input data exceeds bounds after output is called, without delimiter") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);

      auto final_status = chunks.input(val);

      THEN("the output status is equal to waiting") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }

      THEN("the final input status is equal to invalid_length") {
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }
  }

  GIVEN("A chunk splitter of buffer size equal to 256 bytes, that is filled with input data") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("Another byte is given as input") {
      auto input_status = chunks.input(val);
      THEN("the final input status should be invalid_length") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }

    WHEN("output is called after the input status returns invalid length") {
      auto input_status = chunks.input(val);
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to invalid_length") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }
    }

    WHEN("the input receives delimeter") {
      auto input_status = chunks.input(0);
      THEN("input status is equal to output_ready") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to ok") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }

      auto final_status = chunks.input(val);

      THEN("the final input status is equal to ok") {
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::ok);
      }
    }
  }

  GIVEN(
      "A chunk splitter of buffer size equal to 256 bytes, that is filled completely with input "
      "data") {
    constexpr size_t buffer_size = 156;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("delimeter is passed after the buffer is full") {
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val);
        THEN("the initial status is equal to ok") {
          REQUIRE(status == PF::Protocols::ChunkInputStatus::invalid_length);
        }
      }
      THEN("the final input status is equal to output ready") {
        status = chunks.input(0);
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }
    }
  }

  GIVEN(
      "A uint8_t chunk splitter with internal buffer of 256 bytes, with delimeter equal to 0x01") {
    constexpr size_t buffer_size = 156;
    uint8_t delimeter = 0x01;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks{delimeter};

    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("output is called after input status is ok") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to waiting") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
    }

    WHEN("input receives delimeter equal to 1 byte") {
      uint8_t new_val = 1;
      status = chunks.input(new_val);
      THEN("the input status shall be output ready") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to ok") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
    }
  }

  GIVEN(
      "A uint8_t chunk splitter with internal buffer of 256 bytes, with delimeter equal to 255 "
      "bytes") {
    constexpr size_t buffer_size = 156;
    uint8_t delimeter = 0xff;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks{delimeter};

    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("output is called after input status is ok") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to waiting") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
    }

    WHEN("input receives delimeter equal to 1 byte") {
      uint8_t new_val = 255;
      status = chunks.input(new_val);
      THEN("the input status shall be output ready") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to ok") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
    }
  }

  GIVEN(
      "A uint8_t chunk splitter with internal buffer of 256 bytes, with include_delimeter bool "
      "equal to true") {
    constexpr size_t buffer_size = 156;
    uint8_t delimeter = 1;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks{delimeter, true};
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("output is called after input status is ok") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to waiting") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
    }

    WHEN("input receives delimeter equal to 1 byte") {
      uint8_t new_val = 1;
      status = chunks.input(new_val);
      THEN("the input status shall be invalid length") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::invalid_length);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to invalid length") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }
    }
  }

  GIVEN("A chunk splitter of buffer size equal to 256 bytes, that is filled with input data") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    auto input_data = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x03, 0x04);
    PF::Protocols::ChunkInputStatus status;

    WHEN("The output is called after input of 3 chunks") {
      for (size_t i = 0; i < input_data.size(); ++i) {
        status = chunks.input(input_data[0]);
        THEN("the initial status is equal to ok") {
          REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
        }
      }

      for (size_t i = 0; i < input_data.size(); ++i) {
        status = chunks.input(input_data[1]);
        THEN("the initial status is equal to ok") {
          REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
        }
      }

      for (size_t i = 0; i < input_data.size(); ++i) {
        status = chunks.input(input_data[2]);
        THEN("the initial status is equal to ok") {
          REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
        }
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to waiting") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
    }
  }
}

SCENARIO("Protocols::ChunkMerger behaves correctly", "[chunks]") {
  GIVEN("A chunk merger of internal buffer size equal to 30 bytes") {
    constexpr size_t buffer_size = 30;
    PF::Protocols::ChunkMerger chunks;

    WHEN("the input data given is within bounds") {
      uint8_t val = 128;
      PF::Util::Vector<char, buffer_size> buffer;
      PF::IndexStatus index_status;
      for (size_t i = 0; i < buffer_size - 1; ++i) {
        index_status = buffer.push_back(val);
        THEN("the input status should be ok") { REQUIRE(index_status == PF::IndexStatus::ok); }
      }
      PF::Protocols::ChunkOutputStatus status = chunks.transform<buffer_size, char>(buffer);

      THEN("the final status should be ok") {
        REQUIRE(status == PF::Protocols::ChunkOutputStatus::ok);
      }
    }
  }

  GIVEN("A chunk merger of internal buffer size equal to 256 bytes") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkMerger chunks;

    WHEN("The output buffer cannot hold enough data") {
      uint8_t val = 128;
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::IndexStatus index_status;
      for (size_t i = 0; i < buffer_size; ++i) {
        index_status = buffer.push_back(val);
        THEN("the input status should be ok") { REQUIRE(index_status == PF::IndexStatus::ok); }
      }
      PF::Protocols::ChunkOutputStatus status = chunks.transform<buffer_size, uint8_t>(buffer);

      THEN("the transform status should be invalid_length") {
        REQUIRE(status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }
    }

    WHEN("The buffer is available, delimeter is added to the buffer") {
      uint8_t val = 128;
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::IndexStatus index_status;
      for (size_t i = 0; i < buffer_size - 4; ++i) {
        index_status = buffer.push_back(val);
        THEN("the input status should be ok") { REQUIRE(index_status == PF::IndexStatus::ok); }
      }

      PF::Protocols::ChunkOutputStatus status = chunks.transform<buffer_size, uint8_t>(buffer);
      THEN("the transform status should be ok") {
        REQUIRE(status == PF::Protocols::ChunkOutputStatus::ok);
      }

      index_status = buffer.push_back(0);
      THEN("the input status should be ok") { REQUIRE(index_status == PF::IndexStatus::ok); }
      PF::Protocols::ChunkOutputStatus transform_status =
          chunks.transform<buffer_size, uint8_t>(buffer);

      THEN("the final transform status should be ok") {
        REQUIRE(transform_status == PF::Protocols::ChunkOutputStatus::ok);
      }
    }

    WHEN("The buffer is full, delimeter is not added to the buffer") {
      uint8_t val = 128;
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::IndexStatus index_status;
      for (size_t i = 0; i < buffer_size; ++i) {
        index_status = buffer.push_back(val);
        THEN("the input status should be ok") { REQUIRE(index_status == PF::IndexStatus::ok); }
      }

      PF::Protocols::ChunkOutputStatus status = chunks.transform<buffer_size, uint8_t>(buffer);

      index_status = buffer.push_back(0);
      THEN("the final status should be out of bounds") {
        REQUIRE(index_status == PF::IndexStatus::out_of_bounds);
      }
    }
  }
}
