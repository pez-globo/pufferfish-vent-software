/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Laxmanrao R
 */

#include "Pufferfish/Driver/Power/PowerManager.h"

namespace Pufferfish {
namespace Driver {
namespace Power {


PowerAvialability LTC4421::checkInputPower()
{
    PowerAvialability powerSource;
    if(mRegCh1.read() != true ){
      powerSource.wall_outlet_power = true;
    }else{
      powerSource.wall_outlet_power = false;
    }
    if(mRegCh2.read() != true ){
      powerSource.internal_battery_power = true;
    }else{
      powerSource.internal_battery_power = false;
    }
return powerSource;
}

PowerFaultStatus  LTC4421::checkInputPowerFaults()
{
  PowerAvialability inputPowerSource;
  inputPowerSource = this->checkInputPower();
  if( (inputPowerSource.wall_outlet_power != true)
        && (inputPowerSource.internal_battery_power != true) ){
    currentFaultStatus.BatteryDisableFault = true;
  }else
  {
    currentFaultStatus.BatteryDisableFault = false;
    if( mRegFault1.read() != true){
      currentFaultStatus.externalPowerFault = true;
    }else{
      currentFaultStatus.externalPowerFault = false;
    }
    if( mRegFault2.read() != true){
      currentFaultStatus.internalPowerFault = true;
     }else{
       currentFaultStatus.internalPowerFault = false;
     }
  }
  return currentFaultStatus;
}

PowerSource  LTC4421::validateInputPower()
{
  /* FIXME: Fault handling need to be done */
  //currentFaultStatus = this->checkInputPowerFaults;
  /*Check any fault on input power source */
  if(mRegValid1.read() != true){
    return PowerSource::external12vPower;
  }else if(mRegValid2.read() != true){
    return PowerSource::internal12VPower;
  }else{
    return PowerSource::noPower;
  }
}

PowerFaultStatus LTC4421::enableOutputPower()
{
  PowerSource ret = this->validateInputPower();
  if(ret != PowerSource::external12vPower){
    mRegPgood3V3.write(true);
    mRegPgood5V0.write(true);
    currentFaultStatus.externalPGoodFault = false;
  }else{
    mRegPgood3V3.write(false);
     mRegPgood5V0.write(false);
    currentFaultStatus.externalPGoodFault = true;
  }
  if(ret!= PowerSource::internal12VPower){
    mRegPgood3V3.write(true);
    mRegPgood5V0.write(true);
    currentFaultStatus.internalPGoodFault = false;
  }else{
    mRegPgood3V3.write(false);
    mRegPgood5V0.write(false);
    currentFaultStatus.internalPGoodFault = true;
  }
  return currentFaultStatus;
}

BatStatus Battery::readBatteryStatus()
{
  float batteryVoltage;
  static const uint16_t adcSamples = 0;
  batteryVoltage = this->readVoltage();
  if( batteryVoltage != adcSamples ){
    return BatStatus::detected;
  }else{
    return BatStatus::notDetected;
  }
}

float Battery::readVoltage()
{
  float batteryVoltage;
  static const float offset = 13.80f;

  uint32_t VolageRawValue = 0;
  mSamples.read(VolageRawValue);

  batteryVoltage = static_cast<float>( ( offset * VolageRawValue ) / 65536);

  return batteryVoltage;
}

void Battery::enableBatteryCharge(bool &status)
{
  if(status == true){
    mRegEnCharge.write(true);
  }else{
    mRegEnCharge.write(false);
  }
}

float Battery::readCapacity ()
{
  static float batteryCapicity;
  static float batteryVoltage = this->readVoltage();

  /* Calculate the battery voltage capacity */
  batteryCapicity = (batteryVoltage - batteryDeepDischargeVoltage) / (batteryTotalCapacityVoltage - batteryDeepDischargeVoltage);

  /* Convert the batteryCapicity to percentage in capacity */
  batteryCapicity = (batteryCapicity * 60) + 40;

  return batteryCapicity;
}

BatSOCLevels Battery::readSOClevels()
{
  /// TBD: Need to identify levels based on battery soc levels
  static const float Drained = 50.0f;
  static const float lowerLimit = 60.0f;
  static const float midRange = 80.0f;
  static const float HighLimit = 100.0f;

  static float batterySocPercentage = this->readCapacity();

  if (batterySocPercentage == Drained){
    return BatSOCLevels::Drained;
  }else if (batterySocPercentage < lowerLimit){
    return BatSOCLevels::lowerLimit;
  }else if ((batterySocPercentage >= lowerLimit) && (batterySocPercentage < midRange)){
    return BatSOCLevels::midRange;
  }else if ((batterySocPercentage >= midRange) && (batterySocPercentage < HighLimit))
  {
    return BatSOCLevels::HighLimit;
  }else{
    return BatSOCLevels::MaxCharge;
  }
}

}  // namespace Power
}  // namespace Driver
}  // namespace Pufferfish

