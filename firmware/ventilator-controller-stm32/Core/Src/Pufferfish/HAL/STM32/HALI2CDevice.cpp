/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * HALI2CDevice.cpp
 *
 *  Created on: Jul 8, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/HALI2CDevice.h"

#include "stm32h7xx_hal.h"

namespace Pufferfish::HAL {

I2CDeviceStatus HALI2CDevice::read(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat =
      HAL_I2C_Master_Receive(&dev_, addr << 1U, buf, count, HALI2CDevice::default_timeout);
  if (stat == HAL_OK) {
    return I2CDeviceStatus::ok;
  }
  return I2CDeviceStatus::read_error;
}

I2CDeviceStatus HALI2CDevice::write(uint8_t *buf, size_t count) {
  HAL_StatusTypeDef stat =
      HAL_I2C_Master_Transmit(&dev_, addr << 1U, buf, count, HALI2CDevice::default_timeout);
  if (stat == HAL_OK) {
    return I2CDeviceStatus::ok;
  }
  return I2CDeviceStatus::write_error;
}

}  // namespace Pufferfish::HAL
