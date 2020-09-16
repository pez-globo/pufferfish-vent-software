/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * HALSPIDevice.cpp
 *
 *  Created on: 
 *      Author: 
 */

#include "Pufferfish/HAL/Mock/MockSPIDevice.h"

namespace Pufferfish {
namespace HAL {


SPIDeviceStatus MockSPIDevice::read(uint8_t *buf, size_t count) {
  size_t index;
  size_t minumum = (count < mReadBufSize)? count : mReadBufSize;

  for (index = 0; index < minumum; index++) {
    buf[index] = mReadBuf[index];
  }
  return SPIDeviceStatus::ok;
}

void MockSPIDevice::setRead(uint8_t *buf, size_t count) {
  size_t index;
  size_t minumum = (count < mReadBufSize)? count : mReadBufSize;

  for (index = 0; index < minumum; index++)
  {
    mReadBuf[index] = buf[index];
  }
}

SPIDeviceStatus MockSPIDevice::write(uint8_t *buf, size_t count) {
  size_t index;

  mWriteCount = (count < mWriteBufSize)? count : mWriteBufSize;
  for (index = 0; index < mWriteCount; index++) {
    mWriteBuf[index] = buf[index];
  }

  return SPIDeviceStatus::ok;
}

void MockSPIDevice::getWrite(uint8_t *buf, size_t &count) {
  size_t index;

  count = mWriteCount;
  for (index = 0; index < count; index++)
  {
    buf[index] = mWriteBuf[index];
  }
}

void MockSPIDevice::setWriteRead(uint8_t *buf, size_t count) {
  size_t index;
  size_t minumum = (count < mReadBufSize)? count : mReadBufSize;

  for (index = 0; index < minumum; index++)
  {
    mSetWriteReadBuf[index] = buf[index];
  }
}

SPIDeviceStatus MockSPIDevice::writeRead(uint8_t *txBuf, uint8_t *rxBuf, size_t count) {
  size_t index;
  size_t minumum = (count < mReadBufSize)? count : mReadBufSize;

  for (index = 0; index < minumum; index++) {
    rxBuf[index] = mSetWriteReadBuf[index];
  }

  mGetWriteReadCount = (count < mWriteBufSize)? count : mWriteBufSize;
  for (index = 0; index < mWriteCount; index++) {
    mGetWriteReadBuf[index] = txBuf[index];
  }

  return SPIDeviceStatus::ok;
}

void MockSPIDevice::getWriteRead(uint8_t *buf, size_t count) {
  size_t index;

  count = mGetWriteReadCount;
  for (index = 0; index < count; index++)
  {
    buf[index] = mGetWriteReadBuf[index];
  }
}

void MockSPIDevice::chipSelect (bool input) {
  mLastCS = input;
}

bool MockSPIDevice::getChipSelect (){
  return mLastCS;
}


}  // namespace HAL
}  // namespace Pufferfish
