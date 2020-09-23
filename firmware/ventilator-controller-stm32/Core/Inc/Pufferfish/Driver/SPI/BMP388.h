/// BMP388.h
/// DFRobot BMP388 Temperature and Pressure calculation
/// Supports the SPI communication with STM32H743

// Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.


#pragma once

#include "Pufferfish/HAL/Interfaces/SPIDevice.h"
#include <stdint.h>
#include <type_traits>

namespace Pufferfish {
namespace Driver {
namespace SPI {

/**
 * BMP388 chip id 0x50
 */
union chipId {
  struct{
    uint16_t resrved;
    uint8_t id;
  }Bits;
  uint8_t reg[3];
};

/**
 * Sensor Error conditions are reported in the ERR_REG register
 * Bit0: fatal error
 * Bit1: cmd error command execution failed
 * Bit2: Sensor configuration error detected
 */
union SensorFaults {
  struct {
    uint8_t fatalError :1;
    uint8_t commandError :1;
    uint8_t configurationError :1;
    uint8_t resrved :5;
  } Bits;
  uint8_t reg;
};

/**
 * Sensor Status flags are stored in the STATUS register
 * Bit4: command ready
 * Bit5: pressure data ready
 * Bit6: temperature data ready
 */
union SensorStatus {
  struct {
    uint8_t resrved :4;
    uint8_t cmdReady :1;
    uint8_t pressureReady :1;
    uint8_t temperatureReady :1;
    uint8_t unused :1;
  } Bits;
  uint8_t reg;
};

/**
 * The 24 bit pressure data is split and stored in the three consecutive register
 * The 24 bit temperature data is split and stored in the three consecutive register
 * data0: 0x04 Pressure
 * data1: 0x05
 * data2: 0x06
 * data3: 0x04 Temperature
 * data4: 0x05
 * data5: 0x06
 */
union RawSensorData {
  struct {
    uint16_t resrved;
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
    uint8_t data4;
    uint8_t data5;
  } reg;
  uint8_t buf[8];
};

/**
 * The 24 bit sensor time data is split and stord in the three consecutive register
 * time0: 0x0C
 * time1: 0x0D
 * time2: 0x0E
 */
union SensorTime {
  struct {
    uint16_t resrved;
    uint8_t time0;
    uint8_t time1;
    uint8_t time2;
  } reg;
  uint8_t buf[5];
};

/**
 * The Event register contains the sensor status flag
 * Bit0: Power on reset detected
 */
union Event {
  struct {
    uint8_t porDetected :1;
    uint8_t unused :7;
  } Bits;
  uint8_t reg;
};

/**
 * Interrupt status register
 * Bit0: FIFO watermark interrupt
 * Bit1: FIFO Full Interrupt
 * Bit2: Data ready interrupt
 */
union InterruptStatus {
  struct {
    uint8_t fifoWaterMark :1;
    uint8_t fifoFull :1;
    uint8_t unused :1;
    uint8_t dataReady :1;
    uint8_t reserved :4;
  } Bits;
  uint8_t reg;
};

/**
 * Interrupt configuration register. It affects INT_STATUS register and INT pin
 * Bit0: int_od 0->pushPull 1->openDrain
 * Bit1: int_level 0->active Low 1->active high
 * Bit2: int_latch 0->disabled 1->enable
 * Bit3: fwtm_en 0->disabled 1->enable
 * Bit4: ffull_en 0->disabled 1->enable
 * Bit6: drdy_en 0->disabled 1->enable
 */
union InterruptControl {
  struct {
    uint8_t output :1;
    uint8_t level :1;
    uint8_t latch :1;
    uint8_t fwtmEnable :1;
    uint8_t ffullEnable :1;
    uint8_t unused :1;
    uint8_t drdyEnable :1;
    uint8_t reserved :1;
  } Bits;
  uint8_t reg;
};

/**
 * Select serial interface type
 * Bit0: SPI
 * Bit1: i2c_wdt_en
 * Bit2: i2c_wdt_sel
 */
union SerialInterface {
  struct {
    uint8_t spi :1;
    uint8_t i2cWatchDogEnable :1;
    uint8_t i2cWatchDogSelect :1;
    uint8_t unused :5;
  } Bits;
  uint8_t reg;
};

/**
 * Enable or Disable pressure and temperature measurement
 * Bit0: press_en
 * Bit1: temp_en
 * Bit2: mode 00->sleep, 01->forced, 11->normal
 */
union PowerControl {
  struct {
    uint8_t pressureEnable :1;
    uint8_t temperatureEnable :1;
    uint8_t unused :2;
    uint8_t mode :2;
    uint8_t reserved :2;
  } Bits;
  uint8_t reg;
};

/**
 * Oversampling settings for pressure and temperature measurements
 * Bit[2:0]: osr_p
 * Bit[5:3]: osr_t
 */
union SampleRate {
  struct {
    uint8_t pressureOsr :3;
    uint8_t temperatureOsr :3;
    uint8_t unused :2;
  } Bits;
  uint8_t reg;
};

/**
 * Output data rates
 * Bit[4:0]: odr_sel
 */
union OutputDataRate {
  struct {
    uint8_t odrSelect :5;
    uint8_t unused :3;
  } Bits;
  uint8_t reg;
};

/**
 * IIR filter coefficient
 * Bit[3:1]: iir_filter
 */
union IIRFilterConfig {
  struct {
    uint8_t unused :1;
    uint8_t iirFilter :3;
    uint8_t reserved :4;
  } Bits;
  uint8_t reg;
};

/**
 * Register Address
 */
enum class RegisterAddress : uint8_t {
  chipID = 0x00,  /// CHIP_ID: 0x00
  sensorErrors = 0x02,  /// ERR_REG: 0x02
  sensorStatus = 0x03,
  pressureData = 0x04,
  sensorTime = 0x0C,
  event = 0x10,
  interruptstatus = 0x11,
  interruptControl = 0x19,
  ifConfig = 0x1A,
  powerCtrl = 0x1B,
  overSamplingControl = 0x1C,  /// OSR register
  outputDataRates = 0x1D,
  config = 0x1F,
  calibrationData = 0x31,
  command = 0x7E
};

/**
 * Interrupt Types
 */
enum class Interrupts : uint8_t {
  odInt = 0,  /// Configure output 0-> PushPull1,  1-> Open Drain
  levelOfInt,  /// Level of interrupt pin 0 -> active low 1 -> active high
  latchInt,  /// latch of interrupt for INT pin 0-> disable, 1-> enable
  fwtmInt,  /// Enable fifo water mark reached interrupt for INT pin 0-> disable, 1-> enable
  fifoFullInt,  /// Enable fifo full interrupt for INT pin 0-> disable, 1-> enable
  dataReady = 6  /// Enable temperature or pressure data ready 0-> disable, 1-> enable
};

/**
 * Set or reset the bit in register
 */
enum class RegisterSet : bool {
  disable = 0,
  enable = 1
};

/**
 * Register0x1B: PWR_CTRL registers enables or disables pressure and temperature measurements.
 *  Although, the measurement mode can be set here.
 *  Bit 0: press_en
 *  Bit 1: temp_en
 *  Bit [5:4]: 00 -> sleep Mode
 *             01/10 -> forced mode
 *             11 -> normal mode
 */

enum class Modes : uint8_t {
  sleep = 0,
  forced = 1,
  normal = 3
};

/**
 * Sampling rate to read sensor data
 */
enum class Oversampling : uint8_t {
  ultraLowPower = 0,  /// 16bit / 2.64Pa
  lowpower,     /// 17bit / 1.32Pa
  standardResolution,     /// 18bit / 0.66Pa
  highResolution,  /// 19bit / 0.33Pa
  ultraHighResolution,  /// 20bit / 0.17Pa
  highestResolution  /// 21bit / 0.085Pa
};

/**
 * Trimming coefficients from nvm to calculate compensated data
 */
union TrimmingCoefficients {
  struct __attribute__((packed)) {
    uint16_t dummy;
    uint16_t nvmParT1;
    uint16_t nvmParT2;
    int8_t nvmParT3;
    int16_t nvmParP1;
    int16_t nvmParP2;
    int8_t nvmParP3;
    int8_t nvmParP4;
    uint16_t nvmParP5;
    uint16_t nvmParP6;
    int8_t nvmParP7;
    int8_t nvmParP8;
    int16_t nvmParP9;
    int8_t nvmParP10;
    int8_t nvmParP11;
  } data;
  uint8_t buf[23];
};

/**
 * Calibration coefficients
 */
struct CalibrationCoefficient {
  double parT1;
  double parT2;
  double parT3;
  double parP1;
  double parP2;
  double parP3;
  double parP4;
  double parP5;
  double parP6;
  double parP7;
  double parP8;
  double parP9;
  double parP10;
  double parP11;
  double compTemperature;
};

/**
 * Time standby in the Output Data Rate (ODR) register
 */
enum class TimeStandby : uint8_t {
  timestandBy5ms = 0x00,  // Time standby bit field in the Output Data Rate (ODR) register
  timestandBy10ms = 0x01,
  timestandBy20ms = 0x02,
  timestandBy40ms = 0x03,
  timestandBy80ms = 0x04,
  timestandBy160ms = 0x05,
  timestandBy320ms = 0x06,
  timestandBy640ms = 0x07,
  timestandBy1280ms = 0x08,
  timestandBy2560ms = 0x09,
  timestandBy5120ms = 0x0A,
  timestandBy10240ms = 0x0B,
  timestandBy20480ms = 0x0C,
  timestandBy40960ms = 0x0D,
  timestandBy81920ms = 0x0E,
  timestandBy163840ms = 0x0F,
  timestandBy327680ms = 0x10,
  timestandBy655360ms = 0x11
};

/**
 *Infinite Impulse Response (IIR) filter bit field in the configuration register
 */
enum class FilterCoefficient : uint8_t {
  coef0 = 0,
  coef1,
  coef3,
  coef7,
  coef15,
  coef31,
  coef63,
  coef127
};

/**
 * Configure the interrupt pin output type
 */
enum class ConfigureOutput{
  pushPull = 0,
  openDrain
};

/**
 * Abstract class for BMP388 to read pressure
 */
class BMP388 {
 public:

