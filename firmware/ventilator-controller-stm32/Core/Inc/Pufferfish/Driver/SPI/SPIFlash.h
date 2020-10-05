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

#include "Pufferfish/HAL/Interfaces/SPIDevice.h"
#include "Pufferfish/HAL/STM32/HALTime.h"

namespace Pufferfish::Driver::SPI {

/**
 * SPI Instructions
 */
enum class SPIInstruction {
  device_id = 0x90,               /// Instruction for read Device ID
  jedec_id = 0x9F,                /// Instruction for read JEDEC ID
  write_enable = 0x06,            /// Instruction for write enable
  write_disable = 0x04,           /// Instruction for write disable
  write_byte = 0x02,              /// Instruction for write byte
  read_byte = 0x03,               /// Instruction for read byte
  lock_block = 0x36,              /// Instruction for individual lock block
  unlock_block = 0x39,            /// Instruction for individual Unlock block
  global_lock = 0x7E,             /// Instruction for global lock block
  global_unlock = 0x98,           /// Instruction for global Unlock block
  read_block_status = 0x3D,       /// Instruction for read block status
  chip_erase = 0xC7,              /// Instruction for chip erase
  sector_erase_4kb = 0x20,        /// Instruction for sector erase of 4KB
  block_erase_32kb = 0x52,        /// Instruction for block erase of 32KB
  block_erase_64kb = 0xD8,        /// Instruction for block erase of 64KB
  write_status_register1 = 0x01,  /// Instruction for write status register 1
  read_status_register1 = 0x05,   /// Instruction for read status register 1
  write_status_register2 = 0x31,  /// Instruction for write status register 2
  read_status_register2 = 0x35,   /// Instruction for read status register 2
  write_status_register3 = 0x11,  /// Instruction for write status register 3
  read_status_register3 = 0x15,   /// Instruction for read status register 3
  power_down = 0xB9,              /// Instruction for power down
  release_power_down = 0xAB,      /// Instruction for release power down
  reset_enable = 0x66,            /// Instruction for enable reset
  reset_device = 0x99             /// Instruction for reset device
};

/**
 * Memory Status
 */
struct MemoryStatus {
  bool lock;          /// Status of page/sector/block - lock/unlock
  bool not_empty;     /// Status of page/sector/block - empty or not
  bool busy;          /// Status of SPI Flash busy
  bool enable_latch;  /// Status of write enable latch bit
};

/**
 * A class represents external SPI flash memory w25q16
 */
class SPIFlash {
 public:
  /**
   * Constructor for SPI Flash memory
   * @param spi STM32 HAL handler for the SPI port
   */
  explicit SPIFlash(HAL::SPIDevice &spi, HAL::Time &time) : spi_(spi), time_(time) {}

