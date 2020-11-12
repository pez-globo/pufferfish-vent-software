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
#include "Pufferfish/Util/Bytes.h"

namespace Pufferfish::Driver::SPI::FRAM {
  // FRAM

SPIDeviceStatus Device::write(uint16_t addr, uint8_t *buffer, size_t buffer_len) {
  uint8_t op_wren = static_cast<uint8_t>(opcode::RDSR);
  uint8_t op_write = static_cast<uint8_t>(opcode::RDSR);

  fram_spi_.chip_select(false);
  if(fram_spi_.write(&op_wren, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.write(&op_write, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }

  std::array<uint8_t, sizeof(uint16_t)> buf{
    {Util::get_byte<1>(addr), Util::get_byte<0>(addr)}};
  if(fram_spi_.write(buf.data(), buf.size()) != SPIDeviceStatus::ok){
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

SPIDeviceStatus Device::read(uint16_t addr, uint8_t *buffer, size_t buffer_len){
  uint8_t op_read = static_cast<uint8_t>(opcode::RDSR);
  fram_spi_.chip_select(false);

  if(fram_spi_.write(&op_read, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  std::array<uint8_t, sizeof(uint16_t)> buf{
      {Util::get_byte<1>(addr), Util::get_byte<0>(addr)}};
  if(fram_spi_.write(buf.data(), buf.size()) != SPIDeviceStatus::ok){
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
  uint8_t op_rdsr = static_cast<uint8_t>(opcode::RDSR);
  uint8_t op_wren = static_cast<uint8_t>(opcode::RDSR);
  uint8_t op_wrsr = static_cast<uint8_t>(opcode::RDSR);
  uint8_t rx_buf;

  fram_protect_.write(true);
  fram_spi_.chip_select(false);

  if(fram_spi_.write(&op_rdsr, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.read(&rx_buf, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::read_error;
  }
  // Creating Value for new Status Register while retaining WPEN
  //And-ing a mask of 0b11110011 and Or-ing in the Block
  uint8_t status_reg_val = (rx_buf & 0xF3) | static_cast<uint8_t>(block);

  if(fram_spi_.write(&op_wren, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.write(& op_wrsr, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.write(&status_reg_val, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }

  fram_spi_.chip_select(true);
  fram_protect_.write(false);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::protect_status(Block &block){
  uint8_t op_rdsr = static_cast<uint8_t>(opcode::RDSR);
  fram_spi_.chip_select(false);
  uint8_t rx_buf;

  if(fram_spi_.write(&op_rdsr, sizeof(uint8_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if(fram_spi_.read(&rx_buf, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::read_error;
  }
  fram_spi_.chip_select(true);

  // Creating Value to compare block value to
  //And-ing a mask of 0b00001100
  uint8_t protect_status = rx_buf & 0xC;
  switch(protect_status){
    case static_cast<uint8_t>(Block::NONE):
      block = Block::NONE;
      break;
    case static_cast<uint8_t>(Block::UPPER_1_4):
      block = Block::UPPER_1_4;
      break;
    case static_cast<uint8_t>(Block::UPPER_1_2):
      block = Block::UPPER_1_2;
      break;
    case static_cast<uint8_t>(Block::ALL):
      block = Block::ALL;
      break;
    default:
      block = Block::ALL;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::sleep_mode(){
  uint8_t op_sleep = static_cast<uint8_t>(opcode::SLEEP);
  fram_spi_.chip_select(false);

  if(fram_spi_.write(&op_sleep, sizeof(size_t)) != SPIDeviceStatus::ok){
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

}

