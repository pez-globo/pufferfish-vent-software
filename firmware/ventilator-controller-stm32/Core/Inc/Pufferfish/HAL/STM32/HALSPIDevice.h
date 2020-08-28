/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/SPIDevice.h"

#include "Pufferfish/HAL/STM32/DigitalOutput.h"

namespace Pufferfish {
namespace HAL {

/**
 * An HAL SPI device
 */
class HALSPIDevice : public SPIDevice {
 public:
  // maximum default time to wait for response from SPI, in ms
  const static uint32_t DefaultTimeout = 2u;

  /**
   * Constructs an HAL SPI object
   * @param hspi    STM32 HAL handler for the SPI port
   * @param cspin   Chip select pin for SPI 
   */
  HALSPIDevice(SPI_HandleTypeDef &hspi,  DigitalOutput &cspin)
      :
      mDev(hspi), mCsPin(cspin){
  }

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
  SPIDeviceStatus writeRead(uint8_t *txBuf, uint8_t *rxBuf, size_t count) override;

  /**
     * To make the chip select as high or low
     * @param cs true(high) or false(low)
     */
  void chipSelect (bool input);

 protected:
  SPI_HandleTypeDef &mDev;

 private:
  DigitalOutput &mCsPin;

};

}  // namespace HAL
}  // namespace Pufferfish
