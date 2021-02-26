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

#include "Pufferfish/Test/Util.h"
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
      bool input_overwritten;
      PF::Protocols::ChunkInputStatus input_status;
      for (size_t i = 0; i < buffer_size; ++i) {
        input_status = chunks.input(val, input_overwritten);
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

      auto final_status = chunks.input(val, input_overwritten);
      THEN("the final input status is equal to invalid_length") {
        REQUIRE(final_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }
  }
}

SCENARIO(
    "Protocols::ChunkSplitter correctly handles input chunks of acceptable lengths", "[chunks]") {
  GIVEN("A uint8_t chunksplitter with an empty internal buffer") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    PF::Protocols::ChunkInputStatus status;
    bool input_overwritten;

    WHEN("A delimiter is passed and output is called") {
      uint8_t delimiter = 0x00;
      status = chunks.input(delimiter, input_overwritten);
      PF::Util::ByteVector<buffer_size> output_buffer;

      auto output_status = chunks.output(output_buffer);

      THEN("The output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The output buffer is empty") { REQUIRE(output_buffer.empty() == true); }
    }
  }

  GIVEN("A uint8_t chunksplitter with an empty internal buffer of capacity 256 bytes") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    PF::Protocols::ChunkInputStatus status;
    PF::Protocols::ChunkOutputStatus output_status;
    PF::Util::ByteVector<buffer_size> output_buffer;
    bool input_overwritten;

    WHEN(
        "Output method is called after 256 non-delimiter bytes are passed as input, and 257th byte "
        "is passed as a delimiter") {
      for (size_t i = 0; i < buffer_size; ++i) {
        uint8_t val = 10;
        status = chunks.input(val, input_overwritten);
      }

      THEN("The input method reports ok status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
      }

      uint8_t delimiter = 0x00;
      status = chunks.input(delimiter, input_overwritten);

      THEN("The input method reports output_ready status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      output_status = chunks.output(output_buffer);

      THEN("The output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 10;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
    }
  }

  GIVEN("A uint8_t chunksplitter with an empty internal buffer of capacity 256 bytes") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    PF::Protocols::ChunkInputStatus status;
    PF::Protocols::ChunkOutputStatus output_status;
    PF::Util::ByteVector<buffer_size> output_buffer;
    bool input_overwritten;

    WHEN(
        "256 non-delimiter bytes followed by a delimiter byte, are passed as input, with output "
        "called between each input call") {
      uint8_t val = 10;
      for (size_t i = 0; i < buffer_size; ++i) {
        status = chunks.input(val, input_overwritten);
      }

      THEN("The input method reports ok status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
      }

      output_status = chunks.output(output_buffer);

      THEN("The output method reports waiting status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }

      THEN("The output buffer is empty") { REQUIRE(output_buffer.empty() == true); }

      uint8_t delimiter = 0x00;
      status = chunks.input(delimiter, input_overwritten);

      THEN("The input method reports output_ready status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      output_status = chunks.output(output_buffer);

      THEN("The output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 10;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
    }
  }
}

SCENARIO("Protocols::ChunkSplitter correctly handles unacceptably long input chunks", "[chunks]") {
  GIVEN("A uint8_t chunk splitter with a completely full internal buffer of 256 bytes") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus input_status;
    PF::Protocols::ChunkOutputStatus output_status;
    bool input_overwritten;
    for (size_t i = 0; i < buffer_size; ++i) {
      input_status = chunks.input(val, input_overwritten);
      REQUIRE(input_status == PF::Protocols::ChunkInputStatus::ok);
    }

    PF::Util::Vector<uint8_t, buffer_size> output_buffer;

    WHEN("257th non-delimiter byte is passed as input") {
      input_status = chunks.input(val, input_overwritten);
      THEN("The input method reports invalid_length status") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }
    }

    WHEN("More than 256 non-delimiter bytes are passed as input") {
      for (size_t i = 0; i < buffer_size; ++i) {
        input_status = chunks.input(val, input_overwritten);
        THEN(
            "The input method reports invalid_length status on 257th byte and for all the "
            "subsequent bytes") {
          REQUIRE(input_status == PF::Protocols::ChunkInputStatus::invalid_length);
        }
      }
    }

    WHEN(
        "More than 256 non-delimiter bytes are passed as input, after which a delimiter is passed "
        "and output is called") {
      for (size_t i = 0; i < buffer_size; ++i) {
        input_status = chunks.input(val, input_overwritten);
        THEN("The input method reports invalid_length status") {
          REQUIRE(input_status == PF::Protocols::ChunkInputStatus::invalid_length);
        }
      }

      THEN("The input method reports output ready status") {
        input_status = chunks.input(0, input_overwritten);
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      output_status = chunks.output(output_buffer);
      THEN("the output method reports invalid_length status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }

      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 128;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
    }

    WHEN("Output is called after a non-delimiter byte is passed as input") {
      input_status = chunks.input(val, input_overwritten);
      THEN("The input method reports invalid_length status") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::invalid_length);
      }

      output_status = chunks.output(output_buffer);
      THEN("the output method reports invalid_length status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }

      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 128;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
    }

    WHEN("The 257th delimiter byte is passed as input, and output is called") {
      input_status = chunks.input(0, input_overwritten);
      THEN("The input method reports output_ready status") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      output_status = chunks.output(output_buffer);
      THEN("the output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }

      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 128;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
    }
  }
}

SCENARIO(
    "Protocols:: After output is consumed, ChunkSplitter's behavior resets to that of a "
    "ChunkSplitter with an empty buffer",
    "[chunks]") {
  GIVEN(
      "A uint8_t ChunkSplitter completely filled with input data, and no delimiters are consumed "
      "with an output call") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus input_status;
    PF::Protocols::ChunkOutputStatus output_status;
    PF::Util::ByteVector<buffer_size> output_buffer;
    bool input_overwritten;
    for (size_t i = 0; i < buffer_size; ++i) {
      input_status = chunks.input(val, input_overwritten);
      REQUIRE(input_status == PF::Protocols::ChunkInputStatus::ok);
    }

    input_status = chunks.input(0, input_overwritten);
    REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
    output_status = chunks.output(output_buffer);

    WHEN(
        "10 bytes of non-delimiter bytes followed by a delimiter byte are passed as input, and "
        "output is called between each input") {
      constexpr size_t size = 10;

      for (size_t i = 0; i < size; ++i) {
        input_status = chunks.input(val, input_overwritten);
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::ok);
      }

      output_status = chunks.output(output_buffer);
      THEN("The output method reports waiting status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }

      input_status = chunks.input(0, input_overwritten);
      THEN("The input method reports output_ready status") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      output_status = chunks.output(output_buffer);

      THEN("The input method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }

      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < size - 1; ++i) {
          uint8_t val = 128;
          REQUIRE(output_buffer.operator[](i) == val);
        }
      }
    }
  }
}

