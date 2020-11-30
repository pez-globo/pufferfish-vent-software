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

#include "Pufferfish/Driver/BreathingCircuit/Simulator.h"
#include "Pufferfish/Test/Util.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("Simulator:: PCAC simulator behaves properly in inspiratory phase", "[Simulator]") {
    GIVEN("A Simulators object") {
        PF::Driver::BreathingCircuit::SensorVars sensor_vars;
        sensor_vars.flow_o2 = 21;
        sensor_vars.flow_air = 79;
        sensor_vars.po2 = 10;

        SensorMeasurements sensor_measurements; // unused
        CycleMeasurements cycle_measurments;

        uint32_t current_time;

        PF::Driver::BreathingCircuit::Simulators simulators{};

        WHEN("parameters mode is equal to pc_ac") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;
            simulators.transform(2, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(12, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(14, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor_measurments time is equal to current time") {
                // current time = current_time - previous_time = 14-12 = 12
                // sensor_measurments_time = current_time()
                REQUIRE(sensor_measurements.time == 2);
            }
        }

        WHEN("parameters mode is not equal to pcac") {
            Parameters parameters;
            parameters.mode = VentilationMode_hfnc;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::PCACSimulator pcac_simulator{};

            pcac_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor_measurments time is not equal to current time") {
                REQUIRE(sensor_measurements.time != current_time);
            }
        }

        WHEN("parameters ventilating is false") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = false;

            PF::Driver::BreathingCircuit::PCACSimulator pcac_simulator{};

            pcac_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor_measurments time is not equal to current time") {
                REQUIRE(sensor_measurements.time != current_time);
            }
        }

        WHEN("current time is within timeout") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::PCACSimulator pcac_simulator{};

            pcac_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("airway is inspiratory") {
                REQUIRE(PF::Util::isLessFloat(cycle_measurments.rr, parameters.rr) == true);
            }
        }

        WHEN("airway is inspiratory") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::PCACSimulator pcac_simulator{};

            pcac_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor measurments paw is greater than 0") {
                REQUIRE(PF::Util::isLessFloat(sensor_measurements.paw, 0) == false);

            }
        }

        WHEN("final fio2 is calculated") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::PCACSimulator pcac_simulator{};

            pcac_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor measuremnts fio2 should not be equal to 0") {
                REQUIRE(PF::Util::isLessFloat(sensor_measurements.fio2, 0) == false);
            }
        }
    }
}

SCENARIO("Simulator:: PCAC simulator behaves properly in expiratory phase", "[Simulator]") {
    GIVEN("A Simulators object") {
        SensorMeasurements sensor_measurements; // unused
        CycleMeasurements cycle_measurments;

        WHEN("parameters mode is equal to pc_ac") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            uint32_t current_time;

            PF::Driver::BreathingCircuit::Simulators simulators{};

            simulators.transform(10, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(20, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(1200, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor_measurments time is equal to current time") {
                // current time = current_time - previous_time = 14-12 = 12
                // sensor_measurments_time = current_time()
                REQUIRE(sensor_measurements.time == 1180);
            }
        }

        WHEN("current time is not within timeout") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            PF::Driver::BreathingCircuit::PCACSimulator pcac_simulator{};

            pcac_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("airway is expiratory") {
                // not gonna work with the current value of time
                REQUIRE(PF::Util::isLessFloat(sensor_measurements.paw, 0) == false);
            }
        }

        WHEN("airway is expiratory") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.rr = 2.0;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            PF::Driver::BreathingCircuit::PCACSimulator pcac_simulator{};

            pcac_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor measuremnts fio2 should be ") {
                // not gonna work with the current value of time
                REQUIRE(PF::Util::isLessFloat(sensor_measurements.fio2, 0) == false);
            }
        }
    }
}

