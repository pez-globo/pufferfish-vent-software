/*
 * LMP91000.cpp
 *
 *  Created on: Aug 27, 2020
 *      Author: Laxmanrao R
 */


/**
 * Driver code for LMP91000
 */
#include "Pufferfish/Driver/I2C/LMP91000.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {

I2CDeviceStatus LMP91000::configDevice(uint8_t tiacnValue,uint8_t refcanValue, uint8_t modecnValue)
{
  I2CDeviceStatus ret = I2CDeviceStatus::ok;
  /* Set unlock mode */
  ret = write(lockRegAddr,1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  ret = write(ticanRegAddr, tiacnValue);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  ret = write(refcanRegAddr, refcanValue);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  ret = write(lockRegAddr,0);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  ret = write(modecnRegAddr,modecnValue);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  return ret;
}


I2CDeviceStatus LMP91000::write(uint8_t intWriteAddrReg, uint8_t txData)
{
  /* LMP91000 Write Operation
   * 1. Enable MENB
   * 2. Send 7bit + 1byte register address + data
   * 3. Disable MENB
   * */
  I2CDeviceStatus ret = I2CDeviceStatus::ok;

  mEnableModule.write(false);

  ret = mDev.write(&intWriteAddrReg,1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  ret = mDev.write(&txData,1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  mEnableModule.write(false);
  return ret;
}

I2CDeviceStatus LMP91000::read(uint8_t intReadAddrReg, uint8_t &rxData)
{
  /* LMP91000 Read Operation
   * 1. Enable MENB
   * 2. Send 7bit + 1byte register address + data
   * 3. Disable MENB
   * */
  I2CDeviceStatus ret = I2CDeviceStatus::ok;
  mEnableModule.write(false);

  ret = mDev.write(&intReadAddrReg,1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  ret = mDev.read(&rxData,1);
  if (ret != I2CDeviceStatus::ok) {
     return ret;
   }
  mEnableModule.write(false);
  return ret;
}

uint8_t LMP91000::readVoutMeasurements()
{
  static uint32_t adcSamples;
  static uint8_t o2Pct;
  mVout.read(adcSamples);
  /*FIXME: Required offset calculation to convert adc values to o2 concentration pct */
  o2Pct = adcSamples;
  return o2Pct;
}

I2CDeviceStatus LMP91000::statusOfDevice(uint8_t &status)
{
  I2CDeviceStatus ret = I2CDeviceStatus::ok;
  ret = mDev.read(&status,1);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  return ret;
}

I2CDeviceStatus LMP91000::test()
{
  I2CDeviceStatus ret;
  uint8_t status;
  uint8_t result;
  // check status of the device
  ret = this->statusOfDevice(status);
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }
  result = this->readVoutMeasurements();
  // FIXME: conditions
  if(result == 0) {
    return I2CDeviceStatus::testFailed;
  }
  return I2CDeviceStatus::ok;
}

I2CDeviceStatus LMP91000::reset()
{
  /*
   * FIXME: After it is verified that sensor working properly,
   * the LMP91000 needs to be reset to its original configuration
   */



    return I2CDeviceStatus::ok;
}
}
}
}

