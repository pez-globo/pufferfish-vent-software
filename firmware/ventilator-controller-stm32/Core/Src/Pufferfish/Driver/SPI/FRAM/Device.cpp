/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * FRAM
 *
 *      Author: Arjun Dhawan
 *
 * A driver for the CY15B256Q FRAM Chip
 */

#include "Pufferfish/Driver/SPI/FRAM/Device.h"

namespace Pufferfish::Driver::SPI::FRAM {
  // FRAM

SPIDeviceStatus Device::write(uint8_t *addr, uint8_t *buffer, size_t buffer_len) {
  opcode op;
  fram_spi_.chip_select(false);
  size_t count_1 = 1;
  size_t count_2 = 2;
  if(fram_spi_.write(op.WREN, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.write(op.WRITE, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }

  if(fram_spi_.write(addr, count_2) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }

  if(fram_spi_.write(buffer, buffer_len) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }

  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::read(uint8_t *addr, uint8_t *buffer, size_t buffer_len){
  opcode op;
  size_t count_1 = 1;
  size_t count_2 = 2;
  fram_spi_.chip_select(false);

  if(fram_spi_.write(op.READ, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.write(addr, count_2) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.read(buffer, buffer_len) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::read_error;
  }
  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::protect_block(bool protect, Block block){
  //WRITE PROTECT PIN NEEDS TO BE HIGH - ASK ETHAN
  opcode op;
  size_t count_1 = 1;
  size_t count_2 = 2;
  uint8_t *rx_buf;
  fram_spi_.chip_select(false);
  if(fram_spi_.write(op.RDSR, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.read(rx_buf, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::read_error;
  }
  // Creating Value for new Status Register while retaining
  // WPEN
  //And-ing a mask of 0b11110011 and Or-ing in the Block
  uint8_t *status_reg_val = (rx_buf & 0xF3) | block;

  if(fram_spi_.write(op.WREN, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.write(op.WRSR, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.write(status_reg_val, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::protect_status(Block &block){
  opcode op;
  Block bl;
  fram_spi_.chip_select(false);
  size_t count_1 = 1;
  uint8_t *rx_buf;

  if(fram_spi_.write(op.RDSR, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.read(rx_buf, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::read_error;
  }
  fram_spi_.chip_select(true);

  // Creating Value to compare block value to
  //And-ing a mask of 0b00001100
  uint8_t protect_status = rx_buf & 0xC;
  switch(protect_status){
    case Block::NONE:
      block = Block::NONE;
      break;
    case Block::UPPER_1_4:
      block = Block::UPPER_1_4;
      break;
    case Block::UPPER_1_2:
      block = Block::UPPER_1_2;
      break;
    case Block::ALL:
      block = Block::ALL;
      break;
    default:
      block = Block::ALL;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::sleep_mode(){
  opcode op;
  size_t count_1 = 1;
  fram_spi_.chip_select(false);

  if(fram_spi_.write(op.SLEEP, count_1) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

}