SCENARIO("Protocols::Chunksplitter correctly handles delimeters other than 0x00", "[chunks]") {
  GIVEN(
      "A uint8_t chunksplitter with a completely full internal buffer of capacity 256 bytes, with "
      "no delimiters which is equal to 0x01") {
    constexpr size_t buffer_size = 256;
    uint8_t delimeter = 0x01;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks{delimeter};

    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    bool input_overwritten;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val, input_overwritten);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("The Output is called after the internal buffer is full") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("The output method returns waiting status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
      THEN("The output buffer is set to an empty buffer") { REQUIRE(buffer.empty() == true); }
    }

    WHEN("The output is called after delimeter equal to 0x01 is passed as input") {
      uint8_t new_val = 1;
      status = chunks.input(new_val, input_overwritten);
      THEN("the input method reports output ready status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to ok") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }

      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 128;
          REQUIRE(buffer.operator[](i) == val);
        }
      }
    }
  }

  GIVEN(
      "A uint8_t chunksplitter with a completely full internal buffer of capacity 256 bytes, with "
      "no delimiters which is equal to 0xff") {
    constexpr size_t buffer_size = 256;
    uint8_t delimeter = 0xff;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks{delimeter};

    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    bool input_overwritten;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val, input_overwritten);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("The Output is called after the internal buffer is full") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("The output method returns waiting status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
      THEN("The output buffer is set to an empty buffer") { REQUIRE(buffer.empty() == true); }
    }

    WHEN("The output is called after delimeter equal to 0xff is passed as input") {
      uint8_t new_val = 0xff;
      status = chunks.input(new_val, input_overwritten);
      THEN("the input method reports output ready status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output status is equal to ok") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }

      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 128;
          REQUIRE(buffer.operator[](i) == val);
        }
      }
    }
  }

  GIVEN(
      "A uint8_t chunksplitter with a completely full internal buffer of capacity 256 bytes, with "
      "include_delimeter bool equal to true") {
    constexpr size_t buffer_size = 256;
    uint8_t delimeter = 1;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks{delimeter, true};
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    bool input_overwritten;
    for (size_t i = 0; i < buffer_size; ++i) {
      status = chunks.input(val, input_overwritten);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("The output is called after the internal buffer is full") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }
      THEN("The output buffer is set to an empty buffer") { REQUIRE(buffer.empty() == true); }
    }

    WHEN("Output is called after passing delimiter equal to 0x01 as input") {
      uint8_t new_val = 1;
      status = chunks.input(new_val, input_overwritten);
      THEN("the input method reports invalid_length status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::invalid_length);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output method reports invalid length status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }

      THEN("The output buffer is as expected") {
        for (size_t i = 0; i < buffer_size; ++i) {
          uint8_t val = 128;
          REQUIRE(buffer.operator[](i) == val);
        }
      }
    }
  }

  GIVEN(
      "A uint8_t chunksplitter with a partially filled internal buffer of capacity 256 bytes, with "
      "include_delimeter bool equal to true") {
    constexpr size_t buffer_size = 256;
    uint8_t delimeter = 1;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks{delimeter, true};
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus status;
    bool input_overwritten;
    for (size_t i = 0; i < buffer_size / 2; ++i) {
      status = chunks.input(val, input_overwritten);
      REQUIRE(status == PF::Protocols::ChunkInputStatus::ok);
    }

    WHEN("The output is called after the internal buffer is partially filled") {
      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::waiting);
      }

      THEN("The output buffer is set to an empty buffer") { REQUIRE(buffer.empty() == true); }
    }

    WHEN("The delimiter equal to 0x01 is passed as input") {
      uint8_t new_val = 1;
      status = chunks.input(new_val, input_overwritten);
      THEN("the input method reports output_ready status") {
        REQUIRE(status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      PF::Util::Vector<uint8_t, buffer_size> buffer;
      PF::Protocols::ChunkOutputStatus output_status = chunks.output(buffer);
      THEN("the output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }

      THEN("The output buffer contains delimiter") {
        // first 128 bytes of the buffer are 0x80
        for (size_t i = 0; i < (buffer_size / 2) - 1; ++i) {
          uint8_t val = 128;
          REQUIRE(buffer.operator[](i) == val);
        }

        // last byte of the buffer is the delimiter
        REQUIRE(buffer.operator[](128) == 1);
      }
    }
  }
}

SCENARIO(
    "Protocols:: Chunksplitter correctly handles input of multiple delimited chunks before output "
    "consumes them",
    "[chunks]") {
  GIVEN("A uint8_t chunksplitter with an empty internal buffer of capacity 256 bytes") {
    constexpr size_t buffer_size = 256;
    PF::Protocols::ChunkSplitter<buffer_size, uint8_t> chunks;
    uint8_t val = 128;
    PF::Protocols::ChunkInputStatus input_status;
    PF::Protocols::ChunkOutputStatus output_status;
    PF::Util::ByteVector<buffer_size> output_buffer;
    bool input_overwritten;

    WHEN(
        "input to the chunksplitter is '0x01 0x02 0x03 0x00 0x04 0x05 0x00' and output is called "
        "between the delimiter chunks") {
      auto input_data = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x03, 0x00, 0x04, 0x05, 0x00);
      for (size_t i = 0; i < 4; ++i) {
        input_status = chunks.input(input_data[i], input_overwritten);
      }

      output_status = chunks.output(output_buffer);

      for (size_t i = 4; i < 7; ++i) {
        input_status = chunks.input(input_data[i], input_overwritten);
      }

      THEN("The input method reports output_ready status") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      output_status = chunks.output(output_buffer);

      THEN("the output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The output buffer is as expected") {
        auto expected = std::string("\x04\x05", 2);
        REQUIRE(output_buffer == expected);
      }
    }

    WHEN(
        "input to the chunksplitter is '0x01 0x02 0x03 0x00 0x04 0x05' and output is not called "
        "after passing delimeter") {
      auto input_data = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x03, 0x04, 0x05);

      // first 3 bytes are given as input
      for (size_t i = 0; i < 3; ++i) {
        input_status = chunks.input(input_data[i], input_overwritten);
        THEN("The input method reports ok status on the first 3 bytes") {
          REQUIRE(input_status == PF::Protocols::ChunkInputStatus::ok);
        }
      }

      // input is passed with delimiter
      input_status = chunks.input(0, input_overwritten);
      THEN("The input method reports output_ready status on the 4th byte") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      // input is called again
      for (size_t i = 3; i < input_data.size(); ++i) {
        input_status = chunks.input(input_data[i], input_overwritten);
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::ok);
      }

      THEN(
          "The input method reports ok status and sets the input_overwritten flag on the 4th "
          "byte") {
        REQUIRE(input_overwritten == true);
      }

      // input is passed with delimiter
      input_status = chunks.input(0, input_overwritten);
      THEN("The input method reports output_ready status on the delimiter byte") {
        REQUIRE(input_status == PF::Protocols::ChunkInputStatus::output_ready);
      }

      auto output_status = chunks.output(output_buffer);
      THEN("The output method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The output buffer only contains the last chunk '0x04 0x05' ") {
        auto expected = std::string("\x04\x05", 2);
        REQUIRE(output_buffer == expected);
      }
    }
  }
}

