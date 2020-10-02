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

#include <array>
#include <climits>

namespace Pufferfish::Driver::SPI {

SPIDeviceStatus SPIFlash::mem_read(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t count) {
  /* Make the CS pin Low before read operation*/
  spi_.chip_select(false);

  /* Read data from the device */
  if (spi_.write_read(tx_buf, rx_buf, count) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Make the CS pin High after read operation */
  spi_.chip_select(true);

  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::mem_write(uint8_t *tx_buf, uint16_t count) {
  /* Make the CS pin Low before write operation*/
  spi_.chip_select(false);

  /* Write data into the device */
  if (spi_.write(tx_buf, count) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Make the CS pin High after write operation */
  spi_.chip_select(true);

  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::get_device_id(uint8_t &device_id) {
  static const uint8_t count = 6;
  std::array<uint8_t, count> tx_buf = {0};
  std::array<uint8_t, count> rx_buf = {0};

  /* Update the Byte0 of txBuf with device Id instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::device_id);

  /* Invoke memRead to read data from the device */
  if (this->mem_read(tx_buf.data(), rx_buf.data(), count) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  device_id = rx_buf[count - 1];

  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::get_jedec_id(uint16_t &id) {
  static const uint8_t count = 4;
  std::array<uint8_t, count> tx_buf = {0};
  std::array<uint8_t, count> rx_buf = {0};

  /* Update Byte0 of txBuf with JEDEC Id instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::jedec_id);

  /* Invoke memRead to read data from the device */
  if (this->mem_read(tx_buf.data(), rx_buf.data(), count) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  static const uint16_t mask = 0xFF00;
  id = rx_buf[2];
  id = static_cast<uint16_t>(id << static_cast<uint16_t>(CHAR_BIT)) & mask;
  id |= rx_buf[3];

  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::enable_write() {
  static const uint8_t count = 1;

  /* Update txBuf with write enable instruction */
  auto tx_buf = static_cast<uint8_t>(SPIInstruction::write_enable);

  /* Invoke memWrite to write data into the device */
  if (this->mem_write(&tx_buf, count) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::disable_write() {
  static const uint8_t count = 1;

  /* Update txBuf with write disable instruction */
  auto tx_buf = static_cast<uint8_t>(SPIInstruction::write_disable);

  /* Invoke memWrite to write data into the device */
  if (this->mem_write(&tx_buf, count) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::read_busy_status(MemoryStatus &status) {
  static const size_t size = 2;
  std::array<uint8_t, size> tx_buf = {0};
  std::array<uint8_t, size> rx_buf = {0};

  /* Update Byte0 of txBuf with read status register 1 instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::read_status_register1);

  /* Invoke memRead to read data from the device */
  if (this->mem_read(tx_buf.data(), rx_buf.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Check SPI flash is busy or not */
  status.busy = (rx_buf[1] & 0x01) != 0x00 ? true : false;

  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::read_enable_latch(MemoryStatus &status) {
  static const size_t size = 2;
  std::array<uint8_t, size> tx_buf = {0};
  std::array<uint8_t, size> rx_buf = {0};

  /* Update Byte0 of txBuf with read status register 1 instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::read_status_register1);

  /* Invoke memRead to read data from the device */
  if (this->mem_read(tx_buf.data(), rx_buf.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Check the status of write enable latch bit */
  status.enable_latch = static_cast<bool>(rx_buf[1] & 0x02) != 0 ? true : false;

  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::write_protect_selection(bool protect) {
  static const size_t size = 2;
  std::array<uint8_t, size> tx_buf = {0};
  std::array<uint8_t, size> rx_buf = {0};

  /* Update Byte0 of txBuf with read status register 3 instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::read_status_register3);

  /* Invoke memRead to read data from the device */
  if (this->mem_read(tx_buf.data(), rx_buf.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* Update Byte0 of txBuf with write status register 3 instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::write_status_register1);

  /* Update Byte1 of txBuf with input which is to be written */
  tx_buf[1] = (static_cast<uint8_t>(protect) << 2) | rx_buf[1];

  /* Invoke enableWrite to set the WEL bit to 1 */
  if (this->enable_write() != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Invoke memWrite to write data into the device */
  if (this->mem_write(tx_buf.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Provide a delay of 15ms */
  static const uint8_t write_reg_delay = 15;
  time_.delay(write_reg_delay);
  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::unlock_individual_memory(uint32_t addr) {
  static const uint8_t size = 4;
  std::array<uint8_t, size + 1> tx_buf = {0};
  bool protect = true;
  MemoryStatus status{};

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke writeStatusRegister3 to make WPS bit to 1 */
    if (this->write_protect_selection(protect) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }

    /* Update the Byte0 of txBuf with individual block/sector unlock instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::unlock_block);

    /* Fill the Byte1-Byte3 with address */
    for (uint8_t index = 1; index <= 3; index++) {
      tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(tx_buf.data(), size) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::lock_individual_memory(uint32_t addr) {
  static const uint8_t size = 4;
  std::array<uint8_t, size + 1> tx_buf = {0};
  MemoryStatus status{};
  bool protect = true;

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke writeStatusRegister3 to make WPS bit to 1 */
    if (this->write_protect_selection(protect) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }

    /* Update the Byte0 of txBuf with individual block/sector lock instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::lock_block);

    /* Fill the Byte1-Byte3 with address */
    for (uint8_t index = 1; index <= 3; index++) {
      tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(tx_buf.data(), size) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  }
  /* Return busy */
  return SPIDeviceStatus::busy;
}

SPIDeviceStatus SPIFlash::global_unlock_memory() {
  static const uint8_t size = 1;
  /* Update the txBuf with global unlock block/sector instruction */
  auto tx_buf = static_cast<uint8_t>(SPIInstruction::global_unlock);
  bool protect = true;
  MemoryStatus status{};

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke writeStatusRegister3 to make WPS bit to 1 */
    if (this->write_protect_selection(protect) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(&tx_buf, size) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::global_lock_memory() {
  static const uint8_t size = 1;
  /* Update the txBuf with global lock block/sector instruction */
  auto tx_buf = static_cast<uint8_t>(SPIInstruction::global_lock);
  MemoryStatus status{};
  bool protect = true;

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke writeStatusRegister3 to make WPS bit to 1 */
    if (this->write_protect_selection(protect) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(&tx_buf, size) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::read_memory_status(uint32_t addr, MemoryStatus &status) {
  static const uint8_t size = 5;
  std::array<uint8_t, size> tx_buf = {0};
  std::array<uint8_t, size> rx_buf = {0};

  /* Update the Byte0 of txBuf with read block/sector status instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::read_block_status);

  /* Fill the Byte1-Byte3 with address */
  for (uint8_t index = 1; index <= 3; index++) {
    tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
  }
  /* Invoke memRead to read data from the device */
  if (this->mem_read(tx_buf.data(), rx_buf.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* If LSB bit is 1 then block is locked */
  status.lock = ((rx_buf[4] & 0x01) != 0x00) ? true : false;

  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::erase_chip() {
  static const uint8_t length = 1;
  /* Update the txBuf with chip erase instruction */
  auto tx_buf = static_cast<uint8_t>(SPIInstruction::chip_erase);
  MemoryStatus status{};

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke globalUnLockMemory to unlock all the blocks/sectors */
    if (this->global_unlock_memory() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(&tx_buf, length) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Provide a delay of 25000ms */
    static const uint32_t erase_delay = 25000;
    time_.delay(erase_delay);
    /* Invoke globalLockMemory to lock all the blocks/sectors */
    if (this->global_lock_memory() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::erase_sector_4kb(uint32_t addr) {
  static const size_t size = 4;
  std::array<uint8_t, size> tx_buf = {0};
  MemoryStatus status{};

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke readMemoryStatus to get the status of block/sector */
    if (this->read_memory_status(addr, status) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::read_error;
    }

    if (status.lock) {
      /* If block is locked then invoke unLockIndividualMemory to unlock the block/sector */
      if (this->unlock_individual_memory(addr) != SPIDeviceStatus::ok) {
        return SPIDeviceStatus::write_error;
      }
    }

    /* Update the Byte0 of txBuf with sector erase of 4KB instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::sector_erase_4kb);

    /* Fill the Byte1-Byte3 with address */
    for (uint8_t index = 1; index <= 3; index++) {
      tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(tx_buf.data(), size) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Provide a delay of 400ms */
    static const uint32_t erase_delay = 400;
    time_.delay(erase_delay);
    /* Invoke lockIndividualMemory to lock the block/sector */
    if (this->lock_individual_memory(addr) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::erase_block_32kb(uint32_t addr) {
  static const size_t size = 4;
  std::array<uint8_t, size> tx_buf = {0};
  MemoryStatus status;

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke readMemoryStatus to get the status of block/sector */
    if (this->read_memory_status(addr, status) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::read_error;
    }

    if (status.lock) {
      /* If block is locked then invoke unLockIndividualMemory to unlock the block/sector */
      if (this->unlock_individual_memory(addr) != SPIDeviceStatus::ok) {
        return SPIDeviceStatus::write_error;
      }
    }

    /* Update the Byte0 of txBuf with block erase 32KB instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::block_erase_32kb);

    /* Fill the Byte1-Byte3 with address */
    for (uint8_t index = 1; index <= 3; index++) {
      tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(tx_buf.data(), size) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Provide a delay of 1600ms */
    static const uint32_t erase_delay = 1600;
    time_.delay(erase_delay);
    /* Invoke lockIndividualMemory to lock the block/sector */
    if (this->lock_individual_memory(addr) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::erase_block_64kb(uint32_t addr) {
  static const size_t size = 4;
  std::array<uint8_t, size> tx_buf = {0};
  MemoryStatus status;

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke readMemoryStatus to get the status of block/sector */
    if (this->read_memory_status(addr, status) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::read_error;
    }

    if (status.lock) {
      /* If block is locked then invoke unLockIndividualMemory to unlock the block/sector */
      if (this->unlock_individual_memory(addr) != SPIDeviceStatus::ok) {
        return SPIDeviceStatus::write_error;
      }
    }

    /* Update the Byte0 of txBuf with block erase 64KB instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::block_erase_64kb);

    /* Fill the Byte1-Byte3 with address */
    for (uint8_t index = 1; index <= 3; index++) {
      tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(tx_buf.data(), size) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Provide a delay of 2000ms */
    static const uint32_t erase_delay = 2000;
    time_.delay(erase_delay);
    /* Invoke lockIndividualMemory to lock the block/sector */
    if (this->lock_individual_memory(addr) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::power_down() {
  static const uint8_t length = 1;
  /* Update the txBuf with power down instruction */
  auto tx_buf = static_cast<uint8_t>(SPIInstruction::power_down);

  /* Invoke enableWrite to set the WEL bit to 1 */
  if (this->enable_write() != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Invoke memWrite to write data into the device */
  if (this->mem_write(&tx_buf, length) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Provide a delay of 3microsec */
  static const uint8_t power_down_delay = 3;
  time_.delay_micros(power_down_delay);
  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::release_power_down() {
  static const size_t count = 4;
  std::array<uint8_t, count> tx_buf = {0};

  /* Update Byte0 of txBuf with release power down instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::release_power_down);

  /* Invoke enableWrite to set the WEL bit to 1 */
  if (this->enable_write() != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Invoke memWrite to write data into the device */
  if (this->mem_write(tx_buf.data(), count) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  /* Provide a delay of 3microsec */
  static const uint8_t release_power_down_delay = 3;
  time_.delay_micros(release_power_down_delay);
  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::reset_device() {
  static const uint8_t length = 1;
  std::array<uint8_t, length> tx_buf = {0};
  MemoryStatus status;

  /* Invoke readStatusRegister1 to get the status of device */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Update the Byte0 of txBuf with  enable reset instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::reset_enable);

    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(tx_buf.data(), length) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Provide a delay of 30 microsec */
    static const uint32_t reset_delay = 30;
    time_.delay_micros(reset_delay);

    /* Update the Byte0 of txBuf with reset device instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::reset_device);

    /* Invoke memWrite to write data into the device */
    if (this->mem_write(tx_buf.data(), length) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::is_empty(uint32_t addr, uint16_t size, MemoryStatus &status) {
  uint8_t rx_buf[size] = {0};

  /* Invoke readByte to read data from the device */
  if (this->read_byte(addr, rx_buf, size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  for (uint16_t index = 0; index < size; index++) {
    if (rx_buf[index] != 0xFF) {
      status.not_empty = true;
    }
  }
  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::read_byte(uint32_t addr, uint8_t *data, uint16_t size) {
  // FIXME: We will need to use a statically-allocated vector instead of a C
  // array. Also, it is an error to try to initialize a variable-sized array -
  // that relies on a GCC extension - so the array is left uninitialized.
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  uint8_t tx_buf[size + 4];
  // FIXME: We will need to use a statically-allocated vector instead of a C
  // array. Also, it is an error to try to initialize a variable-sized array -
  // that relies on a GCC extension - so the array is left uninitialized.
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  uint8_t rx_buf[size + 4];

  /* Update the Byte0 of txBuf with read byte instruction */
  tx_buf[0] = static_cast<uint8_t>(SPIInstruction::read_byte);

  /* Fill the Byte1-Byte3 with address */
  for (uint8_t index = 1; index <= 3; index++) {
    tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
  }
  /* Invoke memRead to read data from the device */
  if (this->mem_read(tx_buf, rx_buf, size + 4) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Copy the read data into data buffer */
  for (uint16_t index = 4; index <= (size + 4); index++) {
    data[index - 4] = rx_buf[index];
  }
  /* Return ok */
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus SPIFlash::write_byte(uint32_t addr, const uint8_t *data_buf, uint8_t length) {
  // FIXME: We will need to use a statically-allocated vector instead of a C
  // array. Also, it is an error to try to initialize a variable-sized array -
  // that relies on a GCC extension - so the array is left uninitialized.
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  uint8_t tx_buf[length + 4];
  MemoryStatus status;

  /* Invoke readBusyStatus to read the busy bit status  */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke readMemoryStatus to get the status of block/sector */
    if (this->read_memory_status(addr, status) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::read_error;
    }

    if (status.lock) {
      /* If block is locked then invoke unLockIndividualMemory to unlock the block/sector */
      if (this->unlock_individual_memory(addr) != SPIDeviceStatus::ok) {
        return SPIDeviceStatus::write_error;
      }
    }

    /* Update the Byte0 of txBuf with write byte instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::write_byte);

    /* Fill the Byte1-Byte3 with address and remaining bytes with input which is to be written */
    for (uint8_t index = 1; index <= (length + 4); index++) {
      if (index < 4) {
        tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
      } else {
        tx_buf[index] = data_buf[index - 4];
      }
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(static_cast<uint8_t *>(tx_buf), length + 4) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Provide a delay of 3ms */
    static const uint8_t write_delay = 3;
    time_.delay(write_delay);
    /* Invoke lockIndividualMemory to lock the block/sector */
    if (this->lock_individual_memory(addr) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::write_page(uint32_t addr, const uint8_t *data_buf) {
  static const uint16_t length = page_size + 4;
  uint8_t tx_buf[length + 4];
  MemoryStatus status;

  /* Invoke readStatusRegister1 to get the status of device */
  if (this->read_busy_status(status) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  /* If the status of busy bit is false then SPI flash is ready for further instructions */
  if (!status.busy) {
    /* Invoke readMemoryStatus to get the status of block/sector */
    if (this->read_memory_status(addr, status) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::read_error;
    }

    if (status.lock) {
      /* If block is locked then invoke unLockIndividualMemory to unlock the block/sector */
      if (this->unlock_individual_memory(addr) != SPIDeviceStatus::ok) {
        return SPIDeviceStatus::write_error;
      }
    }

    /* Update the Byte0 of txBuf with write byte instruction */
    tx_buf[0] = static_cast<uint8_t>(SPIInstruction::write_byte);

    /* Fill the Byte1-Byte3 with address and remaining bytes with data which is to be written */
    for (uint16_t index = 1; index <= (page_size + 4); index++) {
      if (index < 4) {
        tx_buf[index] = addr >> (static_cast<uint8_t>(CHAR_BIT) * (3U - index));
      } else {
        tx_buf[index] = data_buf[index - 4];
      }
    }
    /* Invoke enableWrite to set the WEL bit to 1 */
    if (this->enable_write() != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Invoke memWrite to write data into the device */
    if (this->mem_write(static_cast<uint8_t *>(tx_buf), length) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Provide a delay of 3ms */
    static const uint8_t write_delay = 3;
    time_.delay(write_delay);
    /* Invoke lockIndividualMemory to lock the block/sector */
    if (this->lock_individual_memory(addr) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::write_error;
    }
    /* Return ok */
    return SPIDeviceStatus::ok;
  } else {
    /* Return busy */
    return SPIDeviceStatus::busy;
  }
}

SPIDeviceStatus SPIFlash::write_sector(uint32_t addr, const uint8_t *tx_buf, uint16_t length) {
  uint32_t page_loop_count = ((addr % page_size) + length);
  uint16_t num_of_pages = (page_loop_count / page_size);
  uint32_t index = 0;
  SPIDeviceStatus ret;
  /* Calculate the number of bytes to be written in a page */
  uint16_t current_page_count = page_size - (addr % page_size);

  /* Calculate the number of pages */
  page_loop_count = (page_loop_count % page_size) == 0 ? num_of_pages : (num_of_pages + 1);
  for (index = 0; index < page_loop_count; index++) {
    if (current_page_count < page_size) {
      /* Invoke writeByte to write the data */
      ret = this->write_byte(addr, tx_buf, current_page_count);
      /* Return ret if it is not ok */
      if (ret != SPIDeviceStatus::ok) {
        return ret;
      }
    }

    if (current_page_count == page_size) {
      /* Invoke writeByte to write the data into page */
      ret = this->write_page(addr, tx_buf);
      /* Return ret if it is not ok */
      if (ret != SPIDeviceStatus::ok) {
        return ret;
      }
    }
    addr += current_page_count;
    tx_buf += current_page_count;
    length = length - current_page_count;
    /* Calculate the number of bytes to be written in a page */
    current_page_count = (length < page_size) ? length : page_size;
  }
  /* Return SPIDeviceStatus */
  return ret;
}

SPIDeviceStatus SPIFlash::write_to_memory(uint32_t addr, const uint8_t *data_buf, uint16_t length) {
  uint16_t page_start_index = (addr % page_size);
  SPIDeviceStatus ret;
  MemoryStatus stat;

  /* Invoke isEmpty to check the page/sector/block is empty or not */
  if (this->is_empty(addr, length, stat) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }

  if (!stat.not_empty) {
    /* Length < 256 */
    if (length == page_size && page_start_index == 0) {
      /* Invoke writeByte to write the data into page */
      ret = this->write_page(addr, data_buf);
      return ret;
    }
    /* Length <= 256 */
    if ((length + page_start_index) <= page_size) {
      /* Invoke writeByte to write the data into address */
      ret = this->write_byte(addr, data_buf, length);
      return ret;
    }
    /* Length > 256 */
    /* Invoke writeSector to write the data in page by page */
    ret = this->write_sector(addr, data_buf, length);
    return ret;
  }
  /* Return write error on buffer is not empty */
  return SPIDeviceStatus::write_error;
}

}  // namespace Pufferfish::Driver::SPI
