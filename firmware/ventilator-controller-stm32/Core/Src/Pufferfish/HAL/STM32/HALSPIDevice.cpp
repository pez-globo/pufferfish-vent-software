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

SPIDeviceStatus HALSPIDevice::read(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_SPI_Receive(&mDev, buf, count,
                                                  HALSPIDevice::DefaultTimeout);
  if (stat == HAL_OK) {
    return SPIDeviceStatus::ok;
  } else {
    return SPIDeviceStatus::readError;
  }
}

SPIDeviceStatus HALSPIDevice::write(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_SPI_Transmit(
      &mDev, buf, count, HALSPIDevice::DefaultTimeout);
  if (stat == HAL_OK) {
    return SPIDeviceStatus::ok;
  } else {
    return SPIDeviceStatus::writeError;
  }
}

SPIDeviceStatus HALSPIDevice::writeRead(uint8_t *txBuf, uint8_t *rxBuf, size_t count) {

  HAL_StatusTypeDef stat = HAL_SPI_TransmitReceive(
      &mDev, txBuf, rxBuf, count, HALSPIDevice::DefaultTimeout);
  if (stat == HAL_OK){
    return SPIDeviceStatus::ok;
  } else if(stat == HAL_BUSY) {
    return SPIDeviceStatus::busy;
  } else{
    return SPIDeviceStatus::error;
  }

}

void HALSPIDevice::chipSelect (bool input)
{

  mCsPin.write(input);

}


}  // namespace HAL
}  // namespace Pufferfish
