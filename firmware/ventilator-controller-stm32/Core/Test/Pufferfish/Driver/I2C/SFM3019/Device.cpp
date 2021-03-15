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
#include "Pufferfish/Driver/I2C/SFM3019/Device.h"

#include "Pufferfish/HAL/Mock/MockI2CDevice.h"
#include "Pufferfish/Test/Util.h"
#include "Pufferfish/Util/Array.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("SFM3019: flow sensor driver behaves properly", "[device]") {
  PF::HAL::MockI2CDevice dev;
  PF::HAL::MockI2CDevice gdev;
  PF::Driver::I2C::SFM3019::GasType gas = PF::Driver::I2C::SFM3019::GasType::air;

  auto buffer = PF::Util::make_array<uint8_t>(0x04, 0x02, 0x60, 0x06, 0x11, 0xa9, 0x12, 0x24, 0x74);
  dev.set_read(buffer.data(), buffer.size());

  GIVEN("A Mock I2C device") {
    PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};

    WHEN("starts a flow measurment") {
      auto status = device.start_measure();
      THEN("should return ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }

    WHEN("stops a flow measurment") {
      auto status = device.stop_measure();

      THEN("status should be ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }

    WHEN("configuring an average window between 0 and 128") {
      uint8_t average_value = 50;
      auto status = device.set_averaging(average_value);
      THEN("should return ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }

    WHEN("configuring an average window of 128") {
      uint8_t average_value = 128;
      auto status = device.set_averaging(average_value);
      THEN("should return ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }

    WHEN("configuring an average window greater than 128") {
      uint8_t average_value = 129;
      auto status = device.set_averaging(average_value);

      THEN("should return ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }
    WHEN("requesting conversion factors") {
      auto status = device.request_conversion_factors();
      THEN("should return ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }

    WHEN("reading product id") {
      uint32_t product_id = 0;
      // auto buffer = PF::Util::make_array<uint8_t>(0x04, 0x02, 0x60, 0x06, 0x11, 0xa9);
      // dev.set_read(buffer.data(), buffer.size());

      auto status = device.read_product_id(product_id);

      THEN("status should be ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
      THEN("The product id is as expected") {
        uint32_t expected = 0x04020611;
        REQUIRE(product_id == expected);
      }
    }

    WHEN("requesting conversion factors") {
      auto status = device.request_conversion_factors();
      THEN("status should be ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }

    WHEN("reading conversion factors after 20 us") {
      // auto buffer = PF::Util::make_array<uint8_t>(0x01, 0x02, 0x17, 0x04, 0x05, 0xF7);
      // dev.set_read(buffer.data(), buffer.size());

      PF::Driver::I2C::SFM3019::ConversionFactors conversion_factors{};

      auto status = device.read_conversion_factors(conversion_factors);

      THEN("status should be ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }

    WHEN("A sample is given") {
      PF::Driver::I2C::SFM3019::Sample sample{};
      PF::Driver::I2C::SFM3019::ConversionFactors conversion_factors;
      sample.raw_flow = 10;
      sample.flow = 2.5;
      conversion_factors.scale_factor = 10;
      conversion_factors.offset = 5;

      auto status =
          device.read_sample(sample, conversion_factors.scale_factor, conversion_factors.offset);

      THEN("status should be ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }
  }

  GIVEN("A Mock I2C device") {
    PF::Driver::I2C::SFM3019::Device device{dev, dev, gas};

    WHEN("reset command is sent") {
      auto status = device.reset();

      THEN("status should be ok") { REQUIRE(status == PF::I2CDeviceStatus::ok); }
    }
  }
}