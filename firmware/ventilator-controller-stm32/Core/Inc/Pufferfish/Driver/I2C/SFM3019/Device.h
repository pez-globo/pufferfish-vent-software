/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * SFM3000.h
 *
 *      Author: March Boonyapaluk
 *
 *  A driver for the SFM3000 Low Pressure Drop Digital Flow Meter
 */

#pragma once

#include <climits>

#include "Pufferfish/Driver/I2C/SensirionDevice.h"
#include "Pufferfish/Driver/Testable.h"
#include "Pufferfish/HAL/CRCChecker.h"
#include "Pufferfish/HAL/Interfaces/I2CDevice.h"
#include "Types.h"

namespace Pufferfish::Driver::I2C::SFM3019 {

static const uint16_t default_i2c_addr = 0x2e;

/**
 * Stateless low-level driver for Sensirion SFM3019 flow sensor device
 */
class Device {
 public:
  explicit Device(HAL::I2CDevice &dev, HAL::I2CDevice &global_dev, GasType gas)
      : crc8_(crc_params), sensirion_(dev, crc8_), global_(global_dev, crc8_), gas(gas) {}

  /**
   * Starts a flow measurement
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus start_measure();

  /**
   * Stops the flow measurement
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus stop_measure();

  /**
   * Configure measurement averaging
   * @param averaging_window a value up to 128 for the size of the averaging window.
   * 0 indicates average-until-read mode.
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus set_averaging(uint8_t averaging_window);

  /**
   * Requests the conversion factors
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus request_conversion_factors();

  /**
   * Reads out the conversion factors. They only become available ~20 us after the request command.
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus read_conversion_factors(ConversionFactors &conversion);

  /**
   * Reads out the product id
   * @param sn[out] the unique serial number
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus read_product_id(uint32_t &product_number);

  /**
   * Reads out the flow rate from the sensor
   * @param sample[out] the sensor reading; only valid on success
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus read_sample(Sample &sample, int16_t scale_factor, int16_t offset);

  /**
   * Causes a global I2C device reset
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus reset();

 private:
  static constexpr HAL::CRC8Parameters crc_params = {0x31, 0xff, false, false, 0x00};

  HAL::SoftCRC8 crc8_;
  SensirionDevice sensirion_;
  SensirionDevice global_;
  const GasType gas;
};

}  // namespace Pufferfish::Driver::I2C::SFM3019
