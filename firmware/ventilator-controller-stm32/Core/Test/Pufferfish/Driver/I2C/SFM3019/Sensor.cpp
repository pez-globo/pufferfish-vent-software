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
#include "Pufferfish/Util/Endian.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("SFM3019: flow sensor behaves properly", "[sensor]") {
    PF::HAL::MockI2CDevice dev;
    PF::HAL::MockI2CDevice gdev;
    PF::Driver::I2C::SFM3019::GasType gas = PF::Driver::I2C::SFM3019::GasType::air;
    bool resetter = false;

    GIVEN("A Mock I2C device") {
        auto body = std::string("\x04\x02\x60\x06\x11\xa9", 6);

        // write to the MOCKI2Cdevice by set_read
        dev.set_read((const uint8_t*)body.c_str(), body.size());
        PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};

        WHEN("the device is initialised") {
            const uint32_t ctime = 20;
            PF::HAL::MockTime time;
            time.set_micros(ctime);

            PF::Driver::I2C::SFM3019::Sensor sensor(device, resetter, time);

            auto status = sensor.setup();

            THEN("status should be equal to setup") {
                REQUIRE(status == PF::InitializableState::setup);
            }
        }
    }

    GIVEN("A Mock I2C device") {
        PF::HAL::MockTime time;
        PF::Driver::I2C::SFM3019::Device device{dev, gdev, gas};
        PF::Driver::I2C::SFM3019::Sensor sensor(device, resetter, time);

        WHEN("sensor output is calculated") {
            const uint32_t ctime = 0x5000;
            time.set_millis(ctime);
            auto current_time = time.millis();

            auto setup_status = sensor.setup();

            float flow = 200;
            auto output_status = sensor.output(flow);
            
            THEN("the final status should ok") {
                REQUIRE(output_status == PF::InitializableState::ok);
            }
        }
    }
}