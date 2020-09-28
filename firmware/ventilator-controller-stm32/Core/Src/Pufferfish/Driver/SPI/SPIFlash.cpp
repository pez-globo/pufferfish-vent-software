/// SPIFlash.cpp
/// This file contains methods for SPI flash memory.

// Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Pufferfish/Driver/SPI/SPIFlash.h"

namespace Pufferfish {
namespace Driver {
namespace SPI {

SPIDeviceStatus SPIFlash::getDeviceID(uint8_t &deviceId){
  uint8_t txBuf[6] = {0};
  uint8_t rxBuf[6] = {0};
  uint8_t count = 6;

  /* Update the Byte0 of txBuf with device Id instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::deviceId);

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write and Read data to and from the device */
  SPIDeviceStatus ret = mSpi.writeRead(txBuf, rxBuf, count);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  deviceId = rxBuf[5];
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::getJEDECID(uint16_t &id){
  uint8_t txBuf[4] = {0};
  uint8_t rxBuf[4] = {0};
  uint8_t count = 4;

  /* Update the Byte0 of txBuf with JEDEC Id instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::JEDECId);

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write and Read data to and from the device */
  SPIDeviceStatus ret = mSpi.writeRead(txBuf, rxBuf, count);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  id = rxBuf[2];
  id = (id << 8) & 0xFF00;
  id |= rxBuf[3];
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::enableWrite(void){
  /* Update the txBuf with write enable instruction */
  uint8_t txBuf = static_cast<uint8_t>(SPIInstruction::writeEnable);

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  SPIDeviceStatus ret = mSpi.write(&txBuf, 1);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::disableWrite(void){
  /* Update the txBuf with write disable instruction */
  uint8_t txBuf = static_cast<uint8_t>(SPIInstruction::writeDisable);

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  SPIDeviceStatus ret = mSpi.write(&txBuf, 1);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::writeByte(uint32_t addr, const uint8_t *input, uint8_t size){
  uint8_t regData = 0;
  uint8_t txBuf[size+4] = {0};

  /* Invoke readBlockStatus to get the status of block */
  SPIDeviceStatus blockStatus = this->readBlockStatus(addr);

  if(blockStatus == SPIDeviceStatus::blockLock ){
    /* if block is locked then invoke unLockIndividualBlock to unlock the block */
    SPIDeviceStatus ret = this->unLockIndividualBlock(addr);
    /* return ret if it is not ok */
    if(ret != SPIDeviceStatus::ok){
      return ret;
     }
  }

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Update the Byte0 of txBuf with write byte instruction */
  txBuf[0]= static_cast<uint8_t>(SPIInstruction::writeByte);

  /* Fill the Byte1-Byte3 with address and remaining bytes with input which is to be written */
  for(uint8_t index = 1; index <= (size+4) ; index++){
    if(index < 4){
      txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
    } else {
      txBuf[index] = input[index-4];
    }
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if (ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, size+4);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if (ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* provide a delay of 3ms */
  HAL::delay(3);

  /* Invoke lockIndividualBlock to lock the block */
  ret = this->lockIndividualBlock(addr);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::readByte(uint32_t addr, uint8_t *data, uint8_t size){
  uint8_t txBuf[size+4] = {0};
  uint8_t rxBuf[size+4] = {0};

  /* Update the Byte0 of txBuf with read byte instruction */
  txBuf[0]= static_cast<uint8_t>(SPIInstruction::readByte);

  /* Fill the Byte1-Byte3 with address */
  for (uint8_t index = 1; index<=3 ; index++){
    txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write and Read data to and from the device */
  SPIDeviceStatus ret = mSpi.writeRead(txBuf, rxBuf, size+4);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if (ret != SPIDeviceStatus::ok) {
    return ret;
  }

  for(uint8_t index = 4; index < (size+4); index++){
    data[index - 4] = rxBuf[index];
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::lockIndividualBlock(uint32_t addr){
  uint8_t txBuf[5] = {0};
  uint8_t regData = 0;
  uint8_t size = 4;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Update the Byte0 of txBuf with individual lock block instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::lockBlock);

  /* Fill the Byte1-Byte3 with address */
  for(uint8_t index = 1; index <= 3 ; index++){
     txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, size);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::unLockIndividualBlock(uint32_t addr){
  uint8_t txBuf[5] = {0};
  uint8_t regData = 0;
  uint8_t size = 4;

  /* Input for Write Status Register 3 - WPS is 1 */
  uint8_t reg3Input = 0x04;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Invoke writeStatusRegister3 to make WPS bit to 1 */
  ret = this->writeStatusRegister3(reg3Input);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Update the Byte0 of txBuf with individual block unlock instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::unlockBlock);

  /* Fill the Byte1-Byte3 with address */
  for (uint8_t index = 1; index <= 3 ; index++){
    txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, size);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::globalBlockUnLock(void){
  /* Update the txBuf with global unlock instruction */
  uint8_t txBuf = static_cast<uint8_t>(SPIInstruction::globalUnLock);
  uint8_t regData = 0;
  /* Input for Write Status Register 3 - WPS is 1 */
  uint8_t regInput = 0x04;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Invoke writeStatusRegister3 to make WPS bit to 1 */
  ret = this->writeStatusRegister3(regInput);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(&txBuf, 1);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::globalBlockLock(void){
  /* Update the txBuf with global lock instruction */
  uint8_t txBuf = static_cast<uint8_t>(SPIInstruction::globalLock);
  uint8_t regData = 0;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(&txBuf, 1);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::readBlockStatus(uint32_t addr){
  uint8_t txBuf[5] = {0};
  uint8_t rxBuf[5] = {0};
  uint8_t  size = 5;

  /* Update the Byte0 of txBuf with read block status instruction */
  txBuf[0]= static_cast<uint8_t>(SPIInstruction::readBlockStatus);

  /* Fill the Byte1-Byte3 with address */
  for (uint8_t index = 1; index<=3 ; index++){
    txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write and Read data to and from the device */
  SPIDeviceStatus ret = mSpi.writeRead(txBuf, rxBuf, size);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as blockLock */
  if(((rxBuf[4]) & 0x01) == 1){
    return SPIDeviceStatus::blockLock;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::eraseChip(void){
  uint8_t regData = 0;
  /* Update the txBuf with chip erase instruction */
  uint8_t txBuf = static_cast<uint8_t>(SPIInstruction::chipErase);

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Invoke globalBlockUnLock to unlock all the blocks */
  ret = this->globalBlockUnLock();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
     return ret;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(&txBuf, 1);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* provide a delay of 25000ms */
  HAL::delay(25000);

  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::eraseSector4KB(uint32_t addr){
  uint8_t txBuf[4] = {0};
  uint8_t regData = 0;

  /* Invoke readBlockStatus to get the status of block */
  SPIDeviceStatus blockStatus = this->readBlockStatus(addr);
  if(blockStatus == SPIDeviceStatus::blockLock ){
    /* if block is locked then invoke unLockIndividualBlock to unlock the block */
    SPIDeviceStatus ret = this->unLockIndividualBlock(addr);
    /* return ret if it is not ok */
    if(ret != SPIDeviceStatus::ok) {
      return ret;
    }
  }

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Update the Byte0 of txBuf with sector erase of 4KB instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::sectorErase4KB);

  /* Fill the Byte1-Byte3 with address */
  for(uint8_t index = 1; index<=3 ; index++){
     txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, 4);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if (ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* provide a delay of 400ms */
  HAL::delay(400);

  /* Invoke lockIndividualBlock to lock the block */
  ret = this->lockIndividualBlock(addr);
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::eraseBlock32KB(uint32_t addr){
  uint8_t txBuf[4] = {0};
  uint8_t regData = 0;

  /* Invoke readBlockStatus to get the status of block */
  SPIDeviceStatus blockStatus = this->readBlockStatus(addr);
  if(blockStatus == SPIDeviceStatus::blockLock ){
    /* if block is locked then invoke unLockIndividualBlock to unlock the block */
    SPIDeviceStatus ret = this->unLockIndividualBlock(addr);
    /* return ret if it is not ok */
    if(ret != SPIDeviceStatus::ok) {
      return ret;
    }
  }

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
     return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Update the Byte0 of txBuf with block erase 32KB instruction */
  txBuf[0]= static_cast<uint8_t>(SPIInstruction::blockErase32KB);

  /* Fill the Byte1-Byte3 with address */
  for(uint8_t index = 1; index<=3 ; index++){
     txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, 4);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* provide a delay of 1600ms */
  HAL::delay(1600);

  /* Invoke lockIndividualBlock to lock the block */
  ret = this->lockIndividualBlock(addr);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::eraseBlock64KB(uint32_t addr){
  uint8_t txBuf[4] = {0};
  uint8_t regData = 0;

  /* Invoke readBlockStatus to get the status of block */
  SPIDeviceStatus blockStatus = this->readBlockStatus(addr);
  if(blockStatus == SPIDeviceStatus::blockLock ){
    /* if block is locked then invoke unLockIndividualBlock to unlock the block */
    SPIDeviceStatus ret = this->unLockIndividualBlock(addr);
    /* return ret if it is not ok */
    if(ret != SPIDeviceStatus::ok) {
      return ret;
    }
  }

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
     return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Update the Byte0 of txBuf with block erase 64KB instruction */
  txBuf[0]= static_cast<uint8_t>(SPIInstruction::blockErase64KB);

  /* Fill the Byte1-Byte3 with address */
  for(uint8_t index = 1; index<=3 ; index++){
     txBuf[index] = (addr >> (8 * (3-index))) & 0xFF;
  }

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, 4);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }

  /* provide a delay of 2000ms */
  HAL::delay(2000);

  /* Invoke lockIndividualBlock to lock the block */
  ret = this->lockIndividualBlock(addr);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok) {
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::writeStatusRegister1(uint8_t input){
  uint8_t txBuf[2] = {0};
  uint8_t regData = 0;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(regData);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((regData & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Update the Byte0 of txBuf with write status register 1 instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::writeStatusRegister1);

  /* Update the Byte1 of txBuf with input which is to be written */
  txBuf[1] = (regData & input);

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, 2);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* provide a delay of 15ms */
  HAL::delay(15);

  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::readStatusRegister1(uint8_t &rxBuf){
  uint8_t txBuf[2] = {0};
  uint8_t temp[2] = {0};

  /* Update the Byte0 of txBuf with read status register 1 instruction */
  txBuf[0]= static_cast<uint8_t>(SPIInstruction::readStatusRegister1);

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write and Read data to and from the device */
  SPIDeviceStatus ret = mSpi.writeRead(txBuf, temp, 2);

  /* Make the CS pin High after read operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  rxBuf = temp[1];
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::writeStatusRegister2(uint8_t input){
  uint8_t txBuf[2] = {0};
  uint8_t dataReg1 = 0;
  uint8_t dataReg2 = 0;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(dataReg1);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((dataReg1 & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Invoke readStatusRegister2 to get the register data */
  ret = this->readStatusRegister2(dataReg2);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Update the Byte0 of txBuf with write status register 2 instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::writeStatusRegister2);

  /* Update the Byte1 of txBuf with input which is to be written */
  txBuf[1] = (dataReg2 & input) ;

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret =  mSpi.write(txBuf, 2);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* provide a delay of 15ms */
  HAL::delay(15);
  /* return SPIDeviceStatus */
  return ret;
}


SPIDeviceStatus SPIFlash::readStatusRegister2(uint8_t &rxBuf){
  uint8_t txBuf[2] = {0};
  uint8_t temp[2] = {0};

  /* Update the Byte0 of txBuf with read status register 2 instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::readStatusRegister2);

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write and Read data to and from the device */
  SPIDeviceStatus ret = mSpi.writeRead(txBuf, temp, 2);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  rxBuf = temp[1];
  /* return SPIDeviceStatus */
  return ret;
}


SPIDeviceStatus SPIFlash::writeStatusRegister3(uint8_t input){
  uint8_t txBuf[2] = {0};
  uint8_t dataReg1 = 0;
  uint8_t dataReg3 = 0;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(dataReg1);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((dataReg1 & 0x01) == 1){
    return SPIDeviceStatus::busy;
  }

  /* Invoke readStatusRegister3 to get the status register 3 data */
  ret = this->readStatusRegister3(dataReg3);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Update the Byte0 of txBuf with write status register 3 instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::writeStatusRegister3);

  /* Update the Byte1 of txBuf with input which is to be written */
  txBuf[1] = (dataReg3 | input);

  /* Invoke enableWrite to set the WEL bit to 1 */
  ret = this->enableWrite();
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, 2);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* provide a delay of 15ms */
  HAL::delay(15);
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::readStatusRegister3(uint8_t &rxBuf){
  uint8_t txBuf[2] = {0};
  uint8_t temp[2] = {0};

  /* Update the Byte0 of txBuf with read status register 3 instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::readStatusRegister3);

  /* Make the CS pin Low before read operation*/
  mSpi.chipSelect(false);

  /* Write and Read data to and from the device */
  SPIDeviceStatus ret = mSpi.writeRead(txBuf, temp, 2);

  /* Make the CS pin high after read operation*/
  mSpi.chipSelect(true);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  rxBuf = temp[1];
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::powerDown(void){
  /* Update the txBuf with power down instruction */
  uint8_t txBuf = static_cast<uint8_t>(SPIInstruction::powerDown);

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  SPIDeviceStatus ret = mSpi.write(&txBuf, 1);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* provide a delay of 3microsec */
  HAL::delayMicros(3);
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::releasePowerDown(void){
  uint8_t txBuf[4] = {0};

  /* Update the Byte0 of txBuf with release power down instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::releasePowerDown);

  /* Make the CS pin Low before write operation*/
  mSpi.chipSelect(false);

  /* Write data into the device */
  SPIDeviceStatus ret = mSpi.write(txBuf, 4);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* provide a delay of 3microsec */
  HAL::delayMicros(3);
  /* return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::resetDevice(void){
  uint8_t txBuf[1] = {0};
  uint8_t data = 0;

  /* Invoke readStatusRegister1 to get the status of device */
  SPIDeviceStatus ret = this->readStatusRegister1(data);
  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
   return ret;
  }

  /* if LSB bit is 1 then return SPIDeviceStatus as busy */
  if((data & 0x01) == 1){
   return SPIDeviceStatus::busy;
  }

  /* Update the Byte0 of txBuf with  enable reset instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::resetEnable);

  /* Make the CS pin Low before write operation */
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, 1);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }

  /* provide a delay of 30 microsec */
  HAL::delayMicros(30);

  /* Update the Byte0 of txBuf with reset device instruction */
  txBuf[0] = static_cast<uint8_t>(SPIInstruction::resetDevice);

  /* Make the CS pin Low after write operation */
  mSpi.chipSelect(false);

  /* Write data into the device */
  ret = mSpi.write(txBuf, 1);

  /* Make the CS pin High after write operation */
  mSpi.chipSelect(true);

  /* return ret if it is not ok */
  if(ret != SPIDeviceStatus::ok){
    return ret;
  }
  /* return SPIDeviceStatus */
  return ret;
}

}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish
