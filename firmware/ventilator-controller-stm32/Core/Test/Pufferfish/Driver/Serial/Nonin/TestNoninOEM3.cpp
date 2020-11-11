/// TestNoninOEM3.cpp
/// Unit tests to confirm behavior of Nonin OEM III checks.

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

#include "Pufferfish/Driver/Serial/Nonin/Device.h"
#include "Pufferfish/HAL/Mock/MockBufferedUART.h"
#include "Pufferfish/Util/Array.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

PF::Driver::Serial::Nonin::Device::PacketStatus waiting_status =
    PF::Driver::Serial::Nonin::Device::PacketStatus::waiting;

PF::Driver::Serial::Nonin::Device::PacketStatus available_status =
    PF::Driver::Serial::Nonin::Device::PacketStatus::available;

PF::Driver::Serial::Nonin::Device::PacketStatus missed_data_status =
    PF::Driver::Serial::Nonin::Device::PacketStatus::missed_data;

PF::Driver::Serial::Nonin::Device::PacketStatus framing_error_status =
    PF::Driver::Serial::Nonin::Device::PacketStatus::framing_error;

SCENARIO("No input data received from BufferedUART", "[NoninOEM3]") {
  PF::HAL::MockReadOnlyBufferedUART mock_uart;
  PF::Driver::Serial::Nonin::Device nonin_uart(mock_uart);
  PF::Driver::Serial::Nonin::PacketMeasurements sensor_measurements{};
  PF::Driver::Serial::Nonin::Device::PacketStatus return_status;

  GIVEN("Input data received from BufferedUART is empty") {
    WHEN("Device::output is invoked") {
      return_status = nonin_uart.output(sensor_measurements);
      THEN("Device output shall return waiting status") {
        REQUIRE(return_status == waiting_status);
      }
    }
  }
}