SCENARIO("Protocols::ChunkMerger behaves correctly", "[chunks]") {
  GIVEN("A ChunkMerger with delimiter equal to 0x00") {
    constexpr size_t buffer_size = 30;
    PF::Protocols::ChunkMerger chunk_merger{};
    PF::Protocols::ChunkOutputStatus output_status;
    bool input_overwritten;

    WHEN("A partially full buffer of size 10 bytes is passed as input to transform method") {
      constexpr size_t size = 10;
      PF::Util::ByteVector<size> buffer;
      PF::IndexStatus index_status;

      for (size_t i = 0; i < size - 1; ++i) {
        uint8_t val = 10;
        buffer.push_back(val);
      }

      output_status = chunk_merger.transform<size, uint8_t>(buffer);

      THEN("The transform method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The ChunkMerger appends a delimeter") {
        auto expected = std::string("\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x00", 10);
        REQUIRE(buffer == expected);
      }
    }

    WHEN("Completely full buffer of size 30 bytes is passed as input to transform method") {
      uint8_t val = 128;
      PF::Util::ByteVector<buffer_size> buffer;
      PF::IndexStatus index_status;

      for (size_t i = 0; i < buffer_size; ++i) {
        index_status = buffer.push_back(val);
        REQUIRE(index_status == PF::IndexStatus::ok);
      }

      output_status = chunk_merger.transform<buffer_size, uint8_t>(buffer);

      THEN("The transform method reports invalid length") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }
    }

    WHEN("An input data with it's last byte equal to the delimiter is passed to transform") {
      constexpr size_t size = 10;
      PF::Util::ByteVector<size> buffer;
      PF::IndexStatus index_status;

      for (size_t i = 0; i < size - 2; ++i) {
        uint8_t val = 10;
        buffer.push_back(val);
      }

      buffer.push_back(0x00);

      output_status = chunk_merger.transform<size, uint8_t>(buffer);

      THEN("The transform method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The ChunkMerger appends a delimeter") {
        auto expected = std::string("\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x00\x00", 10);
        REQUIRE(buffer == expected);
      }
    }

    WHEN("An input data with it's first byte equal to the delimiter is passed to transform") {
      constexpr size_t size = 10;
      PF::Util::ByteVector<size> buffer;
      PF::IndexStatus index_status;

      buffer.push_back(0x00);

      for (size_t i = 1; i < size - 1; ++i) {
        uint8_t val = 10;
        buffer.push_back(val);
      }

      output_status = chunk_merger.transform<size, uint8_t>(buffer);

      THEN("The transform method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The ChunkMerger appends a delimeter") {
        auto expected = std::string("\x00\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x00", 10);
        REQUIRE(buffer == expected);
      }
    }

    WHEN("An input data with multiple delimited chunks is passed as input to transform") {
      constexpr size_t size = 10;
      PF::Util::ByteVector<size> buffer;
      PF::IndexStatus index_status;

      auto data =
          PF::Util::make_array<uint8_t>(0x01, 0x02, 0x00, 0x03, 0x00, 0x04, 0x05, 0x00, 0x07);

      for (size_t i = 0; i < 9; ++i) {
        buffer.push_back(data[i]);
      }

      output_status = chunk_merger.transform<size, uint8_t>(buffer);

      THEN("The transform method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The ChunkMerger appends a delimeter") {
        auto expected = std::string("\x01\x02\x00\x03\x00\x04\x05\x00\x07\x00", 10);
        REQUIRE(buffer == expected);
      }
    }
  }

  GIVEN("A ChunkMerger with delimiter equal to 0x01") {
    constexpr size_t buffer_size = 30;
    PF::Protocols::ChunkMerger chunk_merger{0x01};
    PF::Protocols::ChunkOutputStatus output_status;
    bool input_overwritten;

    WHEN("A partially full buffer of size 10 bytes is passed as input to transform method") {
      constexpr size_t size = 10;
      PF::Util::ByteVector<size> buffer;
      PF::IndexStatus index_status;

      for (size_t i = 0; i < size - 1; ++i) {
        uint8_t val = 10;
        buffer.push_back(val);
      }

      output_status = chunk_merger.transform<size, uint8_t>(buffer);

      THEN("The transform method reports ok status") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::ok);
      }
      THEN("The ChunkMerger appends a delimeter") {
        auto expected = std::string("\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x01", 10);
        REQUIRE(buffer == expected);
      }
    }

    WHEN("Completely full buffer of size 30 bytes is passed as input to transform method") {
      uint8_t val = 128;
      PF::Util::ByteVector<buffer_size> buffer;
      PF::IndexStatus index_status;

      for (size_t i = 0; i < buffer_size; ++i) {
        index_status = buffer.push_back(val);
        REQUIRE(index_status == PF::IndexStatus::ok);
      }

      output_status = chunk_merger.transform<buffer_size, uint8_t>(buffer);

      THEN("The transform method reports invalid length") {
        REQUIRE(output_status == PF::Protocols::ChunkOutputStatus::invalid_length);
      }
    }
  }
}