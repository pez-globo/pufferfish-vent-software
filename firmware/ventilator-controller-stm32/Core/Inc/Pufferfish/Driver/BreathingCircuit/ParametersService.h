/*
 * BreathingCircuit.h
 *
 *  Created on: June 6, 2020
 *      Author: Ethan Li
 *
 *  Hardware-backed 32-bit CRC calculation.
 */

#pragma once

#include <cstdint>

#include "Controller.h"
#include "Pufferfish/Application/States.h"

namespace Pufferfish::Driver::BreathingCircuit {

class ParametersService {
 public:
  virtual void transform(const ParametersRequest &parameters_request, Parameters &parameters) = 0;
  [[nodiscard]] virtual bool mode_active(const Parameters &parameters) const = 0;

 protected:
  static void transform_fio2(float fio2_request, float &fio2);
};

class PCACParameters : public ParametersService {
 public:
  void transform(const ParametersRequest &parameters_request, Parameters &parameters) override;
  [[nodiscard]] bool mode_active(const Parameters &parameters) const override;
};

class HFNCParameters : public ParametersService {
 public:
  void transform(const ParametersRequest &parameters_request, Parameters &parameters) override;
  [[nodiscard]] bool mode_active(const Parameters &parameters) const override;

 private:
  static constexpr float max_flow = 80;  // L/min
};

class ParametersServices {
 public:
  void transform(const ParametersRequest &parameters_request, Parameters &parameters);

 private:
  ParametersService *active_service_ = nullptr;
  PCACParameters pc_ac_;
  HFNCParameters hfnc_;
};

}  // namespace Pufferfish::Driver::BreathingCircuit
