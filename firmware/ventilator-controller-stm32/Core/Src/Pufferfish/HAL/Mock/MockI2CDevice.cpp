/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "stm32h7xx_hal.h"

#include "Pufferfish/HAL/Mock/MockI2CDevice.h"

namespace Pufferfish {
namespace HAL {


I2CDeviceStatus MockI2CDevice::read(uint8_t *buf, size_t count) {
  size_t index;
  for (index = 0; index < count; index++)
  {
    buf[index] = mReadBuf[index];
  }
  return I2CDeviceStatus::ok;
}

void MockI2CDevice::setRead(uint8_t *buf, size_t count) {
  size_t index;

  for (index = 0; index < count; index++)
  {
    mReadBuf[index] = buf[index];
  }

}

I2CDeviceStatus MockI2CDevice::write(uint8_t *buf, size_t count) {
  size_t index;

  for (index = 0; index < count; index++)
  {
    mWriteBuf[index] = buf[index];
  }
  return I2CDeviceStatus::ok;
}

void MockI2CDevice::getWrite(uint8_t *buf, size_t count)
{
  size_t index;

  for (index = 0; index < count; index++)
  {
    buf[index] = mWriteBuf[index];
  }
}

}  // namespace HAL
}  // namespace Pufferfish
