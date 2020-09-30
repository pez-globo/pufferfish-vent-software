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
#include <math.h>

namespace Pufferfish {
namespace Driver {
namespace SPI {

/**
 * Sensor Error conditions are reported in the ERR_REG register
 * Bit0: fatal error
 * Bit1: cmd error command execution failed
 * Bit2: Sensor configuration error detected
 */
struct SensorError {
  bool fatal;
  bool command;
  bool configuration;
};

/**
 * Sensor Status flags are stored in the STATUS register
 * Bit4: command ready
 * Bit5: pressure data ready
 * Bit6: temperature data ready
 */
struct SensorStatus {
  bool cmd_ready;
  bool pressure_ready;
  bool temperature_ready;
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
struct RawSensorData {
  uint8_t data0;
  uint8_t data1;
  uint8_t data2;
  uint8_t data3;
  uint8_t data4;
  uint8_t data5;
};

/**
 * Select serial interface type
 * Bit0: SPI
 * Bit1: i2c_wdt_en
 * Bit2: i2c_wdt_sel
 */
enum class SerialInterface {
  spi = 0,         /// SPI - bit 0
  i2c_wdt_enable,  /// i2c_wdt_en - bit  1
  i2c_wdt_select   /// i2c_wdt_sel - bit 2
};

/**
 * Register Address
 */
enum class RegisterAddress : uint8_t {
  chip_id = 0x00,
  sensor_errors = 0x02,  /// ERR_REG: 0x02
  sensor_status = 0x03,
  pressure_data = 0x04,
  sensor_time = 0x0C,
  event = 0x10,
  interrupt_status = 0x11,
  interrupt_control = 0x19,
  if_config = 0x1A,
  power_ctrl = 0x1B,
  over_sampling_control = 0x1C,  /// OSR register
  output_data_rates = 0x1D,
  config = 0x1F,
  calibration_data = 0x31,
  command = 0x7E
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
enum class Interrupts : uint8_t {
  od_int = 0,     /// Configure output 0-> PushPull,  1-> Open Drain
  level_of_int,   /// Level of interrupt pin 0 -> active low 1 -> active high
  latch_int,      /// latch of interrupt for INT pin 0-> disable, 1-> enable
  fwtm_int,       /// Enable fifo water mark reached interrupt for INT pin 0-> disable, 1-> enable
  fifo_full_int,  /// Enable fifo full interrupt for INT pin 0-> disable, 1-> enable
  data_ready = 6  /// Enable temperature or pressure data ready 0-> disable, 1-> enable
};

/**
 * Set or reset the bit in register
 */
enum class RegisterSet : bool {
  disable = 0,  /// disable
  enable = 1    /// enable
};

/**
 * Oversampling settings for pressure and temperature measurements
 * Bit[2:0]: osr_p
 * Bit[5:3]: osr_t
 */
enum class Oversampling : uint8_t {
  ultra_low_power = 0,    /// 16bit / 2.64Pa
  low_power,              /// 17bit / 1.32Pa
  standard_resolution,    /// 18bit / 0.66Pa
  high_resolution,        /// 19bit / 0.33Pa
  ultra_high_resolution,  /// 20bit / 0.17Pa
  highest_resolution      /// 21bit / 0.085Pa
};

/**
 * Trimming coefficients from nvm to calculate compensated data
 */
struct TrimValues {
  uint16_t parT1;
  uint16_t parT2;
  uint16_t parP5;
  uint16_t parP6;
  int16_t parP1;
  int16_t parP2;
  int16_t parP9;
  int8_t parT3;
  int8_t parP3;
  int8_t parP4;
  int8_t parP7;
  int8_t parP8;
  int8_t parP10;
  int8_t parP11;
};

/**
 * Calibration coefficients
 */
struct CalibrationData {
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
  double comp_temperature;
};

/**
 * Time standby in the Output Data Rate (ODR) register
 */
enum class TimeStandby : uint8_t {
  time5ms = 0x00,  /// Time standby bit field in the Output Data Rate (ODR) register
  time10ms = 0x01,
  time20ms = 0x02,
  time40ms = 0x03,
  time80ms = 0x04,
  time160ms = 0x05,
  time320ms = 0x06,
  time640ms = 0x07,
  time1280ms = 0x08,
  time2560ms = 0x09,
  time5120ms = 0x0A,
  time10240ms = 0x0B,
  time20480ms = 0x0C,
  time40960ms = 0x0D,
  time81920ms = 0x0E,
  time163840ms = 0x0F,
  time327680ms = 0x10,
  time655360ms = 0x11
};

/**
 * Infinite Impulse Response (IIR) filter bit field in the configuration register
 * Bit[3:1]: iir_filter
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
enum class ConfigureOutput {
  push_pull = 0,  /// push pull
  open_drain      /// open drain
};

/**
 * SPI interface mode
 * Bit0: 0 -> 4 wire mode
 *       1 -> 3 wire mode
 */
enum class SPIInterfaceType : uint8_t {
  spi4 = 0,  /// 4 wire mode
  spi3       /// 3 wire mode
};

/**
 * SPI Communication mode
 */
enum class CommunicationType : uint8_t {
  spi = 0,
  i2c_wdt_enable,  /// i2c_wdt_enable
  i2c_wdt_select   /// i2c_wdt_select
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
enum class PowerModes {
  sleep = 0,  /// sleep Mode
  force = 1,  /// forced Mode
  normal = 3  /// normal Mode
};

/**
 * Enable or Disable pressure and temperature measurement
 * Bit0: press_en
 * Bit1: temp_en
 * Bit2: mode 00->sleep, 01->forced, 11->normal
 */
enum class PowerControl {
  pressure_enable = 0,
  temperature_enable = 1,
  mode_bit0 = 4,
  mode_bit1 = 5  ///
};

/**
 * Bit Manipulation
 */
inline uint8_t bit_manipulation(uint8_t pos) {
  return static_cast<uint8_t>(1 << pos);
}

/**
 * Abstract class for BMP388 to read pressure
 */
class BMP388 {
 public:
  /**
   * Constructor for BMP388 to read spi device number
   * @param spi spiDevice
   */
  BMP388(HAL::SPIDevice &spi) : spi_(spi) {}

