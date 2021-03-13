/*
 * Copyright 2020, the Pez Globo team and the PF project contributors
 *
 * Algorithms.cpp
 *
 *  Created on: Nov 11, 2020
 *      Author: Renji Panicker
 *
 * Unit tests to confirm behavior of BreathingCircuit Controller
 *
 */

#include "Pufferfish/Driver/BreathingCircuit/Controller.h"

#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("BreathingCircuit::Controller behaves correctly", "[Controllers]") {
  GIVEN("A Controller object") {
    uint32_t time = 0;
    Parameters parameters;
    parameters.mode = VentilationMode_pc_ac;
    parameters.fio2 = 30;
    parameters.flow = 40;

    PF::Driver::BreathingCircuit::SensorVars sensor_vars;
    sensor_vars.flow_o2 = 21;
    sensor_vars.flow_air = 79;
    sensor_vars.po2 = 10;

    SensorMeasurements sensor_measurements;  // unused
    PF::Driver::BreathingCircuit::ActuatorSetpoints actuator_setpoints;
    PF::Driver::BreathingCircuit::ActuatorVars actuator_vars;

    PF::Driver::BreathingCircuit::HFNCController controller;
    controller.transform(
        time, parameters, sensor_vars, sensor_measurements, actuator_setpoints, actuator_vars);

    WHEN("the device ventilating mode is not hfnc") {
      THEN("the final value should be as expected") {
        REQUIRE(actuator_setpoints.flow_air == Approx(0));
      }
    }
  }

  GIVEN("A Controller object") {
    uint32_t time = 0;
    Parameters parameters;
    parameters.mode = VentilationMode_hfnc;
    parameters.fio2 = 30;
    parameters.flow = 40;
    parameters.ventilating = false;

    PF::Driver::BreathingCircuit::SensorVars sensor_vars;
    sensor_vars.flow_o2 = 21;
    sensor_vars.flow_air = 79;
    sensor_vars.po2 = 10;

    SensorMeasurements sensor_measurements;  // unused
    PF::Driver::BreathingCircuit::ActuatorSetpoints actuator_setpoints;
    PF::Driver::BreathingCircuit::ActuatorVars actuator_vars;

    PF::Driver::BreathingCircuit::HFNCController controller;
    controller.transform(
        time, parameters, sensor_vars, sensor_measurements, actuator_setpoints, actuator_vars);

    WHEN("the device is not ventilating") {
      THEN("the final value should be as expected") {
        REQUIRE(actuator_setpoints.flow_o2 == Approx(0));
        REQUIRE(actuator_setpoints.flow_air == Approx(0));
      }
    }
  }

  GIVEN("A Controller object") {
    uint32_t time = 0;
    Parameters parameters;
    parameters.mode = VentilationMode_hfnc;
    parameters.fio2 = 30;
    parameters.flow = 40;
    parameters.ventilating = true;

    PF::Driver::BreathingCircuit::SensorVars sensor_vars;
    sensor_vars.flow_o2 = 21;
    sensor_vars.flow_air = 79;
    sensor_vars.po2 = 10;

    SensorMeasurements sensor_measurements;  // unused
    PF::Driver::BreathingCircuit::ActuatorSetpoints actuator_setpoints;
    PF::Driver::BreathingCircuit::ActuatorVars actuator_vars;

    PF::Driver::BreathingCircuit::HFNCController controller;
    controller.transform(
        time, parameters, sensor_vars, sensor_measurements, actuator_setpoints, actuator_vars);

    WHEN("the device is ventilating") {
      THEN("the final value should be as expected") {
      }
    }
  }
}
