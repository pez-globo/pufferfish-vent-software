/// TestFrameBuffer.cpp
/// Unit tests to confirm behavior of Frame Buffer checks.

// Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.

#include <array>

#include "Pufferfish/Driver/Serial/Nonin/FrameBuffer.h"
#include "Pufferfish/Util/Array.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("5 bytes of valid data.", "[NoninOEM3]") {
  GIVEN("5 bytes of data : 0x01, 0x83, 0x01, 0x80, 0x00") {
    uint8_t index = 0;
    auto input_data = PF::Util::make_array<uint8_t>(0x01, 0x83, 0x01, 0x80, 0x00);
    PF::Driver::Serial::Nonin::FrameBuffer frame_buffer;
    PF::BufferStatus buffer_status;
    Frame frame_data;

    WHEN("FrameBuffer::input of 4 bytes of data") {
      THEN("Frame buffer status shall be partial") {
        for (index = 0; index < 4; index++) {
          buffer_status = frame_buffer.input(input_data[index]);
          REQUIRE(buffer_status == PF::BufferStatus::partial);
        }
      }
    }
    AND_WHEN("FrameBuffer::input of 5th byte of data") {
      for (index = 0; index < 4; index++) {
        buffer_status = frame_buffer.input(input_data[index]);
      }
      THEN("Frame buffer input status shall be ok") {
        buffer_status = frame_buffer.input(input_data[4]);
        REQUIRE(buffer_status == PF::BufferStatus::ok);
      }
    }
    AND_WHEN("FrameBuffer::input status is ok") {
      for (index = 0; index < 5; index++) {
        buffer_status = frame_buffer.input(input_data[index]);
      }
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("Invoke FrameBuffer::Output shall return status ok") {
        buffer_status = frame_buffer.output(frame_data);
        REQUIRE(buffer_status == PF::BufferStatus::ok);
        for (index = 0; index < 5; index++) {
          REQUIRE(frame_data[index] == input_data[index]);
        }
      }
    }
  }
}

SCENARIO("10 bytes of data for shift_left and reset.", "[NoninOEM3]") {
  GIVEN("A 10 bytes of data : 0x01 0x81 0x01 0x00 0x83 0x01 0x80 0x01 0x00 0x82") {
    uint8_t index = 0;
    auto input_data =
        PF::Util::make_array<uint8_t>(0x01, 0x81, 0x01, 0x00, 0x83, 0x01, 0x80, 0x01, 0x00, 0x82);
    PF::Driver::Serial::Nonin::FrameBuffer frame_buffer;
    PF::BufferStatus buffer_status;
    Frame frame_data;

    WHEN("FrameBuffer::input of 4 bytes of data") {
      THEN("Frame buffer status shall be partial") {
        for (index = 0; index < 4; index++) {
          buffer_status = frame_buffer.input(input_data[index]);
          REQUIRE(buffer_status == PF::BufferStatus::partial);
        }
      }
    }
    AND_WHEN("FrameBuffer::output invoked on buffer input status is partial") {
      for (index = 0; index < 4; index++) {
        buffer_status = frame_buffer.input(input_data[index]);
      }
      THEN("FrameBuffer::output status shall be partial") {
        buffer_status = frame_buffer.output(frame_data);
        REQUIRE(buffer_status == PF::BufferStatus::partial);
      }
    }
    WHEN("FrameBuffer::input of 5th byte of data") {
      for (index = 0; index < 4; index++) {
        buffer_status = frame_buffer.input(input_data[index]);
      }
      THEN("Frame buffer input status shall be ok") {
        buffer_status = frame_buffer.input(input_data[index]);
        REQUIRE(buffer_status == PF::BufferStatus::ok);
      }
    }
    AND_WHEN("FrameBuffer::input status is ok") {
      for (index = 0; index < 5; index++) {
        buffer_status = frame_buffer.input(input_data[index]);
      }
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("Invoke FrameBuffer Output shall return status ok") {
        buffer_status = frame_buffer.output(frame_data);
        REQUIRE(buffer_status == PF::BufferStatus::ok);
        for (index = 0; index < 5; index++) {
          REQUIRE(frame_data[index] == input_data[index]);
        }
      }
    }
    AND_WHEN("FrameBuffer output status is ok") {
      for (index = 0; index < 5; index++) {
        frame_buffer.input(input_data[index]);
      }
      buffer_status = frame_buffer.output(frame_data);
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("Invoke FrameBuffer input shall return buffer input status is full") {
        buffer_status = frame_buffer.input(input_data[5]);
        REQUIRE(buffer_status == PF::BufferStatus::full);
      }
    }
    AND_WHEN("FrameBuffer output status is ok") {
      for (index = 0; index < 5; index++) {
        frame_buffer.input(input_data[index]);
      }
      buffer_status = frame_buffer.output(frame_data);
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("Invoke FrameBuffer shift_left and on invocation of FrameBuffer input shall return ok") {
        frame_buffer.shift_left();
        buffer_status = frame_buffer.input(input_data[5]);
        REQUIRE(buffer_status == PF::BufferStatus::ok);
      }
    }
    AND_WHEN("FrameBuffer input status is ok after FrameBuffer::left_shift") {
      for (index = 0; index < 5; index++) {
        frame_buffer.input(input_data[index]);
      }
      frame_buffer.shift_left();
      buffer_status = frame_buffer.input(input_data[5]);
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("Invoke FrameBuffer::output shall return ok and data shall be shifted by 1 byte") {
        buffer_status = frame_buffer.output(frame_data);
        REQUIRE(buffer_status == PF::BufferStatus::ok);
        for (index = 0; index < 5; index++) {
          REQUIRE(frame_data[index] == input_data[index + 1]);
        }
      }
    }
    WHEN("FrameBuffer::output status is ok") {
      for (index = 0; index < 5; index++) {
        frame_buffer.input(input_data[index]);
      }
      buffer_status = frame_buffer.output(frame_data);
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("FrameBuffer reset and FrameBuffer input for 4 bytes shall return partial") {
        frame_buffer.reset();
        for (index = 5; index < 9; index++) {
          buffer_status = frame_buffer.input(input_data[index]);
          REQUIRE(buffer_status == PF::BufferStatus::partial);
        }
      }
    }
    AND_WHEN("FrameBuffer::output status is ok") {
      for (index = 0; index < 5; index++) {
        frame_buffer.input(input_data[index]);
      }
      buffer_status = frame_buffer.output(frame_data);
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("FrameBuffer reset and FrameBuffer input for 5 bytes shall return ok") {
        frame_buffer.reset();
        for (index = 5; index < 10; index++) {
          buffer_status = frame_buffer.input(input_data[index]);
        }
        REQUIRE(buffer_status == PF::BufferStatus::ok);
      }
    }
    AND_WHEN("FrameBuffer input status is ok") {
      for (index = 0; index < 5; index++) {
        buffer_status = frame_buffer.input(input_data[index]);
      }
      frame_buffer.reset();
      for (index = 5; index < 10; index++) {
        buffer_status = frame_buffer.input(input_data[index]);
      }
      REQUIRE(buffer_status == PF::BufferStatus::ok);
      THEN("FrameBuffer output shall return ok") {
        buffer_status = frame_buffer.output(frame_data);
        REQUIRE(buffer_status == PF::BufferStatus::ok);
        for (index = 0; index < 5; index++) {
          REQUIRE(frame_data[index] == input_data[index + 5]);
        }
      }
    }
  }
}
