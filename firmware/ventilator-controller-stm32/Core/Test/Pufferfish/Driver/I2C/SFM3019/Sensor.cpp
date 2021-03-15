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
#include "Pufferfish/Driver/I2C/SFM3019/Sensor.h"

#include "Pufferfish/HAL/Mock/MockI2CDevice.h"
#include "Pufferfish/HAL/Mock/MockTime.h"
#include "Pufferfish/Util/Array.h"
#include "Pufferfish/Util/Endian.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("SFM3019: flow sensor behaves properly", "[sensor]") {
  PF::HAL::MockI2CDevice dev;
  PF::HAL::MockI2CDevice gdev;
  PF::Driver::I2C::SFM3019::GasType gas = PF::Driver::I2C::SFM3019::GasType::air;
  bool resetter = false;

  GIVEN("A Mock I2C device") {
    auto body = PF::Util::make_array<uint8_t>(0x04, 0x02, 0x06, 0x11, 0x00);

    // write to the MOCKI2Cdevice by set_read
    dev.set_read(body.data(), body.size());
    PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};

    WHEN("the device is initialised") {
      const uint32_t ctime = 20;
      PF::HAL::MockTime time;
      time.set_micros(ctime);

      PF::Driver::I2C::SFM3019::Sensor sensor(device, resetter, time);

      sensor.setup();

      // THEN("status should be equal to setup") { REQUIRE(status == PF::InitializableState::setup);
      // }
    }
  }

  GIVEN("A Mock I2C device") {
    PF::HAL::MockTime time;
    PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};
    PF::Driver::I2C::SFM3019::Sensor sensor(device, resetter, time);
    PF::Driver::I2C::SFM3019::StateMachine state_machine{};

    WHEN("sensor output is calculated") {
      float flow = 0;
      sensor.output(flow);

      THEN("the final status should ok") {
        // REQUIRE(output_status == PF::InitializableState::ok);
      }
    }
  }
}