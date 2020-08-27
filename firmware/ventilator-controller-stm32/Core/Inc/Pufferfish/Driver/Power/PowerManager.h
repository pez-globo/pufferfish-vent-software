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

/**
 * PwrSource enum reports current active power source
 */
enum class PwrSource
{
  noPwr = 0,       /// Ventilator not detected power source to operate
  external12vPwr,  /// Ventilator operating on external power source
  internal12Vpwr   /// Ventilator operating on internal battery power source
};

/**
 *  PwrFaults enum reports various faults related to power
 */
enum class PwrFaults
{
  noFault = 0,         /// No faults related to power module
  external12vPwrfault, /// Fault due to over current on external power source
  internal12Vpwrfault, /// Fault due to over current on internal power source
  extPgoodFault,       /// External power good fault
  intPgoodFault        /// Internal power good fault
};


/**
 *  Battery related parameters
 */
/**
 * BatStatus enum reports the battery status
 */
enum class BatStatus
{
  detected = 0, /// Battery status is detected
  notDetected  /// Battery status is not detected
};

enum class BatSOCLevels
{
  Drained = 0,
  lowerLimit,
  midRange,
  HighLimit,
  MaxCharge
};

enum class BatChargeStatus
{
  charging=0,     /// Battery charge status is charging
  discharging,    /// Battery charge status is discharging
  fullCharged,    /// Battery charge status is full-charged (100% charged)
  fullDischarged  /// Battery charge status is full-discharged (0% charge)
};

struct Profile
{
  float batteryVoltage;
  float batteryTemperature;
  float socMaximum;
  float socMedium;
  float socLow;
  float socCritical;
};

/*
 *  Structure for input pins of LTC4421
 */
struct LTC4421InputPin
{
  /* regCh1       Digital Input signal for V1 Primary power source indicator */
  HAL::DigitalInput  &regCh1;
  /* regCh2       Digital Input signal for V2 Primary power source indicator */
  HAL::DigitalInput  &regCh2;
  /* regValid1    Digital Input signal for validating V1 power source output voltage */
  HAL::DigitalInput  &regValid1;
  /* regValid2    Digital Input signal for validating V2 power source output voltage */
  HAL::DigitalInput  &regValid2;
  /* regFault1    Digital Input signal for V1 current fault indicator */
  HAL::DigitalInput  &regFault1;
  /* regFault2    Digital Input signal for V2 current fault indicator */
  HAL::DigitalInput  &regFault2;
  /* regDisable1  Digital Output signal for V1 disable */
  HAL::DigitalOutput &regDisable1;
  /* regDisable2  Digital Output signal for V2 disable */
  HAL::DigitalOutput &regDisable2;
  /* regPgood3V3  Digital Output signal for 3.3V Power Good Signal */
  HAL::DigitalOutput &regPgood3V3;
  /* regPgood5V0  Digital Output signal for 5.0V Power Good Signal */
  HAL::DigitalOutput &regPgood5V0;
};

/**
 * An abstract class for Power Path control module
 */
class LTC4421 {
 public:
  /**
   * @brief  Constructs a new LTC4421 object
   * @param  LTC4421Input - Takes LTC4421 Pin input
   */
  LTC4421(LTC4421InputPin LTC4421Input) : mLTC4421Input(LTC4421Input) {

  }

  /**
   * @brief  Read Power source
   * @param  None
   * @return returns No power / Internal power / External Power based on input
   */
  PwrSource readPwrSource();

  ///void stop(PwrSource &channel);//TBD

  /**
   * @brief  fault
   * @param  None
   * @return returns any power faults occurred
   */
  PwrFaults fault();

 private:
  /* Private variable for LTC4421 input pins */
  LTC4421InputPin mLTC4421Input;
  bool mExtPwr = false;bool mIntPwr = false;
  bool mExtPwrFault = false;bool mIntPwrFault = false;
  bool mExtPgoodFault = false;bool mIntPgoodFault = false;

};

/* FIXME: Move to separate source file */
/**
 * An abstract class for Battery Charge control module
 */
class Battery {

 public:
    const float batteryTotalCapacityVoltage = 13.8f;
    const float batteryVoltageUpperLimit1   = 12.0f;
    const float batteryVoltageLowerLimit1   = 10.0f;
    const float batteryDeepDischargeVoltage = 9.0f;

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
     BatStatus readVoltage(); //AVINTBD : float &batteryVoltage);

     /**
       * @brief  enables the charge of battery based on status
       * @param  status -
       * @return None
       */
     void enBatteryCharge ( bool &status);

     float capacity();

     BatSOCLevels socLevels();

     BatChargeStatus status();

     //BatStatus readProfile(Profile &parameters);//////////////////////////

 private:
     Pufferfish::HAL::AnalogInput &mSamples;
     HAL::DigitalOutput &mRegEnMeas;
     HAL::DigitalOutput &mRegEnCharge;
     BatChargeStatus BatteryChargeStatus;
     uint32_t VolageRawValue = 0;
     float batteryVoltage = 0.0f;
     float lastBatteryVoltage = 0.0f;
     float batteryCapicity = 0.0f;

};

}  // namespace Power
}  // namespace Driver
}  // namespace Pufferfish
