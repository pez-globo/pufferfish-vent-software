/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Laxmanrao R
 */

#include "Pufferfish/Driver/Power/PowerManager.h"

namespace Pufferfish {
namespace Driver {
namespace Power {

/*
 *  FIXME: Required to optimize code
 */
PwrSource LTC4421::readPwrSource(void)
{
  if(( mLTC4421Input.regCh1.read() != true) && ( mExtPwrFault == false )  )
  {
    if(mLTC4421Input.regFault1.read() != true ){
      mLTC4421Input.regDisable1.write(false);
      mExtPwrFault = true;
      /*
       * FIXME: Required timer for cool down the cycle
       */
      //AVINTBD
      return PwrSource::external12vPwr;
    }
    else
    {
      if(mLTC4421Input.regValid1.read() != true)
      {
        mLTC4421Input.regPgood3V3.write(true);
        mLTC4421Input.regPgood5V0.write(true);
        mExtPwr = true;
        mIntPwr = false;
        mExtPwrFault = false;
        return PwrSource::external12vPwr;
      }
      else
      {
        mLTC4421Input.regPgood3V3.write(false);
        mLTC4421Input.regPgood5V0.write(false);
        mExtPgoodFault = true;
        //AVINTBD
        return PwrSource::noPwr;
      }
    }
  }
  else if( (mLTC4421Input.regCh2.read() != true) && ( mIntPwrFault == false )  )
  {
    if(mLTC4421Input.regFault2.read() != true )
    {
      mLTC4421Input.regDisable2.write(false);
      mIntPwrFault = true;
      /*
      * FIXME: Required timer for cool down the cycle
      */
      //AVINTBD
      return PwrSource::internal12Vpwr;
     }
    else
    {
       if(mLTC4421Input.regValid2.read() != true)
       {
         mLTC4421Input.regPgood3V3.write(true);
         mLTC4421Input.regPgood5V0.write(true);
         mIntPwr = true;
         mIntPgoodFault = false;
         return PwrSource::internal12Vpwr;
       }
       else
       {
         mLTC4421Input.regPgood3V3.write(false);
         mLTC4421Input.regPgood5V0.write(false);
         mIntPgoodFault = true;
         //AVINTBD
         return PwrSource::noPwr;
       }
     }
   }
  else
  {
    return PwrSource::noPwr;
  }
}

/*
 * FIXME: Is it required to report multiple faults
 */
PwrFaults LTC4421::fault()
{
  if( mExtPwrFault == true ){
    return PwrFaults::external12vPwrfault;
  }else if( mIntPwrFault == true ){
    return PwrFaults::internal12Vpwrfault;
  }else if( mExtPgoodFault == true ){
    return PwrFaults::extPgoodFault;
  }else if (mIntPgoodFault == true ){
    return PwrFaults::intPgoodFault;
  }
  else{
    return PwrFaults::noFault;
  }
}

BatStatus Battery::readVoltage()
{
  static const uint16_t adcSamples = 0;
  static const float offset = 13.80f;
  mRegEnMeas.write(true);

  mSamples.read(VolageRawValue);

  if( VolageRawValue != adcSamples )
  {
    batteryVoltage = static_cast<float>( ( offset * VolageRawValue ) / 65536);
    return BatStatus::detected;
  }
  else
  {
    return BatStatus::notDetected;
  }
}

void Battery::enBatteryCharge(bool &status)
{
  if(status == true){
    mRegEnCharge.write(true);
  }else{
    mRegEnCharge.write(false);
  }
}

float Battery::capacity ()
{
  /* Calculate the battery voltage capacity */
  batteryCapicity = (batteryVoltage - batteryDeepDischargeVoltage) / (batteryTotalCapacityVoltage - batteryDeepDischargeVoltage);

  /* Convert the batteryCapicity to percentage in capacity */
  batteryCapicity = (batteryCapicity * 60) + 40;

  return batteryCapicity;
}

BatSOCLevels Battery::socLevels()
{
  /// TBD: Need to identify levels based on battery soc levels
  static const float Drained = 50.0f;
  static const float lowerLimit = 60.0f;
  static const float midRange = 80.0f;
  static const float HighLimit = 100.0f;

  if (batteryCapicity == Drained){
    return BatSOCLevels::Drained;
  }else if (batteryCapicity < lowerLimit){
    return BatSOCLevels::lowerLimit;
  }else if ((batteryCapicity >= lowerLimit) && (batteryCapicity < midRange)){
    return BatSOCLevels::midRange;
  }else if ((batteryCapicity >= midRange) && (batteryCapicity < HighLimit))
  {
    return BatSOCLevels::HighLimit;
  }else{
    return BatSOCLevels::MaxCharge;
  }
}


BatChargeStatus Battery::status()
{
  if(batteryVoltage == batteryTotalCapacityVoltage)
  {
    BatteryChargeStatus = BatChargeStatus::fullCharged;
  }else if(batteryVoltage == batteryDeepDischargeVoltage)
  {
    BatteryChargeStatus = BatChargeStatus::fullDischarged;
  }else if (lastBatteryVoltage < batteryVoltage)
  {
    BatteryChargeStatus = BatChargeStatus::charging;
  }else if (lastBatteryVoltage > batteryVoltage)
  {
   BatteryChargeStatus = BatChargeStatus::discharging;
  }

  return BatteryChargeStatus;
}

}  // namespace Power
}  // namespace Driver
}  // namespace Pufferfish

