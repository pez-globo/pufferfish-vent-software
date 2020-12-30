/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * SensirionDevice.cpp
 *
 *  Created on: Dec 1, 2020
 *      Author: Rohan Purohit
 *
 * Unit tests to confirm behavior Sensirion Device
 *
 */
#include "Pufferfish/Driver/I2C/SensirionDevice.h"

#include "Pufferfish/Driver/I2C/SFM3019/Types.h"
#include "Pufferfish/HAL/CRCChecker.h"
#include "Pufferfish/HAL/Mock/MockI2CDevice.h"
#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("SensirionDevice:: Sensirion device behaves properly", "[sensiriondevice]") {
  GIVEN("A Sensirion device") {
    using Command = PF::Driver::I2C::SFM3019::Command;

    PF::HAL::MockI2CDevice dev;
    static constexpr PF::HAL::CRC8Parameters crc_params = {0x31, 0xff, false, false, 0x00};

    PF::HAL::SoftCRC8 crc8c{crc_params};
    PF::Driver::I2C::SensirionDevice sensirion_device{dev, crc8c};

    WHEN("Byte Command data is written into it") {
      auto expected = std::string("\\x36");

      uint8_t command = 0x36;
      auto status = sensirion_device.write(command);

      constexpr size_t buffer_size = 254UL;
      PF::Util::ByteVector<buffer_size> input_buffer;
      uint8_t buffer;
      size_t count = buffer_size;

      // Read buffer written to mock device
      dev.get_write(input_buffer.buffer(), count);
      auto read_buffer = PF::Util::convertByteVectorToHexString(input_buffer, count);

      THEN("status should be ok") {
        REQUIRE(status == PF::I2CDeviceStatus::ok);
        REQUIRE(read_buffer == expected);
      }
    }

    WHEN("Command data is written into it") {
      auto expected = std::string("\\x36\\x08");
      auto status = sensirion_device.write(static_cast<uint16_t>(Command::start_measure_air));

      constexpr size_t buffer_size = 254UL;
      PF::Util::ByteVector<buffer_size> input_buffer;
      uint8_t buffer;
      size_t count = buffer_size;

      // Read buffer written to mock device
      dev.get_write(input_buffer.buffer(), count);
      auto read_buffer = PF::Util::convertByteVectorToHexString(input_buffer, count);

      THEN("status should be ok") {
        REQUIRE(status == PF::I2CDeviceStatus::ok);
        REQUIRE(read_buffer == expected);
      }
    }

    WHEN("Command and argument data is written into it") {
      auto expected = std::string("\\x36\\x08\\x36\\x6A\\x09");
      uint16_t command = 0x366A;
      auto status =
          sensirion_device.write(static_cast<uint16_t>(Command::start_measure_air), command);

      constexpr size_t buffer_size = 254UL;
      PF::Util::ByteVector<buffer_size> input_buffer;
      uint8_t buffer;
      size_t count = buffer_size;

      // Read buffer written to mock device
      dev.get_write(input_buffer.buffer(), count);
      auto read_buffer = PF::Util::convertByteVectorToHexString(input_buffer, count);

      THEN("status should be ok") {
        REQUIRE(status == PF::I2CDeviceStatus::ok);
        REQUIRE(read_buffer == expected);
      }
    }
  }
}