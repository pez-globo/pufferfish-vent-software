/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *
 */

#pragma once

#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {
namespace SPI {

/**
 * A class represents SPI flash memory in STM32
 */

class SPIFlash {

public:
  SPIFlash (HAL::SPIDevice &spi, HAL::HALSPIDevice &cspin )
            :mSpi(spi), mCsPin(cspin){
  }

  /** Read spi device id
   *
   * @param void
   * @return deviceid- ID based on device
   */
   uint16_t getDeviceID(void);

   /** Enable write for spi device
     *
     * @param void
     * @return ok on success, error code otherwise
     */
   SPIDeviceStatus enableWrite(void);

   /** Disable write for spi device
     *
     * @param void
     * @return ok on success, error code otherwise
     */
   SPIDeviceStatus disableWrite(void);

   /** Read data from spi device
     *
     * @param addr address to read data
     * @param rxBuf pointer to reception data buffer
     * @param size amount of data to be receive
     * @return ok on success, error code otherwise
     */
   SPIDeviceStatus readData(uint32_t addr, uint8_t *rxBuf, uint8_t size);

   /** Chip Erase
     *
     * @param void
     * @return true on success, false otherwise
     */
   bool eraseChip(void);

   /** Sector Erase - 4KB
     *
     * @param addr address to erase the sector
     * @return ok on success, writeError otherwise
     */
   SPIDeviceStatus eraseSector4KB(uint32_t addr);

   /** Block Erase - 32KB
     *
     * @param addr address to erase the block
     * @return ok on success, writeError otherwise
     */
   SPIDeviceStatus eraseBlock32KB(uint32_t addr);

   /** Block Erase - 64KB
     *
     * @param addr address to erase the block
     * @return ok on success, writeError otherwise
     */
   SPIDeviceStatus eraseBlock64KB(uint32_t addr);

   /** write byte into spi device
     *
     * @param addr address to write data
     * @param input data to be written
     * @return ok on success, blockLock otherwise
     */
   SPIDeviceStatus writeByte(uint32_t addr, uint8_t input);

   /** read status from spi device
     *
     * @param void
     * @param input data to be written
     * @return notBusy on success, busy otherwise
     */
   SPIDeviceStatus readSpiStatus(void);

   /** protect block for spi device
     *
     * @param void
     * @param input data to be written
     * @return blockProtect on success, busy otherwise
     */
   SPIDeviceStatus protectBlock(void);

   /** read block protect status from spi device
     *
     * @param void
     * @param input data to be written
     * @return blockProtect on success, blockNotProtect otherwise
     */
   SPIDeviceStatus readBlockProtectStatus(void);

   /** lock the block based on address
     *
     * @param addr address of the lock block
     * @return blockLock on success, blockUnLock otherwise
     */
   SPIDeviceStatus lockBlock(uint32_t addr);

   /** unlock the block based on address
     *
     * @param addr address of the unlock block
     * @return blockUnLock on success, blockLock otherwise
     */
   SPIDeviceStatus unLockBlock(uint32_t addr);

   /** read the block status
     *
     * @param addr address of the block
     * @return blockLock on success, blockUnLock otherwise
     */
   SPIDeviceStatus readBlockLockStatus(uint32_t addr);

   /** write data into a page
     *
     * @param addr address of the page
     * @param data to be written
     * @param size size of the data
     * @return ok on success, busy otherwise
     */
   SPIDeviceStatus pageProgram(uint32_t addr, uint8_t *data, uint16_t size);

   /** device power down
     *
     * @param void
     * @return ok on success, writeError otherwise
     */
   SPIDeviceStatus powerDown(void);

   /** device release power down
     *
     * @param void
     * @return ok on success, writeError otherwise
     */
   SPIDeviceStatus releasePowerDown(void);

private:
   HAL::SPIDevice &mSpi;
   HAL::HALSPIDevice &mCsPin;
   static const uint8_t deviceIdInstruction = 0x90;
   static const uint8_t readDataInstruction = 0x03;
   static const uint8_t writeDataInstruction = 0x02;
   static const uint8_t writeEnableInstruction = 0x06;
   static const uint8_t writeDisableInstruction = 0x04;
   static const uint8_t chipEraseInstruction = 0x60;
   static const uint8_t sectorErase4KBInstruction = 0x20;
   static const uint8_t blockErase32KBInstruction = 0x52;
   static const uint8_t blockErase64KBInstruction = 0xD8;
   static const uint8_t readStatusReg1 = 0x05;
   static const uint8_t writeStatusReg3 = 0x11;
   static const uint8_t readStatusReg3 = 0x15;
   static const uint8_t writeProtectSelection = 0x04;
   static const uint8_t lockBlockInstruction = 0x36;
   static const uint8_t unlockBlockInstruction = 0x39;
   static const uint8_t readBlockStatusInstruction = 0x3D;
   static const uint8_t powerDownInstruction = 0xB9;
   static const uint8_t releasePowerDownInstruction = 0xAB;
};

}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish
