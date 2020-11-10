/*
 * BreathingCircuit.tpp
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 */

#include "Pufferfish/Driver/BreathingCircuit/ParametersService.h"

namespace Pufferfish::Driver::BreathingCircuit {

// ParametersService

void ParametersService::transform_fio2(float fio2_request, float &fio2) {
  if (fio2_request >= fio2_min && fio2_request <= fio2_max) {
    fio2 = fio2_request;
  }
}

// PCAC Parameters

void PCACParameters::transform(
    const ParametersRequest &parameters_request, Parameters &parameters) {
  parameters.mode = parameters_request.mode;
  if (!mode_active(parameters)) {
    return;
  }

  parameters.ventilating = parameters_request.ventilating;
  if (parameters_request.rr > 0) {
    parameters.rr = parameters_request.rr;
  }
  if (parameters_request.ie > 0) {
    parameters.ie = parameters_request.ie;
  }
  if (parameters_request.pip > 0) {
    parameters.pip = parameters_request.pip;
  }
  parameters.peep = parameters_request.peep;
  transform_fio2(parameters_request.fio2, parameters.fio2);
}

bool PCACParameters::mode_active(const Parameters &parameters) const {
  return parameters.mode == VentilationMode_pc_ac;
}

// HFNC Parameters

void HFNCParameters::transform(
    const ParametersRequest &parameters_request, Parameters &parameters) {
  parameters.mode = parameters_request.mode;
  if (!mode_active(parameters)) {
    return;
  }

  parameters.ventilating = parameters_request.ventilating;
  if (parameters_request.flow >= 0 && parameters_request.flow <= max_flow) {
    parameters.flow = parameters_request.flow;
  }
  transform_fio2(parameters_request.fio2, parameters.fio2);
}

bool HFNCParameters::mode_active(const Parameters &parameters) const {
  return parameters.mode == VentilationMode_hfnc;
}

// ParametersServices

void ParametersServices::transform(
    const ParametersRequest &parameters_request, Parameters &parameters) {
  switch (parameters_request.mode) {
    case VentilationMode_pc_ac:
      active_service_ = &pc_ac_;
      break;
    case VentilationMode_hfnc:
      active_service_ = &hfnc_;
      break;
    default:
      active_service_ = nullptr;
      break;
  }
  if (active_service_ == nullptr) {
    return;
  }

  active_service_->transform(parameters_request, parameters);
}

}  // namespace Pufferfish::Driver::BreathingCircuit
