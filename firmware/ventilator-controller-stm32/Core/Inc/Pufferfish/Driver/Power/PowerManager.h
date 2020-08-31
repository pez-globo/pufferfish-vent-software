/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Laxmanrao R
 */

#pragma once

#include "Pufferfish/Statuses.h"
#include "Pufferfish/Types.h"
#include "Pufferfish/HAL/HAL.h"
namespace Pufferfish {
namespace Driver {
namespace Power {


///struct Profile
///{
///  float batteryVoltage;
///  float batteryTemperature;
///  float socMaximum;
///  float socMedium;
///  float socLow;
///  float socCritical;
///};



struct PowerFaultStatus{
  bool BatteryDisableFault;
  bool externalPowerFault;
  bool internalPowerFault;
  bool externalPGoodFault;
  bool internalPGoodFault;
};

struct PowerAvialability {
  bool wall_outlet_power;
  bool internal_battery_power;
};

/**
 * An abstract class for Power Path control module
 */
class LTC4421 {
 public:

  static PowerFaultStatus currentFaultStatus;
  /**
   * @brief  Constructs a new LTC4421 object
   * @param  LTC4421Input - Takes LTC4421 Pin input
   */
  LTC4421( HAL::DigitalInput  &regCh1,      HAL::DigitalInput  &regCh2,
           HAL::DigitalInput  &regValid1,   HAL::DigitalInput  &regValid2,
           HAL::DigitalInput  &regFault1,   HAL::DigitalInput  &regFault2,
           HAL::DigitalOutput &regDisable1, HAL::DigitalOutput &regDisable2,
           HAL::DigitalOutput &regPgood3V3, HAL::DigitalOutput &regPgood5V0
           )
          :
          mRegCh1(regCh1),
          mRegCh2(regCh2),
          mRegValid1(regValid1),
          mRegValid2(regValid2),
          mRegFault1(regFault1),
          mRegFault2(regFault2),
          mRegDisable1(regDisable1),
          mRegDisable2(regDisable2),
          mRegPgood3V3(regPgood3V3),
          mRegPgood5V0(regPgood5V0)
          {
  }
  /**
   * Check input power source on ch1 && ch2
   * @return is it internal power source or external
   */
  PowerAvialability checkInputPower();
  /**
   * Check faults for input power source on ch1 && ch2
   * @return is it internal power or external power faults when
   */
  PowerFaultStatus  checkInputPowerFaults();
  /**
   * Validate input power source
   * @return is it internal power source or external source
   */
  PowerSource  validateInputPower();
  /**
   * Enable output power 3V3 & 5V0 PGood signals
   * @return is it internal power source or external source
   */
  PowerFaultStatus enableOutputPower();

  private:

  /* Private variable for LTC4421 input pins */
  HAL::DigitalInput  &mRegCh1;HAL::DigitalInput  &mRegCh2;HAL::DigitalInput  &mRegValid1;
  HAL::DigitalInput  &mRegValid2;HAL::DigitalInput  &mRegFault1;HAL::DigitalInput  &mRegFault2;
  HAL::DigitalOutput &mRegDisable1;HAL::DigitalOutput &mRegDisable2;HAL::DigitalOutput &mRegPgood3V3;
  HAL::DigitalOutput &mRegPgood5V0;
  PowerFaultStatus FaultStatus;
};

/* FIXME: Move to separate source file */
/**
 * An abstract class for Battery Charge control module
 */
class Battery {

 public:
    static constexpr float batteryTotalCapacityVoltage = 13.8f;
    static constexpr float batteryVoltageUpperLimit1   = 12.0f;
    static constexpr float batteryVoltageLowerLimit1   = 10.0f;
    static constexpr float batteryDeepDischargeVoltage = 9.0f;

    /**
      * @brief  Constructs a new Battery object
      * @param  adcValus -
      *         regEnMeas -
      *         regEnCharge -
      */
    Battery( Pufferfish::HAL::AnalogInput &adcValus,HAL::DigitalOutput &regEnMeas,HAL::DigitalOutput &regEnCharge )
             : mSamples(adcValus),mRegEnMeas(regEnMeas),mRegEnCharge(regEnCharge){

     }

     /**
       * @brief  Reads battery voltage
       * @param  batteryVoltage
       * @return returns battery status
       */
     BatStatus readBatteryStatus();

     float readVoltage();

     /**
       * @brief  enables the charge of battery based on status
       * @param  status -
       * @return None
       */
     void enableBatteryCharge( bool &status);

     float readCapacity();

     BatSOCLevels readSOClevels();

private:

     Pufferfish::HAL::AnalogInput &mSamples;
     HAL::DigitalOutput &mRegEnMeas;
     HAL::DigitalOutput &mRegEnCharge;
};

}  // namespace Power
}  // namespace Driver
}  // namespace Pufferfish
