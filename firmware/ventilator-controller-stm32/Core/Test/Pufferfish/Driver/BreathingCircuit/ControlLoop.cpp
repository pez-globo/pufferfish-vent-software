/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Algorithms.cpp
 *
 *  Created on: Nov 11, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of BreathingCircuit Controller
 *
 */

#include "Pufferfish/Driver/BreathingCircuit/ControlLoop.h"

#include "Pufferfish/HAL/Mock/MockI2CDevice.h"
#include "Pufferfish/HAL/Mock/MockPWM.h"
#include "Pufferfish/HAL/Mock/MockTime.h"
#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("BreathingCircuit::Controlloop behaves correctly", "[ControlLoop]") {
  GIVEN("A Controller object") {
    bool resetter = false;
    PF::HAL::MockTime time;
    PF::HAL::MockI2CDevice dev;
    auto body = std::string("\x2e\x04\x02\x06\x11", 5);

    // wrtie to the MOCKI2Cdevice by set_read
    dev.set_read(reinterpret_cast<const uint8_t*>(body.c_str()), body.size());

    PF::HAL::MockI2CDevice gdev;
    PF::Driver::I2C::SFM3019::GasType gas_air = PF::Driver::I2C::SFM3019::GasType::air;
    PF::Driver::I2C::SFM3019::GasType gas_o2 = PF::Driver::I2C::SFM3019::GasType::o2;
    PF::Driver::I2C::SFM3019::Device device_air{dev, gdev, gas_air};
    PF::Driver::I2C::SFM3019::Device device_o2{dev, gdev, gas_o2};

    PF::Driver::I2C::SFM3019::Sensor sensor_air(device_air, resetter, time);
    PF::Driver::I2C::SFM3019::Sensor sensor_o2(device_o2, resetter, time);
    PF::Driver::BreathingCircuit::ActuatorSetpoints actuator_setpoints{};

    SensorMeasurements sensor_measurements;  // unused
    PF::HAL::MockPWM pwm_air{};
    uint32_t duty_air = 0.5;
    pwm_air.set_duty_cycle_raw(duty_air);
    PF::HAL::MockPWM pwm_o2{};
    uint32_t duty_02 = 0.5;
    pwm_o2.set_duty_cycle_raw(duty_02);

    WHEN("parameters mode is not equal to hfnc") {
      Parameters parameters;
      parameters.mode = VentilationMode_pc_ac;
      parameters.fio2 = 21;
      parameters.flow = 40;
      parameters.ventilating = true;

      PF::Driver::BreathingCircuit::HFNCControlLoop hfnc_control_loop{
          parameters, sensor_measurements, sensor_air, sensor_o2, pwm_air, pwm_o2};

      uint32_t current_time = 20;
      hfnc_control_loop.update(current_time);

      THEN("sensor measurments flow is zero") {
        REQUIRE(sensor_measurements.flow == Approx(0));
      }
    }

    WHEN("time is within timeout") {
      Parameters parameters;
      parameters.mode = VentilationMode_hfnc;
      parameters.fio2 = 21;
      parameters.flow = 40;
      parameters.ventilating = true;

      PF::Driver::BreathingCircuit::HFNCControlLoop hfnc_control_loop{
          parameters, sensor_measurements, sensor_air, sensor_o2, pwm_air, pwm_o2};

      uint32_t current_time = 0.5;
      hfnc_control_loop.update(current_time);

      THEN("final values should be same") {
        REQUIRE(sensor_measurements.flow == Approx(0));
      }
    }

    WHEN("time is not within timeout") {
      Parameters parameters;
      parameters.mode = VentilationMode_hfnc;
      parameters.fio2 = 30;
      parameters.flow = 40;
      parameters.ventilating = true;

      PF::Driver::BreathingCircuit::HFNCControlLoop control_loop{
          parameters, sensor_measurements, sensor_air, sensor_o2, pwm_air, pwm_o2};

      uint32_t current_time = 24;
      control_loop.update(current_time);

      THEN("the actuator flow values should be non-zero") {
      }
    }
  }
}
