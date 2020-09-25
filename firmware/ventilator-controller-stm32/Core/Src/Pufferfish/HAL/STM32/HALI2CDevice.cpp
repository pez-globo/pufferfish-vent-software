/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * HALI2CDevice.cpp
 *
 *  Created on: Jul 8, 2020
 *      Author: March Boonyapaluk
 */


#include "Pufferfish/HAL/STM32/HALI2CDevice.h"
#include "Pufferfish/HAL/CRC.h"

namespace Pufferfish {
namespace HAL {

I2CDeviceStatus HALI2CDevice::read(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_I2C_Master_Receive(&mDev, mAddr << 1, buf, count,
                                                  HALI2CDevice::DefaultTimeout);
  if (stat == HAL_OK) {
    return I2CDeviceStatus::ok;
  } else {
    return I2CDeviceStatus::readError;
  }
}

I2CDeviceStatus HALI2CDevice::write(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat = HAL_I2C_Master_Transmit(
      &mDev, mAddr << 1, buf, count, HALI2CDevice::DefaultTimeout);
  if (stat == HAL_OK) {
    return I2CDeviceStatus::ok;
  } else {
    return I2CDeviceStatus::writeError;
  }
}

}  // namespace HAL
}  // namespace Pufferfish