  /**
   * Constructor for BMP388 to read spi device number
   * @param spi spiDevice
   */

  BMP388(HAL::SPIDevice &spi):
      mSpi(spi) {
  }

  /**
   * @brief  Get the chip id register address: 0x00
   * @param  chipId Update the default Chip Id as : 0x50
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status getChipId(uint8_t &memId);

  /**
   * @brief  read sensor fault conditions register address is 0x02
   * @param  fault fatal error or cmd error or config error
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status getErrors(SensorFaults fault);

  /**
   * @brief  read sensor status register address is 0x03
   * @param  status command ready or pressure ready, temperature ready
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status getSensorstatus(SensorStatus status);

  /**
   * @brief  Resets the BMP388 sensor device
   * @param  None
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status reset();

  /**
    * @brief  Enable Pressure sensor
    * @param  RegisterSet enable or disable
    * @return SPIBMP388Status returns ok/writeError
    */
  SPIBMP388Status enablePressure(RegisterSet status);

  /**
   * @brief  read register data
   * @param  RegisterAddress register address
   * @param  value read data from address
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status readRegister(RegisterAddress address, uint8_t &value);

  /**
   * @brief  write to register
   * @param  RegisterAddress register address
   * @param  value write data to address
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status setRegister(RegisterAddress address, uint8_t value);

  /**
   * @brief  Enable temperature sensor
   * @param  RegisterSet enable or disable
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status enableTemperature(RegisterSet status);

  /**
   * @brief  Select power mode
   * @param  Modes:
   *               00-> sleep
   *               01/02-> forced
   *               11->normal
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status selectPowerMode(Modes mode);

  /**
   * @brief  data ready interrupt
   * @param  RegisterSet enable or disable
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status dataReadyInterrupt(RegisterSet status);

  /**
   * @brief  interrupt pin output type
   * @param  ConfigureOutput push pull or open drain
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status InterruptPinOutputType(ConfigureOutput type);

  /**
   * @brief  Level of Interrupt pin
   * @param  RegisterSet active low or active high
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status levelOfInterruptPin(RegisterSet level);

  /**
   * @brief  latching of interrupts for INT pin and int_status register
   * @param  RegisterSet disable or enable
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status latchInterrupt(RegisterSet status);

  /**
   * @brief  Enable FIFO watermark reached interrupt for INT pin and INT_STATUS
   * @param  RegisterSet disable or enable
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status fifoWaterMarkInterrupt(RegisterSet status);

  /**
   * @brief  Enable FIFO full for INT pin and INT_STATUS
   * @param  RegisterSet disable or enable
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status fifoFullInterrupt(RegisterSet status);

  /**
   * @brief  Oversampling settings for pressure
   * @param  Oversampling oversampling rate
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status pressureOverSampling(Oversampling const rate);

  /**
   * @brief  Oversampling settings for temperature
   * @param  Oversampling oversampling rate
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status temperatureOverSampling(Oversampling const rate);

  /**
   * @brief  Control settings for odr selection
   * @param  TimeStandby prescaler values
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status outputDataRate(TimeStandby const prescaler);

  /**
   * @brief  Configure the IIR filter coefficients
   * @param  FilterCoefficient coefficients
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status setIIRFilter(FilterCoefficient coefficient);

  /**
   * @brief  Trimming coefficients from Nvm
   * @param  FilterCoefficient coefficients
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status readCalibrationData(TrimmingCoefficients &data);

  /**
   * @brief  Read sensor data pressure and temperature
   * @param  RawSensorData pressure and temperature
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status readRawData(RawSensorData &data);

  /**
   * @brief  Convert 3 bytes raw data to temperature
   * @param  int32_t temperature
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status rawTemperature(uint32_t &temperature);

  /**
   * @brief  Convert 3 bytes raw data to pressure
   * @param  uint32_t pressure
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status rawPressure(uint32_t &samples);

  /**
   * @brief  Compensated temperature
   * @param  double temperature
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status readCompensateTemperature(double &compensatedData);

  /**
   * @brief  Compensated Pressure
   * @param  double Pressure
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status readCompensatePressure(double &compensatedData);

  /**
   * @brief  Power on reset event flag
   * @param  bool set or reset
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status getPowerOnReset(bool status);

  /**
   * @brief  Sensor timings
   * @param  uint32_t time
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status getSensorTime(uint32_t time);

  /**
   * @brief  Calculate calibration coefficient values
   * @param  CalibrationCoefficient Parameters
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status calcCalibrationCoefficient(CalibrationCoefficient &data);

  /**
   * @brief  Check power on reset detected or not
   * @param  flag Event structure
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status powerOnReset(Event flag);

  /**
     * @brief  Set type of serial communication
     * @param  type SPI or I2C
     * @return SPIBMP388Status returns ok/writeError
     */
  SPIBMP388Status setSerialCommunication(SerialInterface type);

 private:

  /**
   * @brief Writes the data into the BMP388 register
   * @param RegisterAddress   BMP388 read/write registers
   * @param txBuff  buff data to write
   * @param count number of bytes
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status write(RegisterAddress registerType, uint8_t *txBuff,
                        size_t count);

  /**
   * @brief  Writes and read the data from the register
   * @param  RegisterAddress BMP388 read/write registers
   * @param  rxbuf data read from DO pin
   * @return SPIBMP388Status returns ok/readError
   */

  SPIBMP388Status read(RegisterAddress registerType, uint8_t *rxBuf,
                       size_t count);

  /* mSpi object is updated by constructor used to read/write data through SPI */
  HAL::SPIDevice &mSpi;

};
}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish
