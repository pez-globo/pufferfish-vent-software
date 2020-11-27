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

#include "Pufferfish/Driver/BreathingCircuit/Controller.h"
#include "Pufferfish/Test/Util.h"

#include "catch2/catch.hpp"
#include <iostream>

namespace PF = Pufferfish;

SCENARIO("BreathingCircuit::Controller behaves correctly", "[Controllers]") {
  GIVEN("A Controller object") {
    uint32_t time = 0;
    Parameters parameters;
    parameters.mode = VentilationMode_hfnc;
    parameters.fio2 = 21;
    parameters.flow = 40;

    Pufferfish::Driver::BreathingCircuit::SensorVars sensor_vars;
    sensor_vars.flow_o2 = 21;
    sensor_vars.flow_air = 79;
    sensor_vars.po2 = 10;

    SensorMeasurements sensor_measurements; // unused
    Pufferfish::Driver::BreathingCircuit::ActuatorSetpoints actuator_setpoints;
    Pufferfish::Driver::BreathingCircuit::ActuatorVars actuator_vars;

    Pufferfish::Driver::BreathingCircuit::HFNCController controller;
    controller.transform(time, parameters, sensor_vars, sensor_measurements, actuator_setpoints, actuator_vars);

    WHEN("the current time is written to it") {
      THEN("the final time should be the same") {
       REQUIRE(PF::Util::isEqualFloat(actuator_setpoints.flow_o2, 0.0) == true);
      }
    }
  }
}