SCENARIO("Complete packet is not available", "[NoninOEM3]") {
  PF::HAL::MockReadOnlyBufferedUART mock_uart;
  PF::Driver::Serial::Nonin::Device nonin_uart(mock_uart);
  PF::Driver::Serial::Nonin::PacketMeasurements sensor_measurements{};
  PF::Driver::Serial::Nonin::Device::PacketStatus return_status;

  GIVEN("4 bytes of BufferedUART data") {
    auto uart_data = PF::Util::make_array<uint8_t>(0x01, 0x81, 0x00, 0x00);
    uint8_t index = 0;
    for (index = 0; index < 4; index++) {
      mock_uart.set_read(uart_data[index]);
    }
    WHEN("4 bytes of data received from BufferedUART") {
      THEN("Device::output shall return waiting status") {
        for (index = 0; index < 4; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
      }
    }
  }

  GIVEN("2 valid frames of data from BufferedUART data") {
    auto uart_data =
        PF::Util::make_array<uint8_t>(0x01, 0x81, 0x01, 0x00, 0x83, 0x01, 0x80, 0x01, 0x48, 0xCA);
    uint8_t index = 0;
    for (index = 0; index < 10; index++) {
      mock_uart.set_read(uart_data[index]);
    }
    WHEN("4 bytes of data received from BufferedUART") {
      THEN("Device::output shall return waiting status") {
        for (index = 0; index < 4; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
      }
    }
    AND_WHEN("5th byte received from BufferedUART") {
      for (index = 0; index < 4; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be waiting") {
        return_status = nonin_uart.output(sensor_measurements);
        REQUIRE(return_status == waiting_status);
      }
    }
    AND_WHEN("2nd frame data is received from BufferedUART") {
      for (index = 0; index < 5; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be waiting") {
        for (index = 5; index < 10; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
      }
    }
  }

  GIVEN("2 frames of data from BufferedUART data with checksum error in 2nd frame") {
    auto uart_data =
        PF::Util::make_array<uint8_t>(0x01, 0x81, 0x01, 0x00, 0x83, 0x01, 0x80, 0x01, 0x48, 0xCB);
    uint8_t index = 0;
    for (index = 0; index < 10; index++) {
      mock_uart.set_read(uart_data[index]);
    }

    WHEN("4 bytes of data received from BufferedUART") {
      THEN("Device::output shall return waiting status") {
        for (index = 0; index < 4; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
      }
    }
    AND_WHEN("5th byte received from BufferedUART") {
      for (index = 0; index < 4; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be waiting") {
        return_status = nonin_uart.output(sensor_measurements);
        REQUIRE(return_status == waiting_status);
      }
    }
    AND_WHEN("2nd frame data is received from BufferedUART") {
      for (index = 0; index < 5; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be waiting") {
        for (index = 5; index < 9; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
      }
    }
    AND_WHEN("2nd frame checksum byte(i.e., 10th byte) is received from BufferedUART") {
      for (index = 0; index < 9; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be framing_error on checksum error") {
        return_status = nonin_uart.output(sensor_measurements);
        REQUIRE(return_status == framing_error_status);
      }
    }
  }

  GIVEN("BufferedUART data with status byte error (Bit-7 is low) in 2nd frame") {
    auto uart_data =
        PF::Util::make_array<uint8_t>(0x01, 0x81, 0x01, 0x00, 0x83, 0x01, 0x7F, 0x01, 0x48, 0xCA);
    uint8_t index = 0;
    for (index = 0; index < 10; index++) {
      mock_uart.set_read(uart_data[index]);
    }

    WHEN("4 bytes of data received from BufferedUART") {
      THEN("Device::output shall return waiting status") {
        for (index = 0; index < 4; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
      }
    }
    AND_WHEN("5th byte received from BufferedUART") {
      for (index = 0; index < 4; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be waiting") {
        return_status = nonin_uart.output(sensor_measurements);
        REQUIRE(return_status == waiting_status);
      }
    }
    AND_WHEN("2nd frame data is received from BufferedUART") {
      for (index = 0; index < 5; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be waiting") {
        for (index = 5; index < 9; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
      }
    }
    AND_WHEN("10th byte is received from BufferedUART") {
      for (index = 0; index < 9; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("return_status of Device::output shall be framing_error on status byte error") {
        return_status = nonin_uart.output(sensor_measurements);
        REQUIRE(return_status == framing_error_status);
      }
    }
  }
}

SCENARIO("Validate the Nonin OEM III with invalid data received from BufferedUART") {
  PF::HAL::MockReadOnlyBufferedUART mock_uart;
  PF::Driver::Serial::Nonin::Device nonin_uart(mock_uart);
  PF::Driver::Serial::Nonin::PacketMeasurements sensor_measurements{};
  PF::Driver::Serial::Nonin::Device::PacketStatus return_status;
  GIVEN("Valid 24 frames with 23 Frames of first packet and 1 frame of second packet ") {
    auto uart_data = PF::Util::make_array<uint8_t>(
        0x01,
        0x81,
        0x01,
        0x00,
        0x83,  /// HR MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// HR LSB
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2
        0x01,
        0x80,
        0x01,
        0x30,
        0xB2,  /// REV
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2-D
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2 Fast
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2 B-B
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// E-HR MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// E-HR LSB
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// E-SpO2
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// E-SpO2-D
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// HR-D MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// HR-D LSB
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// E-HR-D MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// E-HR-D LSB
        /// New packet first frame due to loss in previous packet
        0x01,
        0x81,
        0x01,
        0x00,
        0x83  /// HR MSB
    );
    uint16_t index = 0;
    for (index = 0; index < 120; index++) {
      mock_uart.set_read(uart_data[index]);
    }
    WHEN("0 to 115 bytes of data receiving from bufferedUART") {
      THEN("Device::output shall return waiting status for each frame received 0f 23 frames") {
        for (index = 0; index < 115; index++) {
          if ((index > 0) && (index % 5 == 0)) {
            REQUIRE(return_status == waiting_status);
          }
          return_status = nonin_uart.output(sensor_measurements);
        }
      }
    }
    AND_WHEN("116 to 120 bytes of data receiving from bufferedUART") {
      for (index = 0; index < 115; index++) {
        return_status = nonin_uart.output(sensor_measurements);
      }
      for (index = 115; index < 120; index++) {
        return_status = nonin_uart.output(sensor_measurements);
      }
      THEN("shall return missed_data due to loss of 2 frames") {
        REQUIRE(return_status == missed_data_status);
      }
    }
  }
}

SCENARIO("Validate NoninOEM3 for valid packet data", "[NoninOEM3]") {
  GIVEN("125 bytes of BufferedUART data") {
    auto uart_data = PF::Util::make_array<uint8_t>(
        0x01,
        0x81,
        0x01,
        0x00,
        0x83,  /// HR MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// HR LSB
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2
        0x01,
        0x80,
        0x01,
        0x30,
        0xB2,  /// REV
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2-D
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2 Fast
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// SpO2 B-B
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// E-HR MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// E-HR LSB
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// E-SpO2
        0x01,
        0x80,
        0x01,
        0x61,
        0xE3,  /// E-SpO2-D
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// HR-D MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// HR-D LSB
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// E-HR-D MSB
        0x01,
        0x80,
        0x01,
        0x48,
        0xCA,  /// E-HR-D LSB
        0x01,
        0x80,
        0x01,
        0x00,
        0x82,  /// reserved
        0x01,
        0x80,
        0x01,
        0x00,
        0x82  /// reserved
    );
    PF::HAL::MockReadOnlyBufferedUART mock_uart;
    uint8_t index = 0;
    for (index = 0; index < 125; index++) {
      mock_uart.set_read(uart_data[index]);
    }
    PF::Driver::Serial::Nonin::Device nonin_uart(mock_uart);
    PF::Driver::Serial::Nonin::PacketMeasurements sensor_measurements{};
    PF::Driver::Serial::Nonin::Device::PacketStatus return_status;

    WHEN("Device::output is invoked for 124 bytes valid bytes read") {
      THEN("Device::output shall return waiting status") {
        for (index = 0; index < 124; index++) {
          return_status = nonin_uart.output(sensor_measurements);
          REQUIRE(return_status == waiting_status);
        }
        return_status = nonin_uart.output(sensor_measurements);
        REQUIRE(return_status == available_status);
      }
    }
    AND_WHEN("return_status for valid 124 bytes data is waiting") {
      for (index = 0; index < 124; index++) {
        nonin_uart.output(sensor_measurements);
      }
      THEN("invoke the nonin_uart to read 125th byte data") {
        return_status = nonin_uart.output(sensor_measurements);
        REQUIRE(return_status == available_status);
        REQUIRE(sensor_measurements.heart_rate == 72);
        REQUIRE(sensor_measurements.heart_rate_d == 72);
        REQUIRE(sensor_measurements.e_heart_rate == 72);
        REQUIRE(sensor_measurements.e_heart_rate_d == 72);
        REQUIRE(sensor_measurements.spo2 == 97);
        REQUIRE(sensor_measurements.spo2_d == 97);
        REQUIRE(sensor_measurements.e_spo2 == 97);
        REQUIRE(sensor_measurements.e_spo2_d == 97);
        REQUIRE(sensor_measurements.spo2_d_beat == 97);
        REQUIRE(sensor_measurements.spo2_d_fast == 97);
        REQUIRE(sensor_measurements.nonin_oem_revision == 48);
      }
    }
  }
}
