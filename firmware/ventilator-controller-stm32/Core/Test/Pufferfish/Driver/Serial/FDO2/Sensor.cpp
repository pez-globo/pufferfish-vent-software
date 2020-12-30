/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Device.cpp
 *
 *  Created on: Nov 25, 2020
 *      Author: Rohan Purohit
 *
 * Unit tests to confirm behavior of flow sensor driver
 *
 */
#include "Pufferfish/Driver/Serial/FDO2/Sensor.h"

#include "Pufferfish/HAL/Mock/MockBufferedUART.h"
#include "Pufferfish/HAL/Mock/MockTime.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("FDO2: flow sensor behaves properly", "[sensor]") {
  GIVEN("A Mock I2C device") {
    constexpr size_t rx_buffer_size = 252UL;
    constexpr size_t tx_buffer_size = 252UL;

    PF::HAL::MockBufferedUART<rx_buffer_size, tx_buffer_size> uart{};
    auto body = std::string("\x04\x02\x00\x06\x11\x00", 6);
    uart.write((const uint8_t*)body.c_str());

    PF::Driver::Serial::FDO2::Device device{uart};

    WHEN("the device is initialised") {
      const uint32_t ctime = 0x3200;
      PF::HAL::MockTime time;
      time.set_millis(ctime);

      auto current_time = time.millis();

      PF::Driver::Serial::FDO2::Sensor sensor(device, time);

      auto status = sensor.setup();

      THEN("the status should be equal to setup") {
        REQUIRE(current_time == ctime);
        REQUIRE(status == PF::InitializableState::setup);
      }
    }
  }

  GIVEN("A Mock I2C device") {
    constexpr size_t rx_buffer_size = 252UL;
    constexpr size_t tx_buffer_size = 252UL;

    PF::HAL::MockBufferedUART<rx_buffer_size, tx_buffer_size> uart{};
    auto body = std::string("\x04\x02\x06\x11");
    uart.write((const uint8_t*)body.c_str());
    PF::HAL::MockTime time;
    PF::Driver::Serial::FDO2::Device device{uart};

    WHEN("output from the sensor is calculated") {
      const uint32_t ctime = 0x5000;
      time.set_millis(ctime);

      PF::Driver::Serial::FDO2::Sensor sensor(device, time);

      auto setup_status = sensor.setup();

      uint32_t po2 = 20;
      auto output_status = sensor.output(po2);

      THEN("the final status should ok") {
        REQUIRE(output_status == PF::InitializableState::ok);
        // Will fail as there is no return for check version in Mock Uart device
      }
    }
  }
}