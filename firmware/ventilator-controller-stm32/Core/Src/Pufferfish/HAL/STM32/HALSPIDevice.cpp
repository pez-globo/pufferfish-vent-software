/// HALAnalogInput.cpp
/// This file has methods for abstract STM32 HAL class for SPI device.

// Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Pufferfish/HAL/STM32/HALSPIDevice.h"

#include "stm32h7xx_hal.h"

namespace Pufferfish::HAL {

SPIDeviceStatus HALSPIDevice::read(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_SPI_Receive(&dev_, buf, count, HALSPIDevice::default_timeout);
  if (stat == HAL_OK) {
    return SPIDeviceStatus::ok;
  }
  return SPIDeviceStatus::read_error;
}

SPIDeviceStatus HALSPIDevice::write(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_SPI_Transmit(&dev_, buf, count, HALSPIDevice::default_timeout);
  if (stat == HAL_OK) {
    return SPIDeviceStatus::ok;
  }
  return SPIDeviceStatus::write_error;
}

SPIDeviceStatus HALSPIDevice::write_read(uint8_t *tx_buf, uint8_t *rx_buf, size_t count) {
  HAL_StatusTypeDef stat =
      HAL_SPI_TransmitReceive(&dev_, tx_buf, rx_buf, count, HALSPIDevice::default_timeout);
  if (stat == HAL_OK) {
    return SPIDeviceStatus::ok;
  }
  if (stat == HAL_BUSY) {
    return SPIDeviceStatus::busy;
  }
  return SPIDeviceStatus::error;
}

void HALSPIDevice::chip_select(bool input) {
  cs_pin_.write(input);
}

}  // namespace Pufferfish::HAL