  /**
   * It is used to read the data from device.
   * @param tx_buf pointer to transmission data buffer
   * @param rx_buf pointer to reception data buffer
   * @param count the number of bytes to be read
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus mem_read(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t count);

  /**
   * It is used to write data into the device.
   * @param txBuf pointer to transmission data buffer
   * @param count the number of bytes to write
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus mem_write(uint8_t *tx_buf, uint16_t count);

  /**
   * Read the specific device ID (14h).
   * @param deviceId output of the data
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus get_device_id(uint8_t &device_id);

  /**
   * Read JEDEC device ID (4015h).
   * @param jedecId output of the data
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus get_jedec_id(uint16_t &id);

  /**
   * Enable write for SPI device - It sets the Write Enable Latch (WEL)
   * bit in the Status Register to 1.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus enable_write();

  /**
   * Disable write for SPI device - It resets the Write Enable Latch (WEL)
   * bit in the Status Register to 0.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus disable_write();

  /**
   * Read Busy Status - It checks the BUSY status bit to determine
   * when the cycle is complete and if the device can accept another instruction.
   * It may be used at any time, even while a Program, Erase or Write Status
   * Register cycle is in progress.
   * @param status to check the status of busy bit
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus read_busy_status(MemoryStatus &status);

  /**
   * Read Enable Latch - It checks the status of write enable latch(WEL) bit.
   * @param status to check the status of WEL bit
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus read_enable_latch(MemoryStatus &status);

  /**
   * Write Protect Selection - It is used to set the write protect select(WPS) bit.
   * @param protect input of the write protect selection bit true/false
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus write_protect_selection(bool protect);

  /**
   * Lock the block/sector based on address - To protect the memory
   * array from Erase/Program.
   * @param addr address of the lock block/sector
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus lock_individual_memory(uint32_t addr);

  /**
   * Unlock the block/sector based on address
   * @param addr address of the unlock block/sector
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus unlock_individual_memory(uint32_t addr);

  /**
   * Lock the block globally - All Block/Sector Lock bits can
   * be set to 1 by the Global Block/Sector Lock instruction.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus global_lock_memory();

  /**
   * Unlock the block globally - All Block/Sector Lock bits can
   * be set to 0 by the Global Block/Sector UnLock instruction.
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus global_unlock_memory();

  /**
   * Read the block/sector status - To read out the lock bit value
   * of a specific block or sector. If LSB is 1,
   * the corresponding block/sector is locked;
   * if LSB is 0, the corresponding block/sector is unlocked
   * @param addr address of the block/sector
   * @param status to check the status of lock bit
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus read_memory_status(uint32_t addr, MemoryStatus &status);

  /**
   * Chip Erase
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus erase_chip();

  /**
   * Sector Erase - The Sector Erase instruction sets all memory within
   * a specified sector (4K-bytes) to the erased state of all 1s (FFh).
   * @param addr address to erase the sector of 4KB
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus erase_sector_4kb(uint32_t addr);

  /**
   * Block Erase - The Block Erase instruction sets all memory within
   * a specified sector (32K-bytes) to the erased state of all 1s (FFh).
   * @param addr address to erase the block of 32KB
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus erase_block_32kb(uint32_t addr);

  /**
   * Block Erase - The Block Erase instruction sets all memory within
   * a specified sector (64K-bytes) to the erased state of all 1s (FFh).
   * @param addr address to erase the block of 64KB
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus erase_block_64kb(uint32_t addr);

  /**
   * Power down - It is used to enter the
   * device into power-down state
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus power_down();

  /**
   * Release power down - It is used to release the
   * device from the power-down state
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus release_power_down();

  /**
   * Reset device - It is used to reset the device
   * @param void
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus reset_device();

  /**
   * IsEmpty - It is used to check the page/sector/block is empty or not.
   * @param addr address of the sector/block
   * @param size amount of data to be check empty or not
   * @param status to check the status of page/sector/block is empty or not
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus is_empty(uint32_t addr, uint16_t size, MemoryStatus &status);

  /**
   * Read bytes of data from SPI device
   * @param addr address to read data
   * @param data pointer to reception data buffer
   * @param size the number of bytes to read
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus read_byte(uint32_t addr, uint8_t *data, uint16_t size);

  /**
   * Write bytes of data into SPI device
   * @param addr address to write data
   * @param dataBuf pointer to transmission data buffer
   * @param length the number of bytes to write
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus write_byte(uint32_t addr, const uint8_t *data_buf, uint8_t length);

  /**
   * WritePage - It is used to write the data in a page.
   * @param addr address to write data
   * @param dataBuf pointer to transmission data buffer
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus write_page(uint32_t addr, const uint8_t *data_buf);

  /**
   * WriteSector - It is used to write the data in page by page.
   * @param addr address to write data
   * @param txBuf pointer to transmission data buffer
   * @param length the number of bytes to write
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus write_sector(uint32_t addr, const uint8_t *tx_buf, uint16_t length);

  /**
   * Write To Memory - It is used to write the data into a memory.
   * @param addr address to write data
   * @param dataBuf pointer to transmission data buffer
   * @param length the number of bytes to write
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus write_to_memory(uint32_t addr, const uint8_t *data_buf, uint16_t length);

 private:
  HAL::SPIDevice &spi_;
  HAL::Time &time_;
  static const uint16_t page_size = 0x100;
};

}  // namespace Pufferfish::Driver::SPI
