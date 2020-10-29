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
    explicit Device(HAL::SPIDevice &spi) : fram_spi_(spi) {}
    /**
     * Writes to the FRAM chip
     * Order of Operations: CS Low, Write WREN Opcode, Write WRITE Opcode,
     *                      Write Address, Write Buffer, CS High
     *
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus write(uint8_t *addr, uint8_t *buffer, size_t buffer_len);

    /**
     * Reads from the FRAM chip
     * Order of Operations: CS Low, Write READ Opcode, Write read address,
     *                      Read buffer for buffer_len
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus read(uint8_t *addr, uint8_t *buffer, size_t buffer_len);

    /**
     * Protects a portion of the FRAM chip. Options are enumerated in
     * Pufferfish::Driver::SPI::FRAM::Block
     * Order of Operations: CS_Low, Read RDSR to get Status Reg value,
     * Mask in block value, Write WREN, Write WRSR to enable writing
     * to Status Reg, Write new status register value
     *
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus protect_block(bool protect, Block block);

    /**
     * Get the protect status of the FRAM chip
     *
     * Order of Operations: CS_Low, Write RDSR, Read in to a buffer,
     *                      Determine Protect Status
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus protect_status(Block &block);

    /**
     * Put the FRAM chip in sleep mode
     * Device automatically goes out of sleep
     * mode when the CS Pin goes low again.
     *
     * @return ok on success, error code otherwise
     */
    SPIDeviceStatus sleep_mode();

   private:
    HAL::SPIDevice &fram_spi_;
    struct opcode {
      uint8_t *WREN  = 0b00000110;
      uint8_t *WRDI  = 0b00000100;
      uint8_t *RDSR  = 0b00000101;
      uint8_t *WRSR  = 0b00000001;
      uint8_t *READ  = 0b00000011;
      uint8_t *FSTRD = 0b00001011;
      uint8_t *WRITE = 0b00000010;
      uint8_t *SLEEP = 0b10111001;
      uint8_t *RDID  = 0b10011111;
    };
  };
}
