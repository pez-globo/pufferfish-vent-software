/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Laxmanrao R
 */
#pragma once

#include "Pufferfish/HAL/HAL.h"
#include "Pufferfish/Types.h"
#include "Pufferfish/Driver/Testable.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {

/**
 * Abstract class for LMP91000
 */

class LMP91000: public Testable
{
public:

  /**
   * Protection status resister for lock or unlock configuration settings
   */
  enum class ProtectionReg{
    lock = 0,
    unlock
  };

  /* LMP91000 Address 1001 000 */
  static const uint16_t defaultI2CAddr = 0x48;
  static const uint8_t statusRegAddr = 0x00;
  static const uint8_t lockRegAddr = 0x01;
  static const uint8_t ticanRegAddr = 0x10;
  static const uint8_t refcanRegAddr = 0x11;
  static const uint8_t modecnRegAddr = 0x12;

  LMP91000(HAL::I2CDevice &dev, HAL::DigitalOutput &enableModule,
           HAL::AnalogInput &vOut)
           :
           mDev(dev),
           mEnableModule(enableModule),
           mVout(vOut){
}
  /**
   * Configuration parameters for LMP91000
   * @Param tiacnValue tia control register values
   * @Param refcanValue Voltage reference value
   * @Param modecnValue Mode control value
   * @return I2CDeviceStatus success or error codes
   */
  I2CDeviceStatus configDevice(uint8_t tiacnValue,uint8_t refcanValue, uint8_t modecnValue);

  /**
   * Read status of the device
   * @Param status Device read or not ready to accept the commands
   * @return I2CDeviceStatus success or error codes
   */
  I2CDeviceStatus statusOfDevice(uint8_t &status);

  /**
    * Write data for setting configuration parameters
    * @Param intWriteAddrReg Write register address
    * @Param txData Set data to the registers
    * @return I2CDeviceStatus success or error codes
    */
  I2CDeviceStatus write(uint8_t intWriteAddrReg, uint8_t txData);

  /**
     * Read raw data from the sensor
     * @Param intWriteAddrReg Read register address
     * @Param rxData Read sensor data
     * @return I2CDeviceStatus success or error codes
     */
  I2CDeviceStatus read(uint8_t intReadAddrReg, uint8_t &rxData);
  /**
   * Convert current (mA) to O2 concentration
   * @return uint8_t O2 concentration pct
   */
  uint8_t readVoutMeasurements();

  I2CDeviceStatus test() override;
  I2CDeviceStatus reset() override;
private:
  HAL::I2CDevice &mDev;
  HAL::DigitalOutput &mEnableModule;
  HAL::AnalogInput &mVout;

};

}  // namespace I2C
}  // namespace Driver
}  // namespace Pufferfish
