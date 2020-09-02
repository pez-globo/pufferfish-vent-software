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
 * Driver code for LMP91000
 */

class LMP91000: public Testable
{
public:

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

  I2CDeviceStatus configDevice(uint8_t tiacnValue,uint8_t refcanValue, uint8_t modecnValue);
  I2CDeviceStatus statusOfDevice(uint8_t &status);
  I2CDeviceStatus write(uint8_t intWriteAddrReg, uint8_t txData);
  I2CDeviceStatus read(uint8_t intReadAddrReg, uint8_t &rxData);
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