  /**
   * @brief  Get the chip id register address: 0x00
   * @param  chipId Update the default Chip Id as : 0x50
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus get_chip_id(uint8_t &mem_id);

  /**
   * @brief  read sensor fault conditions register address is 0x02
   * @param  fault fatal error or cmd error or config error
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus get_errors(SensorError &fault);

  /**
   * @brief  read sensor status register address is 0x03
   * @param  status command ready or pressure ready, temperature ready
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus get_sensor_status(SensorStatus &status);

  /**
   * @brief  Resets the BMP388 sensor device
   * @param  None
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus reset();

  /**
   * @brief  Enable Pressure sensor
   * @param  RegisterSet enable or disable
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus enable_pressure(RegisterSet status);

  /**
   * @brief  Enable temperature sensor
   * @param  RegisterSet enable or disable
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus enable_temperature(RegisterSet status);

  /**
   * @brief  Select power mode
   * @param  Modes:
   * 00-> sleep
   * 01/02-> forced
   * 11->normal
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus select_power_mode(PowerModes mode);

  /**
   * @brief  data ready interrupt
   * @param  RegisterSet enable or disable
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus data_ready_interrupt(RegisterSet status);

  /**
   * @brief  interrupt pin output type
   * @param  ConfigureOutput push pull or open drain
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus interrupt_pin_output_type(RegisterSet type);

  /**
   * @brief  Level of Interrupt pin
   * @param  RegisterSet active low or active high
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus level_of_interrupt_pin(RegisterSet level);

  /**
   * @brief  Oversampling settings for pressure
   * @param  Oversampling oversampling rate
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus pressure_over_sampling(Oversampling const rate);

  /**
   * @brief  Oversampling settings for temperature
   * @param  Oversampling oversampling rate
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus temperature_over_sampling(Oversampling const rate);

  /**
   * @brief  Control settings for odr selection
   * @param  TimeStandby prescaler values
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus output_data_rate(TimeStandby const pre_scaler);

  /**
   * @brief  Configure the IIR filter coefficients
   * @param  FilterCoefficient coefficients
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus set_IIR_filter(FilterCoefficient coefficient);

  /**
   * @brief  Trimming coefficients from Nvm
   * @param  FilterCoefficient coefficients
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus read_calibration_data(TrimValues &data);

  /**
   * @brief  Read sensor data pressure and temperature
   * @param  RawSensorData pressure and temperature
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus read_raw_data(RawSensorData &data);

  /**
   * @brief  Convert 3 bytes raw data to temperature
   * @param  int32_t temperature
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus raw_temperature(uint32_t &temperature);

  /**
   * @brief  Convert 3 bytes raw data to pressure
   * @param  uint32_t pressure
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus raw_pressure(uint32_t &samples);

  /**
   * @brief  Compensated temperature
   * @param  double temperature
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus read_compensate_temperature(double &compensated_data);

  /**
   * @brief  Compensated Pressure
   * @param  double compensated_pressure
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus read_compensate_pressure(double &compensated_pressure);

  /**
   * @brief  Power on reset event flag
   * @param  bool set or reset
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus get_power_on_reset(bool status);

  /**
   * @brief  Sensor timings
   * @param  uint32_t time
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus get_sensor_time(uint32_t &time);

  /**
   * @brief  Calculate calibration coefficient values
   * @param  CalibrationCoefficient Parameters
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus calc_calibration_coefficient(CalibrationData &coefficient);

  /**
   * @brief  Check power on reset detected or not
   * @param  flag Event structure
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus get_power_on_reset(bool &flag);

  /**
   * @brief  Set type of serial communication
   * @param  type SPI or I2C
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus enable_spi_communication(SPIInterfaceType mode);

  /**
   * @brief  Data ready interrupt status is asserted after a pressure and temperature
   * measurements and conversion data stored to the data register
   * @param  status data ready or not
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus get_data_ready_interrupt(bool &status);

  /**
   * @brief  Internal method to combine two 8 bit data's to form a 16 bit data
   * @param  msb and lsb
   * @return returns result
   */
  uint16_t concat_two_bytes(uint8_t msb, uint8_t lsb);

 private:
  /**
   * @brief Writes the data into the BMP388 register
   * @param RegisterAddress   BMP388 read/write registers
   * @param tx_buf  buff data to write
   * @param count number of bytes
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus write(RegisterAddress register_type, uint8_t *tx_buf, size_t count);

  /**
   * @brief  Writes and read the data from the register
   * @param  RegisterAddress BMP388 read/write registers
   * @param  rx_buf data read from DO pin
   * @return SPIDeviceStatus returns ok/readError
   */
  SPIDeviceStatus read(RegisterAddress register_type, uint8_t *rx_buf, size_t count);

  /**
   * @brief  write to register
   * @param  RegisterAddress register address
   * @param  value write data to address
   * @return SPIDeviceStatus returns ok/writeError
   */
  SPIDeviceStatus set_register(RegisterAddress address, uint8_t const bit_mask, uint8_t value);

  /* spi_ object is updated by constructor used to read/write data through SPI */
  HAL::SPIDevice &spi_;
};

}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish
