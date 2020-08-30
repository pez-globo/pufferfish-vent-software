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

SPIDeviceStatus SPIFlash::readData(uint32_t addr, uint8_t *rxBuf, uint8_t size){
  uint8_t txbuf[size+4] = {0};

  txbuf[0]= readDataInstruction;
  for (uint8_t index = 1; index<=3 ; index++){
    txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  SPIDeviceStatus stat = mSpi.writeRead(txbuf, rxBuf, size+4);
  if (stat == SPIDeviceStatus::ok) {
    return SPIDeviceStatus::ok;
  } else {
    return SPIDeviceStatus::readError;
  }

}

bool SPIFlash::eraseChip(void){
  uint8_t temp=writeDisableInstruction;

  this->enableWrite();
  SPIDeviceStatus stat = mSpi.write(&temp, 1);
  this->disableWrite();
  if (stat == SPIDeviceStatus::ok) {
    return true;
  } else {
    return false;
  }

}

bool SPIFlash::eraseSector4KB(uint32_t addr){
  uint8_t txbuf[4] = {0};
  txbuf[0]= sectorErase4KBInstruction;

  this->enableWrite();
  for (uint8_t index = 1; index<=3 ; index++){
    txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  SPIDeviceStatus stat = mSpi.write(txbuf, 4);
  this->disableWrite();
  if (stat == SPIDeviceStatus::ok) {
    return true;
  } else {
    return false;
  }

}

SPIDeviceStatus SPIFlash::eraseBlock32KB(uint32_t addr){
  uint8_t txbuf[4] = {0};
  txbuf[0]= blockErase32KBInstruction;

  SPIDeviceStatus protectStatus = this->readBlockProtectStatus();
  SPIDeviceStatus blockStatus = this->unLockBlock(addr);
  if(protectStatus == SPIDeviceStatus::blockNotProtect
      && blockStatus == SPIDeviceStatus::blockUnLock ){
       this->enableWrite();
       for (uint8_t index = 1; index<=3 ; index++){
         txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
       }

       SPIDeviceStatus stat = mSpi.write(txbuf, 4);
       this->disableWrite();
       if (stat == SPIDeviceStatus::ok) {
         return SPIDeviceStatus::ok;
       } else {
         return SPIDeviceStatus::writeError;
       }
  } else {
    return SPIDeviceStatus::blockProtect;
  }

}

SPIDeviceStatus SPIFlash::eraseBlock64KB(uint32_t addr){
  uint8_t txbuf[4] = {0};
  txbuf[0]= blockErase64KBInstruction;

  SPIDeviceStatus protectStatus = this->readBlockProtectStatus();
  SPIDeviceStatus blockStatus = this->unLockBlock(addr);
  if(protectStatus == SPIDeviceStatus::blockNotProtect
      && blockStatus == SPIDeviceStatus::blockUnLock ){
       this->enableWrite();
       for (uint8_t index = 1; index<=3 ; index++){
           txbuf[index] = (addr >> (8 * (3-index))) & 0xFF;
       }

       SPIDeviceStatus stat = mSpi.write(txbuf, 4);
       this->disableWrite();
       if (stat == SPIDeviceStatus::ok) {
         return SPIDeviceStatus::ok;
       } else {
         return SPIDeviceStatus::writeError;
       }
 } else {
   return SPIDeviceStatus::blockProtect;
 }

}

SPIDeviceStatus SPIFlash::writeByte(uint32_t addr, uint8_t input){
  static uint8_t  defaultsize = 1;
  uint8_t byteAddr = 0;
  uint8_t temp= writeDataInstruction;

  SPIDeviceStatus status = this->readSpiStatus();
  if(status == SPIDeviceStatus::notbusy){

    this->enableWrite();
    mSpi.write(&temp, defaultsize);

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

    return status;
  } else {
    return status;
  }

}

SPIDeviceStatus SPIFlash::readSpiStatus(void){
  uint8_t txbuf[1] = {0};
  uint8_t rxbuf[1] = {0};
  uint8_t  defaultsize = 1;

  txbuf[0]= readStatusReg1;
  mSpi.writeRead(txbuf, rxbuf, defaultsize);
  if(((*rxbuf) & 0x01) == 1){
    return SPIDeviceStatus::busy;
  } else {
    return SPIDeviceStatus::notbusy;
  }

}

SPIDeviceStatus SPIFlash::protectBlock(void){
  uint8_t txbuf[2]= {writeStatusReg3,writeProtectSelection};
  uint8_t defaultsize = 2;

  SPIDeviceStatus ret = this->readSpiStatus();
  if(ret == SPIDeviceStatus::notbusy){
    this->enableWrite();
    mSpi.write(txbuf, defaultsize);
    this->disableWrite();

    return SPIDeviceStatus::blockProtect;
  } else {
    return ret;
  }

}

SPIDeviceStatus SPIFlash::readBlockProtectStatus(void){
  uint8_t txbuf[1] = {0};
  uint8_t rxbuf[1] = {0};
  uint8_t  defaultsize = 1;

  txbuf[0]= readStatusReg3;
  mSpi.writeRead(txbuf, rxbuf, defaultsize);
  if(((*rxbuf) & 0x04) == 1){
    return SPIDeviceStatus::blockProtect;
  } else {
    return SPIDeviceStatus::blockNotProtect;
  }

}

SPIDeviceStatus SPIFlash::lockBlock(uint32_t addr){
  uint8_t byteAddr =0;
  uint8_t input =lockBlockInstruction;
  uint8_t defaultsize =1;
  SPIDeviceStatus ret;

  SPIDeviceStatus protectStatus = this->readBlockProtectStatus();
  if(protectStatus == SPIDeviceStatus::blockNotProtect){
    this->protectBlock();
  }

  SPIDeviceStatus status = this->readSpiStatus();
  if(status == SPIDeviceStatus::notbusy){
      this->enableWrite();
      mSpi.write(&input, defaultsize);

      byteAddr = static_cast<uint8_t>(((addr&0xFF000000) >> 24) & 0xFF);
      mSpi.write(&byteAddr, defaultsize);

      byteAddr = static_cast<uint8_t>(((addr&0x00FF00000) >> 16) & 0xFF);
      mSpi.write(&byteAddr, defaultsize);

      byteAddr = static_cast<uint8_t>(((addr&0x0000FF00) >> 8) & 0xFF);
      mSpi.write(&byteAddr, defaultsize);

      byteAddr = static_cast<uint8_t>((addr&0x000000FF) & 0xFF);
      mSpi.write(&byteAddr, defaultsize);

      this->disableWrite();
      ret = SPIDeviceStatus::blockLock;
    } else {
      ret = SPIDeviceStatus::busy;
    }
    return ret;
}


SPIDeviceStatus SPIFlash::unLockBlock(uint32_t addr){
  uint8_t byteAddr =0;
  uint8_t input =unlockBlockInstruction;
  uint8_t defaultsize =1;
  SPIDeviceStatus ret;

  SPIDeviceStatus protectStatus = this->readBlockProtectStatus();
  if(protectStatus == SPIDeviceStatus::blockNotProtect){
    this->protectBlock();
  }

  SPIDeviceStatus status = this->readSpiStatus();
  if(status == SPIDeviceStatus::notbusy){
    this->enableWrite();
    mSpi.write(&input, defaultsize);

    byteAddr = static_cast<uint8_t>(((addr&0xFF000000) >> 24) & 0xFF);
    mSpi.write(&byteAddr, defaultsize);

    byteAddr = static_cast<uint8_t>(((addr&0x00FF00000) >> 16) & 0xFF);
    mSpi.write(&byteAddr, defaultsize);

    byteAddr = static_cast<uint8_t>(((addr&0x0000FF00) >> 8) & 0xFF);
    mSpi.write(&byteAddr, defaultsize);

    byteAddr = static_cast<uint8_t>((addr&0x000000FF) & 0xFF);
    mSpi.write(&byteAddr, defaultsize);

    this->disableWrite();
    ret = SPIDeviceStatus::blockUnLock;
  }else {
    ret = SPIDeviceStatus::busy;
  }
    return ret;
}


}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish


