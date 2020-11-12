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
  auto op_wren = static_cast<uint8_t>(Opcode::rdsr);
  auto op_write = static_cast<uint8_t>(Opcode::rdsr);

  fram_spi_.chip_select(false);
  if (fram_spi_.write(&op_wren, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if (fram_spi_.write(&op_write, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }

  std::array<uint8_t, sizeof(uint16_t)> buf{{Util::get_byte<1>(addr), Util::get_byte<0>(addr)}};
  if (fram_spi_.write(buf.data(), buf.size()) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }

  if (fram_spi_.write(buffer, buffer_len) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }

  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::read(uint16_t addr, uint8_t *buffer, size_t buffer_len) {
  auto op_read = static_cast<uint8_t>(Opcode::rdsr);
  fram_spi_.chip_select(false);

  if (fram_spi_.write(&op_read, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  std::array<uint8_t, sizeof(uint16_t)> buf{{Util::get_byte<1>(addr), Util::get_byte<0>(addr)}};
  if (fram_spi_.write(buf.data(), buf.size()) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if (fram_spi_.read(buffer, buffer_len) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::read_error;
  }
  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::protect_block(bool /*protect*/, Block block) {
  auto op_rdsr = static_cast<uint8_t>(Opcode::rdsr);
  auto op_wren = static_cast<uint8_t>(Opcode::rdsr);
  auto op_wrsr = static_cast<uint8_t>(Opcode::rdsr);
  uint8_t rx_buf = 0;

  fram_protect_.write(true);
  fram_spi_.chip_select(false);

  if (fram_spi_.write(&op_rdsr, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }
  if (fram_spi_.read(&rx_buf, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::read_error;
  }
  // Creating Value for new Status Register while retaining WPEN
  // And-ing a mask of 0b11110011 and Or-ing in the Block
  static const uint8_t mask = 0b11110011;
  uint8_t status_reg_val = (rx_buf & mask) + static_cast<uint8_t>(block);

  if (fram_spi_.write(&op_wren, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }
  if (fram_spi_.write(&op_wrsr, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }
  if (fram_spi_.write(&status_reg_val, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    fram_protect_.write(false);
    return SPIDeviceStatus::write_error;
  }

  fram_spi_.chip_select(true);
  fram_protect_.write(false);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::protect_status(Block &block) {
  auto op_rdsr = static_cast<uint8_t>(Opcode::rdsr);
  fram_spi_.chip_select(false);
  uint8_t rx_buf = 0;

  if (fram_spi_.write(&op_rdsr, sizeof(uint8_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  if (fram_spi_.read(&rx_buf, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::read_error;
  }
  fram_spi_.chip_select(true);

  // Creating Value to compare block value to
  // And-ing a mask of 0b00001100
  static const uint8_t mask = 0b00001100;
  uint8_t protect_status = rx_buf & mask;
  switch (protect_status) {
    case static_cast<uint8_t>(Block::none):
      block = Block::none;
      break;
    case static_cast<uint8_t>(Block::upper_1_4):
      block = Block::upper_1_4;
      break;
    case static_cast<uint8_t>(Block::upper_1_2):
      block = Block::upper_1_2;
      break;
    case static_cast<uint8_t>(Block::all):
      block = Block::all;
      break;
    default:
      block = Block::all;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus Device::sleep_mode() {
  auto op_sleep = static_cast<uint8_t>(Opcode::sleep);
  fram_spi_.chip_select(false);

  if (fram_spi_.write(&op_sleep, sizeof(size_t)) != SPIDeviceStatus::ok) {
    fram_spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  fram_spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

}  // namespace Pufferfish::Driver::SPI::FRAM
