/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 */

#pragma once

#include "stm32h7xx_hal.h"
#include "Pufferfish/HAL/Interfaces/I2CDevice.h"

namespace Pufferfish {
namespace HAL {

/**
 * An I2C slave device
 */
class HALI2CDevice : public I2CDevice {
 public:
  // maximum default time to wait for response from I2C, in ms
  const static uint32_t DefaultTimeout = 500u;

  /**
   * Constructs an HAL I2C object
   * @param hi2c    STM32 HAL handler for the I2C port
   * @param address the I2C address of the device
   */
  HALI2CDevice(I2C_HandleTypeDef &hi2c, uint16_t address)
      :
      mDev(hi2c),
      mAddr(address) {
  }

  I2CDeviceStatus read(uint8_t *buf, size_t count) override;

  /**
   * Note: there *could* be a race condition that modifies buf
   *    when this is being used with DMA and IT HAL_I2C
   */
  I2CDeviceStatus write(uint8_t *buf, size_t count) override;

 protected:
  I2C_HandleTypeDef &mDev;
  const uint16_t mAddr;
};

}  // namespace HAL
}  // namespace Pufferfish
