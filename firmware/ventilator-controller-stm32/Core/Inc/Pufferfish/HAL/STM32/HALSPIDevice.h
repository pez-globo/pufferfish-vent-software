/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/DigitalOutput.h"
#include "Pufferfish/HAL/Interfaces/SPIDevice.h"
#include "stm32h7xx_hal.h"

namespace Pufferfish {
namespace HAL {

/**
 * An HAL SPI device
 */
class HALSPIDevice : public SPIDevice {
 public:
  // maximum default time to wait for response from SPI, in ms
  const static uint32_t default_timeout = 2U;

  /**
   * Constructs an HAL SPI object
   * @param hspi    STM32 HAL handler for the SPI port
   * @param cspin   Chip select pin for SPI
   */
  HALSPIDevice(SPI_HandleTypeDef &hspi, DigitalOutput &cspin) : dev_(hspi), cs_pin_(cspin) {}

  /**
   * Reads data from the device
   * @param buf[out]    output of the data
   * @param count   the number of bytes to be read
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus read(uint8_t *buf, size_t count) override;

  /**
   * Writes data to the device
   * @param buf the data to be written
   * @param count the number of bytes to write
   * @return ok on success, error code otherwise
   */
  SPIDeviceStatus write(uint8_t *buf, size_t count) override;

  /**
   * Write and Read data to and from the device
   * @param txBuf pointer to transmission data buffer
   * @param rxBuf pointer to reception data buffer
   * @param count amount of data to be sent and received
   * @return ok on success
   */
  SPIDeviceStatus write_read(uint8_t *tx_buf, uint8_t *rx_buf, size_t count) override;

  /**
   * To make the chip select as high or low
   * @param cs true(high) or false(low)
   */
  void chip_select(bool input) override;

 private:
  SPI_HandleTypeDef &dev_;
  DigitalOutput &cs_pin_;
};

}  // namespace HAL
}  // namespace Pufferfish
