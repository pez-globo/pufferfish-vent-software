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

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("SFM3019: flow sensor behaves properly", "[sensor]") {
    PF::HAL::MockI2CDevice dev;
    PF::HAL::MockI2CDevice gdev;
    PF::Driver::I2C::SFM3019::GasType gas = PF::Driver::I2C::SFM3019::GasType::air;
    bool resetter = false;

    GIVEN("A Mock I2C device") {
        auto body = std::string("\x2e\x04\x02\x06\x11", 5);
        dev.write(body.c_str(), body.size());
        PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};

        WHEN("the device is initialised") {
            const uint32_t ctime = 0x3200;
            PF::HAL::MockTime time;
            time.set_millis(ctime);

            PF::Driver::I2C::SFM3019::Sensor sensor(device, resetter, time);
            auto current_time = time.millis();

            auto status = sensor.setup();

            THEN("the final time should be the same") {
                REQUIRE(current_time == ctime);
                REQUIRE(status == PF::InitializableState::setup);
                // Will fail as there is no return for read_product_id in MOCKI2CDEVICE 
            }
        }
    }

    GIVEN("A Mock I2C device") {
        PF::HAL::MockTime time;
        PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};
        PF::Driver::I2C::SFM3019::Sensor sensor(device, resetter, time);

        WHEN("the device is initialised") {
            const uint32_t ctime = 0x5000;
            time.set_millis(ctime);
            auto current_time = time.millis();

            auto setup_status = sensor.setup();

            float flow = 200;
            auto output_status = sensor.output(flow);
            
            THEN("the final status should ok") {
                REQUIRE(output_status == PF::InitializableState::ok);
                // Will fail as there is no return for read_product_id in MOCKI2CDEVICE 
            }
        }
    }
}