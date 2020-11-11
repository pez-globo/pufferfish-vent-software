/*
 * CRC.h
 *
 *  Created on: May 19, 2020
 *      Author: Ethan Li
 *
 *  Hardware-backed CRC calculation.
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/CRCChecker.h"
#include "stm32h7xx_hal.h"

namespace Pufferfish::HAL {

class HALCRC32 : public CRC32 {
 public:
  explicit HALCRC32(CRC_HandleTypeDef &hcrc) : hcrc_(hcrc) {}

  uint32_t compute(const uint8_t *data, size_t size) override;

 private:
  CRC_HandleTypeDef &hcrc_;
};

}  // namespace Pufferfish::HAL
