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

#include "Pufferfish/Driver/BreathingCircuit/ParametersService.h"

#include "Pufferfish/Test/Util.h"
#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("BreathingCircuit:: ParameterService behaves properly", "[ParameterService]") {
  GIVEN("A PCACParameterservice object") {
    PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

    WHEN("parameters ventilation mode is not equal to pc_ac") {
      Parameters parameters;
      parameters.mode = VentilationMode_hfnc;
      parameters.ventilating = true;

      ParametersRequest parametersRequest;

      pcacparameters.transform(parametersRequest, parameters);
      THEN("parameters ventilating should not be equal to parameters request ventilatings") {
        REQUIRE(parametersRequest.ventilating != parameters.ventilating);
      }
    }

    WHEN("parameters ventilation mode is equal to pc_ac") {
      Parameters parameters;
      parameters.mode = VentilationMode_pc_ac;
      parameters.fio2 = 22;
      parameters.ventilating = true;

      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;

      pcacparameters.transform(parametersRequest, parameters);
      THEN("parameters ventilating should not be equal to parameters request ventilatings") {
        REQUIRE(parameters.ventilating == parametersRequest.ventilating);
      }
    }
  }

  GIVEN("A PCACParameterservice object") {
    Parameters parameters;
    parameters.mode = VentilationMode_pc_ac;
    parameters.fio2 = 21;
    parameters.flow = 40;
    parameters.ventilating = true;

    WHEN("parameters request rr is greater than 0") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.rr = 2;

      PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

      pcacparameters.transform(parametersRequest, parameters);
      THEN("they should be same") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.rr, parameters.rr) == true);
      }
    }

    WHEN("parameters request rr is less than 0") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.rr = -2;

      PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

      pcacparameters.transform(parametersRequest, parameters);
      THEN("they should be same") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.rr, parameters.rr) == false);
      }
    }

    WHEN("parameters request ie is greater than 0") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.ie = 2;

      PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

      pcacparameters.transform(parametersRequest, parameters);
      THEN("they should be same") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.ie, parameters.ie) == true);
      }
    }

    WHEN("parameters request ie is less than 0") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.ie = -2;

      PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

      pcacparameters.transform(parametersRequest, parameters);
      THEN("they should be same") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.ie, parameters.ie) == false);
      }
    }

    WHEN("parameters request pip is less than 0") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.pip = -2;

      PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

      pcacparameters.transform(parametersRequest, parameters);
      THEN("they should be same") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.pip, parameters.pip) == false);
      }
    }

    WHEN("parameters request pip is greater than 0") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.pip = 2;

      PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

      pcacparameters.transform(parametersRequest, parameters);
      THEN("they should be same") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.pip, parameters.pip) == true);
      }
    }
  }

  GIVEN("A PCACParameterservice object") {
    Parameters parameters;
    parameters.mode = VentilationMode_pc_ac;
    parameters.flow = 40;
    parameters.peep = 30;
    parameters.ventilating = true;

    PF::Driver::BreathingCircuit::PCACParameters pcacparameters{};

    WHEN("parameters request fio2 is greater than fio2_min and less than fio2_max") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.flow = 40;
      parametersRequest.rr = 2;
      parametersRequest.pip = 4;
      parametersRequest.ie = 6;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 30;

      pcacparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == true);
      }
    }

    WHEN("parameters request fio2 is less than fio2_min and less than fio2_max") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.flow = 40;
      parametersRequest.rr = 2;
      parametersRequest.pip = 4;
      parametersRequest.ie = 6;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 15;

      pcacparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == false);
      }
    }

    WHEN("parameters request fio2 is less than fio2_max and greater than fio2_min") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.flow = 40;
      parametersRequest.rr = 2;
      parametersRequest.pip = 4;
      parametersRequest.ie = 6;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 80;

      pcacparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == true);
      }
    }

    WHEN("parameters request fio2 is greater than fio2_max and greater than fio2_min") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_pc_ac;
      parametersRequest.flow = 40;
      parametersRequest.rr = 2;
      parametersRequest.pip = 4;
      parametersRequest.ie = 6;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 102;

      pcacparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == false);
      }
    }
  }

  GIVEN("A HFNParameterservice object") {
    PF::Driver::BreathingCircuit::HFNCParameters hfncparameters{};

    WHEN("parameters mode is not equal to hfnc") {
      Parameters parameters;
      parameters.mode = VentilationMode_pc_ac;
      parameters.ventilating = true;

      ParametersRequest parametersRequest;
      parametersRequest.flow = 40;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 30;

      hfncparameters.transform(parametersRequest, parameters);
      THEN("parameters request ventilating and parameters ventilating value should be different") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == false);
      }
    }
  }

  GIVEN("A HFNParameterservice object") {
    PF::Driver::BreathingCircuit::HFNCParameters hfncparameters{};

    WHEN("parameters request flow is greater than 0 and less than flow_max") {
      Parameters parameters;
      parameters.mode = VentilationMode_hfnc;
      parameters.ventilating = true;

      ParametersRequest parametersRequest;
      parametersRequest.flow = 20;
      parametersRequest.ventilating = true;
      parametersRequest.mode = VentilationMode_hfnc;

      hfncparameters.transform(parametersRequest, parameters);

      THEN("parameters request flow and parameters flow should be same") {
        REQUIRE(PF::Util::isEqualFloat(parameters.flow, parametersRequest.flow) == true);
      }
    }

    WHEN("parameters request flow is less than 0 and less than flow_max") {
      Parameters parameters;
      parameters.mode = VentilationMode_hfnc;
      parameters.fio2 = 21;
      parameters.ventilating = true;

      ParametersRequest parametersRequest;
      parametersRequest.flow = -2;
      parametersRequest.ventilating = true;
      parametersRequest.mode = VentilationMode_hfnc;

      hfncparameters.transform(parametersRequest, parameters);

      THEN("parameters request flow and parameters flow should be same") {
        REQUIRE(PF::Util::isEqualFloat(parameters.flow, parametersRequest.flow) == false);
      }
    }

    WHEN("parameters request flow is greater than 0 and greater than flow_max") {
      Parameters parameters;
      parameters.mode = VentilationMode_hfnc;
      parameters.fio2 = 21;
      parameters.ventilating = true;

      ParametersRequest parametersRequest;
      parametersRequest.flow = 82;
      parametersRequest.ventilating = true;
      parametersRequest.mode = VentilationMode_hfnc;

      hfncparameters.transform(parametersRequest, parameters);

      THEN("parameters request flow and parameters flow should be same") {
        REQUIRE(PF::Util::isEqualFloat(parameters.flow, parametersRequest.flow) == false);
      }
    }
  }

  GIVEN("A HFNParameterservice object") {
    Parameters parameters;
    parameters.mode = VentilationMode_hfnc;
    parameters.ventilating = true;

    PF::Driver::BreathingCircuit::HFNCParameters hfncparameters{};

    WHEN("parameters request fio2 is greater than fio2_min and less than fio2_max") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_hfnc;
      parametersRequest.flow = 40;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 30;

      hfncparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == true);
      }
    }

    WHEN("parameters request fio2 is less than fio2_min and less than fio2_max") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_hfnc;
      parametersRequest.flow = 40;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 15;

      hfncparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == false);
      }
    }

    WHEN("parameters request fio2 is less than fio2_max and greater than fio2_min") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_hfnc;
      parametersRequest.flow = 40;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 80;

      hfncparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == true);
      }
    }

    WHEN("parameters request fio2 is greater than fio2_max and greater than fio2_min") {
      ParametersRequest parametersRequest;
      parametersRequest.mode = VentilationMode_hfnc;
      parametersRequest.flow = 40;
      parametersRequest.ventilating = true;
      parametersRequest.fio2 = 102;

      hfncparameters.transform(parametersRequest, parameters);
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == false);
      }
    }
  }

  GIVEN("A Parameterservice object") {
    Parameters parameters;
    parameters.flow = 40;
    parameters.ventilating = true;

    ParametersRequest parametersRequest;
    parametersRequest.mode = VentilationMode_pc_ac;
    parametersRequest.flow = 40;
    parametersRequest.rr = 2;
    parametersRequest.pip = 4;
    parametersRequest.ie = 6;
    parametersRequest.ventilating = true;
    parametersRequest.fio2 = 80;

    PF::Driver::BreathingCircuit::ParametersServices parameterservices{};

    parameterservices.transform(parametersRequest, parameters);

    WHEN("parameters request mode is equal to pc_ac") {
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == true);
      }
    }
  }

  GIVEN("A Parameterservice object") {
    Parameters parameters;
    parameters.flow = 42;
    parameters.ventilating = true;

    ParametersRequest parametersRequest;
    parametersRequest.mode = VentilationMode_hfnc;
    parametersRequest.flow = 40;
    parametersRequest.ventilating = true;
    parametersRequest.fio2 = 80;

    PF::Driver::BreathingCircuit::ParametersServices parameterservices{};

    parameterservices.transform(parametersRequest, parameters);

    WHEN("parameters request mode is not equal to hfnc") {
      THEN("parameters fio2 is equal to parameters request fio2") {
        REQUIRE(PF::Util::isEqualFloat(parametersRequest.fio2, parameters.fio2) == true);
      }
    }
  }
}