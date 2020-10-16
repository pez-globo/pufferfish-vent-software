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

SPIDeviceStatus Device::write(uint16_t addr, uint8_t *buffer, size_t buffer_len) {
  //CS Low
  //WREN
  //Write Address
  //Write Buffer
}

SPIDeviceStatus Device::read(uint16_t addr, uint8_t *buffer, size_t buffer_len){
  //CS Low
  //READ
  //Read Address
}

SPIDeviceStatus Device::protect_block(bool protect, Block block){
  //CS Low
  //WREN
  //WRSR
  //Write Block
}

SPIDeviceStatus Device::protect_status(Block block){
  //CS Low
  //WREN
  //RDSR
  //Read Block
}


}
}
