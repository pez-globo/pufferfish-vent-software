/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * HALSPIDevice.cpp
 *
 *  Created on: 
 *      Author: 
 */

#include "stm32h7xx_hal.h"

#include "Pufferfish/HAL/STM32/HALSPIDevice.h"
#include "Pufferfish/HAL/CRC.h"

namespace Pufferfish {
namespace HAL {

 /**
   * Receives an amount of data.
   * @param mDev    SPI handle
   * @param buf     pointer to data buffer
   * @param count   the number of bytes to be read
   * @param Timeout Timeout duration
   * @return status ok on success, error code otherwise
   */
SPIDeviceStatus HALSPIDevice::read(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_SPI_Receive(&mDev, buf, count,
                                                  HALSPIDevice::DefaultTimeout);
  if (stat == HAL_OK) {
    return SPIDeviceStatus::ok;
  } else {
    return SPIDeviceStatus::readError;
  }
}

/**
   * Trnasmits an amount of data.
   * @param mDev    SPI handle
   * @param buf     pointer to data buffer
   * @param count   the number of bytes to be read
   * @param Timeout Timeout duration
   * @return status ok on success, error code otherwise
   */
SPIDeviceStatus HALSPIDevice::write(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_SPI_Transmit(
      &mDev, buf, count, HALSPIDevice::DefaultTimeout);
  if (stat == HAL_OK) {
    return SPIDeviceStatus::ok;
  } else {
    return SPIDeviceStatus::writeError;
  }
}

}  // namespace HAL
}  // namespace Pufferfish
