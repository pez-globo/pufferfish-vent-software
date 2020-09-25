/// SPIFlash.h
/// This file contains class and its methods prototype for SPI flash memory.

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

#pragma once

#include "Pufferfish/HAL/HAL.h"

namespace Pufferfish {
namespace Driver {
namespace SPI {

/**
 * A class represents external SPI flash memory w25q16
 */
class SPIFlash {

public:
  /**
   * @brief Constructor for SPI Flash memory
   * @param spi STM32 HAL handler for the SPI port
   */
  SPIFlash (HAL::SPIDevice &spi):mSpi(spi){
  }

  /**
   * @brief Read the specific device ID (14h).
   * @param deviceId output of the data
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus getDeviceID(uint8_t &deviceId);

  /**
   * @brief Read JEDEC device ID (4015h).
   * @param deviceId output of the data
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus getJEDECID(uint16_t &id);

  /**
   * @brief Enable write for SPI device - It sets the Write Enable Latch (WEL)
   * bit in the Status Register to 1.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus enableWrite(void);

  /**
   * @brief Disable write for SPI device - It resets the Write Enable Latch (WEL)
   * bit in the Status Register to 0.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus disableWrite(void);

  /**
   * @brief Write bytes of data into SPI device
   * @param addr address to write data
   * @param input data to be written
   * @param size amount of data to be transmit
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus writeByte(uint32_t addr, const uint8_t *input, uint8_t size);

  /**
   * @brief Read bytes of data from SPI device
   * @param addr address to read data
   * @param size amount of data to be receive
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus readByte(uint32_t addr, uint8_t *data, uint8_t size);

  /**
   * @brief Lock the block based on address - To protect the memory
   * array from Erase/Program.
   * @param addr address of the lock block
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus lockIndividualBlock(uint32_t addr);

  /**
   * @brief Unlock the block based on address
   * @param addr address of the unlock block
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus unLockIndividualBlock(uint32_t addr);

  /**
   * @brief Lock the block globally - All Block/Sector Lock bits can
   * be set to 1 by the Global Block/Sector Lock instruction.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus globalBlockLock(void);

  /**
   * @brief Unlock the block globally - All Block/Sector Lock bits can
   * be set to 0 by the Global Block/Sector UnLock instruction.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus globalBlockUnLock(void);

  /**
   * @brief Read the block status - To read out the lock bit value
   * of a specific block or sector. If LSB is 1,
   * the corresponding block/sector is locked;
   * if LSB is 0, the corresponding block/sector is unlocked
   * @param addr address of the block
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus readBlockStatus(uint32_t addr);

  /**
   * @brief Chip Erase
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus eraseChip(void);

  /**
   * @brief Sector Erase - The Sector Erase instruction sets all memory within
   * a specified sector (4K-bytes) to the erased state of all 1s (FFh).
   * @param addr address to erase the sector
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus eraseSector4KB(uint32_t addr);

  /**
   * @brief Block Erase - The Block Erase instruction sets all memory within
   * a specified sector (32K-bytes) to the erased state of all 1s (FFh).
   * @param addr address to erase the block
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus eraseBlock32KB(uint32_t addr);

  /**
   * @brief Block Erase - The Block Erase instruction sets all memory within
   * a specified sector (64K-bytes) to the erased state of all 1s (FFh).
   * @param addr address to erase the block
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus eraseBlock64KB(uint32_t addr);

  /**
   * @brief Write status register1 - The Write Status Register instruction
   * allows the Status Registers to be written.
   * @param input data which is to be written into register1
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus writeStatusRegister1(uint8_t input);

  /**
   * @brief Read status register1 - It checks the BUSY status bit to determine
   * when the cycle is complete and if the device can accept another instruction.
   * It may be used at any time, even while a Program, Erase or Write Status
   * Register cycle is in progress.
   * @param rxbuf pointer to reception data buffer
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus readStatusRegister1(uint8_t &rxBuf);

  /**
   * @brief Write status register2 - The Write Status Register instruction
   * allows the Status Registers to be written.
   * @param input data which is to be written into register2
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus writeStatusRegister2(uint8_t input);

  /**
   * @brief Read status register2 - The Read Status Register instructions
   * allow the 8-bit Status Registers to be read.
   * @param rxbuf pointer to reception data buffer
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus readStatusRegister2(uint8_t &rxBuf);

  /**
   * @brief Write status register3 - The Write Status Register instruction
   * allows the Status Registers to be written.
   * @param input data which is to be written into register3
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus writeStatusRegister3(uint8_t input);

  /**
   * @brief Read status register3 - The Read Status Register instructions
   * allow the 8-bit Status Registers to be read.
   * @param rxbuf pointer to reception data buffer
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus readStatusRegister3(uint8_t &rxBuf);

  /**
   * @brief Power down - It is used to enter the
   * device into power-down state
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus powerDown(void);

  /**
   * @brief Release power down - It is used to release the
   * device from the power-down state
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus releasePowerDown(void);

  /**
   * @brief Reset device - It is used to reset the device
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus resetDevice(void);

private:
  HAL::SPIDevice &mSpi;
};

}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish
