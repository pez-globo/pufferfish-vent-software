/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * HALSPIDevice.cpp
 *
 *  Created on:
 *      Author:
 */

#include "Pufferfish/HAL/STM32/HALSPIDevice.h"

#include "Pufferfish/HAL/CRC.h"
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
