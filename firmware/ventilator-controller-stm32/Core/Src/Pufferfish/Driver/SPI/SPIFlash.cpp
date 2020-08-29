/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *
 */

#include "Pufferfish/Driver/SPI/SPIFlash.h"

namespace Pufferfish {
namespace Driver {
namespace SPI {

uint16_t SPIFlash::getDeviceID(void){
  uint8_t txbuf[5] = { deviceIdInstruction, 0x00, 0x00, 0x00, 0x00 };
  uint8_t rxbuf[5] = {0};
  uint8_t count = 5;
  uint16_t deviceid = 0;

  mSpi.writeRead(txbuf, rxbuf, count);

  deviceid = rxbuf[3];
  deviceid = (deviceid << 8) & 0xFF00;
  deviceid |= rxbuf[4];

  return deviceid;
}

SPIDeviceStatus SPIFlash::enableWrite(void){
  uint8_t temp=writeEnableInstruction;
  SPIDeviceStatus stat = mSpi.write(&temp, 1);

  if (stat == SPIDeviceStatus::ok) {
    return SPIDeviceStatus::ok;
  } else {
    return SPIDeviceStatus::writeError;
  }

}

SPIDeviceStatus SPIFlash::disableWrite(void){
  uint8_t temp=writeDisableInstruction;
  SPIDeviceStatus stat = mSpi.write(&temp, 1);

  if (stat == SPIDeviceStatus::ok) {
      return SPIDeviceStatus::ok;
    } else {
      return SPIDeviceStatus::writeError;
    }

}

SPIDeviceStatus SPIFlash::readData(uint32_t addr, uint8_t &rxBuf, uint8_t size){
  uint8_t txbuf[size+4] = {0};

  txbuf[0]= readDataInstruction;
  for (uint8_t index = 1; index<=3 ; index++)
  {
    txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  SPIDeviceStatus stat = mSpi.writeRead(txbuf, &rxBuf, size+4);
  if (stat == SPIDeviceStatus::ok) {
        return SPIDeviceStatus::ok;
      } else {
        return SPIDeviceStatus::readError;
      }
}

bool SPIFlash::eraseChip(void){
  uint8_t temp=writeDisableInstruction;
  SPIDeviceStatus stat = mSpi.write(&temp, 1);
  if (stat == SPIDeviceStatus::ok) {
    return true;
  } else {
    return false;
  }
}

bool SPIFlash::eraseSector4KB(uint32_t addr){
  uint8_t txbuf[4] = {0};
  txbuf[0]= sectorErase4KBInstruction;

  for (uint8_t index = 1; index<=3 ; index++)
  {
    txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  SPIDeviceStatus stat = mSpi.write(txbuf, 4);
  if (stat == SPIDeviceStatus::ok) {
    return true;
  } else {
    return false;
  }
}

bool SPIFlash::eraseBlock32KB(uint32_t addr){
  uint8_t txbuf[4] = {0};
  txbuf[0]= blockErase32KBInstruction;
  for (uint8_t index = 1; index<=3 ; index++){
      txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
    }

  SPIDeviceStatus stat = mSpi.write(txbuf, 4);
  if (stat == SPIDeviceStatus::ok) {
    return true;
  } else {
    return false;
  }

}

bool SPIFlash::eraseBlock64KB(uint32_t addr){
  uint8_t txbuf[4] = {0};
  txbuf[0]= blockErase64KBInstruction;
  for (uint8_t index = 1; index<=3 ; index++){
      txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
    }

  SPIDeviceStatus stat = mSpi.write(txbuf, 4);
  if (stat == SPIDeviceStatus::ok) {
    return true;
  } else {
    return false;
  }

}

SPIDeviceStatus SPIFlash::readByte(uint32_t addr, uint8_t &rxBuf){
  uint8_t txbuf[4] = {0};
  for (uint8_t index = 0; index<=3 ; index++)
    {
      txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
    }

    SPIDeviceStatus stat = mSpi.writeRead(txbuf, &rxBuf, 1);
    if (stat == SPIDeviceStatus::ok) {
          return SPIDeviceStatus::ok;
        } else {
          return SPIDeviceStatus::readError;
        }

}

// FIXME: we can do by using union
void SPIFlash::writeByte(uint32_t addr, uint8_t input){
  static uint8_t  defaultsize = 1;
  uint8_t byteAddr = 0;
  this->enableWrite();
  byteAddr = static_cast<uint8_t>(((addr&0xFF000000) >> 24) & 0xFF);
  mSpi.write(&byteAddr, defaultsize);

  byteAddr = static_cast<uint8_t>(((addr&0x00FF00000) >> 16) & 0xFF);
  mSpi.write(&byteAddr, defaultsize);

  byteAddr = static_cast<uint8_t>(((addr&0x0000FF00) >> 8) & 0xFF);
  mSpi.write(&byteAddr, defaultsize);

  byteAddr = static_cast<uint8_t>((addr&0x000000FF) & 0xFF);
  mSpi.write(&byteAddr, defaultsize);

  mSpi.write(&input, defaultsize);
  this->disableWrite();

}

}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish


