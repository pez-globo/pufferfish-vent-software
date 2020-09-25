/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/SPIDevice.h"

namespace Pufferfish {
namespace HAL {

/**
 * An abstarct class for SPI device
 */
class MockSPIDevice : public SPIDevice {
 public:
  // maximum default time to wait for response from SPI, in ms
  const static uint32_t DefaultTimeout = 2u;

  /**
   * Constructs an Mock SPI object
   */
  MockSPIDevice() {
  }

  /**
   * @brief  Read method to read data from private buffer variable mReadBuf
   * @param  buf returns the data stored in setRead method
   * @param  count size of data to set
   * @return returns SPIDeviceStatus as ok
   */
  SPIDeviceStatus read(uint8_t *buf, size_t count) override;

  /**
   * @brief  Updates the private buffer variable mReadBuf with the input data
   * @param  buf update the private variable mReadBuf with the buffer input
   * @param  count size of data to set
   * @return None
   */
  void setRead(uint8_t *buf, size_t count);

  /**
   * @brief  Writes data to the device
   * @param buf the data to be written
   * @param count the number of bytes to write
   * @return returns SPIDeviceStatus as ok
   */
  SPIDeviceStatus write(uint8_t *buf, size_t count) override;

  /**
   * @brief  Reads private buffer variable mWriteBuf and updates to buf
   * @param  buf returns the data stored in write method
   * @param  count buffer size to return
   * @return None
   */
  void getWrite(uint8_t *buf, size_t &count);

  /**
   * @brief  Updates the private buffer variable mSetWriteReadBuf with the input data
   * @param  buf update the private variable mSetWriteReadBuf with the buffer input
   * @param  count size of data to set
   * @return None
   */
  void setWriteRead(uint8_t *buf, size_t count);

  /**
   * @brief  Write and Read data to and from the device
   * @param  txBuf pointer to transmission data buffer
   * @param  rxBuf pointer to reception data buffer
   * @param  count amount of data to be sent and received
   * @return returns SPIDeviceStatus as ok
   */
  SPIDeviceStatus writeRead(uint8_t *txBuf, uint8_t *rxBuf, size_t count) override;

  /**
   * @brief  Reads private buffer variable mGetWriteReadBuf and updates to buf
   * @param  buf returns the data stored in writeRead method
   * @param  count buffer size to return
   * @return None
   */
  void getWriteRead(uint8_t *buf, size_t count);

  /**
   * @brief  Sets the mLastCS
   * @param  true(high) or false(low)
   * @return None
   */
  void chipSelect (bool input) override;

  /**
   * @brief  Gets the last chip select input data
   * @param  None
   * @return returns mLastCS input data
   */
  bool getChipSelect ();

 private:
  /* Last chip select input */
  bool mLastCS = false;

  /* Read and Write buffer size */
  static const uint8_t mReadBufSize = 50, mWriteBufSize = 50;

  /* The 50 bytes of read and write operation are used */
  uint8_t mReadBuf[mReadBufSize], mWriteBuf[mWriteBufSize];
  /* The 50 bytes of writeRead operation are used */
  uint8_t mSetWriteReadBuf[mReadBufSize], mGetWriteReadBuf[mWriteBufSize];
  /* Write data count */
  size_t mWriteCount, mGetWriteReadCount;

};

}  // namespace HAL
}  // namespace Pufferfish
