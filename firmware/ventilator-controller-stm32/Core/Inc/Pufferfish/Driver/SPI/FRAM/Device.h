/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * FRAM
 *
 *      Author: Arjun Dhawan
 *
 * A driver for the CY15B256Q FRAM Chip
 */

#pragma once //used to prevent clashes in multiple inclusions

#include "Pufferfish/HAL/Interfaces/SPIDevice.h"

enum class Block {
  NONE       = 0b00000000,
  UPPER_1_4  = 0b00000100,
  UPPER_1_2  = 0b00001000,
  ALL        = 0b00001100
};

namespace Pufferfish::Driver::SPI::FRAM {
  class Device {
   public:
    /**
     * Writes to the FRAM chip
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus write(uint16_t addr, uint8_t *buffer, size_t buffer_len);

    /**
     * Reads from the FRAM chip
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus read(uint16_t addr, uint8_t *buffer, size_t buffer_len);

    /**
     * Protects a portion of the FRAM chip. Options are enumerated in
     * Pufferfish::Driver::SPI::FRAM::Block
     *
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus protect_block(bool protect, Block block);

    /**
     * Get the protect status of the FRAM chip
     *
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus protect_status(Block block);

   private:
    struct opcode {
      int8_t WREN  = 0b0000110;
      int8_t WRDI  = 0b00000100;
      int8_t RDSR  = 0b00000101;
      int8_t WRSR  = 0b00000001;
      int8_t READ  = 0b00000011;
      int8_t FSTRD = 0b00001011;
      int8_t WRITE = 0b00000010;
      int8_t SLEEP = 0b10111001;
      int8_t RDID  = 0b10011111;
    };
  };
}
