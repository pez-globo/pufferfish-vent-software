/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/SPIDevice.h"

namespace Pufferfish {
namespace HAL {

/**
 * An SPI slave device
 */
class HALSPIDevice : public SPIDevice {
 public:
  // maximum default time to wait for response from SPI, in ms
  const static uint32_t DefaultTimeout = 2u;

  /**
   * Constructs an HAL SPI object
   * @param hspi    STM32 HAL handler for the SPI port
   */
  HALSPIDevice(SPI_HandleTypeDef &hspi)
      :
      mDev(hspi) {
  }

  SPIDeviceStatus read(uint8_t *buf, size_t count) override;

  
  SPIDeviceStatus write(uint8_t *buf, size_t count) override;

 protected:
  SPI_HandleTypeDef &mDev;
};

}  // namespace HAL
}  // namespace Pufferfish
