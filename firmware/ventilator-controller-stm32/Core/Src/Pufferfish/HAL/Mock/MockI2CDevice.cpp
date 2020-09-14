/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Hemanth Gowda S
 */

#include "Pufferfish/HAL/Mock/MockI2CDevice.h"

namespace Pufferfish {
namespace HAL {



I2CDeviceStatus MockI2CDevice::read(uint8_t *buf, size_t count) {
  size_t index;
  size_t minumum = (count < mReadBufSize)? count : mReadBufSize;

  for (index = 0; index < minumum; index++) {
    buf[index] = mReadBuf[index];
  }
  return I2CDeviceStatus::ok;
}

void MockI2CDevice::setRead(uint8_t *buf, size_t count) {
  size_t index;
  size_t minumum = (count < mReadBufSize)? count : mReadBufSize;

  for (index = 0; index < minumum; index++)
  {
    mReadBuf[index] = buf[index];
  }

}

I2CDeviceStatus MockI2CDevice::write(uint8_t *buf, size_t count) {
  size_t index;

  mWriteCount = (count < mWriteBufSize)? count : mWriteBufSize;
  for (index = 0; index < mWriteCount; index++) {
    mWriteBuf[index] = buf[index];
  }

  return I2CDeviceStatus::ok;
}

void MockI2CDevice::getWrite(uint8_t *buf, size_t &count) {
  size_t index;

  count = mWriteCount;
  for (index = 0; index < count; index++)
  {
    buf[index] = mWriteBuf[index];
  }
}

}  // namespace HAL
}  // namespace Pufferfish
