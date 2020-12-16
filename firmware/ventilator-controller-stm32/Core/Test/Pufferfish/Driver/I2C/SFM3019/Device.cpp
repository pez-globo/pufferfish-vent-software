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

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("SFM3019: flow sensor driver behaves properly", "[device]") {
    PF::HAL::MockI2CDevice dev;
    PF::HAL::MockI2CDevice gdev;
    PF::Driver::I2C::SFM3019::GasType gas = PF::Driver::I2C::SFM3019::GasType::air;

    auto buffer = std::string("\x04\x02\x60\x06\x11\xa9\x12\x24\x74", 9);
    dev.set_read((const uint8_t*)buffer.c_str(), buffer.size());

    GIVEN("A Mock I2C device") {
        PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};

        WHEN("starts a flow measurment") {
            auto status = device.start_measure();
            THEN("should return ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }

        WHEN("reading product id") {
            uint32_t product_id = 0x04020611;
            auto buffer = std::string("\x04\x02\x60\x06\x11\xa9", 6);
            dev.set_read((const uint8_t*)buffer.c_str(), buffer.size());

            auto status = device.read_product_id(product_id);

            THEN("status should be ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }

        WHEN("configuring an average window") {
            uint8_t average_value = 50;
            auto status = device.set_averaging(average_value);
            THEN("should return ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }

        WHEN("requesting conversion factors") {
            auto status = device.request_conversion_factors();
            THEN("status should be ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }

        WHEN("reading conversion factors after 20 us") {
            PF::Driver::I2C::SFM3019::ConversionFactors conversion_factors{};
            conversion_factors.scale_factor = 10;
            conversion_factors.offset = 5;
            conversion_factors.flow_unit = 10;

            auto status = device.read_conversion_factors(conversion_factors);

            THEN("status should be ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }

        WHEN("A sample is given") {
            PF::Driver::I2C::SFM3019::Sample sample;
            PF::Driver::I2C::SFM3019::ConversionFactors conversion_factors;
            sample.raw_flow = 10;
            sample.flow = 2.5;
            conversion_factors.scale_factor = 10;
            conversion_factors.offset = 5;

            auto status = device.read_sample(sample, conversion_factors.scale_factor, conversion_factors.offset);

            THEN("status should be ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }

        WHEN("stops a flow measurment") {
            auto status = device.stop_measure();

            THEN("status should be ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }
    }

    GIVEN("A Mock I2C device") {
        PF::Driver::I2C::SFM3019::Device device{dev, dev, gas};

        WHEN("reset command is sent") {
            auto status = device.reset();

            THEN("status should be ok") {
                REQUIRE(status == PF::I2CDeviceStatus::ok);
            }
        }
    }

}