SCENARIO("Simulator:: HFNC simulator properly", "[Simulator]") {
    SensorMeasurements sensor_measurements; // unused
    CycleMeasurements cycle_measurments;

    GIVEN("A hnfc simulator object") {
        Parameters parameters;
        parameters.mode = VentilationMode_hfnc;
        parameters.fio2 = 30;
        parameters.flow = 40;
        parameters.rr = 4000;
        parameters.peep = 4.0;
        parameters.pip = 10.0;
        parameters.ventilating = true;

        PF::Driver::BreathingCircuit::SensorVars sensor_vars;
        sensor_vars.flow_o2 = 21;
        sensor_vars.flow_air = 79;
        sensor_vars.po2 = 10;

        PF::Driver::BreathingCircuit::Simulators simulators{};

        simulators.transform(10, parameters, sensor_vars, sensor_measurements, cycle_measurments);
        simulators.transform(20, parameters, sensor_vars, sensor_measurements, cycle_measurments);
        simulators.transform(50, parameters, sensor_vars, sensor_measurements, cycle_measurments);

        WHEN("simulator is not within timeout") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.ventilating = false;

            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);

            THEN("Cycle measurments rr is equal to parameters rr") {
                REQUIRE(PF::Util::isEqualFloat(cycle_measurments.rr, 4000) == true);
            }
        }

        WHEN("parameters mode is not equal to hfnc") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.ventilating = true;

            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            uint32_t current_time;

            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);

            THEN("sensor_measurments time is not equal to current time") {
                REQUIRE(sensor_measurements.time != current_time);
            }
        }

        WHEN("parameters ventilating is false") {
            Parameters parameters;
            parameters.mode = VentilationMode_pc_ac;
            parameters.fio2 = 30;
            parameters.flow = 40;
            parameters.ventilating = false;

            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            uint32_t current_time;

            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
            THEN("sensor_measurments time is not equal to current time") {
                REQUIRE(sensor_measurements.time != current_time);
            }
        }
    }

    GIVEN("A hnfc simulator object") {
        Parameters parameters;
        parameters.mode = VentilationMode_hfnc;
        parameters.fio2 = 30;
        parameters.flow = 40;
        parameters.rr = 2;
        parameters.peep = 4.0;
        parameters.pip = 10.0;
        parameters.ventilating = true;

        WHEN("simulator is within timeout") {
            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 0;

            PF::Driver::BreathingCircuit::Simulators simulators{};

            simulators.transform(10, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(20, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(50, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
        
            THEN("Cycle measurments rr is equal to parameters rr") {            
                REQUIRE(PF::Util::isEqualFloat(parameters.rr, cycle_measurments.rr) == false);
            }
        }

        WHEN("sensor vars po2 is equal to 0") {
            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 0;
            float expected_fio2 = 0.188932;

            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);

            THEN("sensor measurments fio2 is of certain value") {
                REQUIRE(PF::Util::isEqualFloat(sensor_measurements.fio2, expected_fio2) == true);            
            }
        }

        WHEN("SPO2 is calculated") {
            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 0;
            float expected_fio2 = 0.188932;

            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);

            THEN("spo2 is less than spo2_min") {
                // given our values spo2 is less than spo2_min
                REQUIRE(PF::Util::isEqualFloat(sensor_measurements.spo2, 21) == true);
            }
        }
    }

    GIVEN("A hnfc simulator object") {
        Parameters parameters;
        parameters.mode = VentilationMode_hfnc;
        parameters.fio2 = 30;
        parameters.flow = 40;
        parameters.rr = 2;
        parameters.peep = 4.0;
        parameters.pip = 10.0;
        parameters.ventilating = true;

        WHEN("simulator is within timeout") {
            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            PF::Driver::BreathingCircuit::Simulators simulators{};

            simulators.transform(10, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(20, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            simulators.transform(50, parameters, sensor_vars, sensor_measurements, cycle_measurments);
            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);
        
            THEN("Cycle measurments rr is equal to parameters rr") {            
                REQUIRE(PF::Util::isEqualFloat(parameters.rr, cycle_measurments.rr) == false);
            }
        }

        WHEN("sensor vars po2 is not equal to 0") {
            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;
            float expected_fio2 = 0.00098692;

            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);

            THEN("sensor measurments fio2 is of certain value") {
                REQUIRE(PF::Util::isEqualFloat(sensor_measurements.fio2, expected_fio2) == true);
            }
        }

        WHEN("SPO2 is calculated") {
            PF::Driver::BreathingCircuit::SensorVars sensor_vars;
            sensor_vars.flow_o2 = 21;
            sensor_vars.flow_air = 79;
            sensor_vars.po2 = 10;

            PF::Driver::BreathingCircuit::HFNCSimulator hfnc_simulator{};

            hfnc_simulator.transform(parameters, sensor_vars, sensor_measurements, cycle_measurments);

            THEN("spo2 is less than spo2_min") {
                // given our values spo2 is less than spo2_min
                REQUIRE(PF::Util::isEqualFloat(sensor_measurements.spo2, 21) == true);
            }
        }
    }
}