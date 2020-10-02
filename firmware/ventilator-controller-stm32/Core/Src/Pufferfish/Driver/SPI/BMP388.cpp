/// BMP388.cpp
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

#include "Pufferfish/Driver/SPI/BMP388.h"
#include <array>
#include <climits>

namespace Pufferfish::Driver::SPI {

SPIDeviceStatus BMP388::read(RegisterAddress register_type, uint8_t *rx_buf, size_t count) {
  uint8_t tx_register[2];
  tx_register[0] = static_cast<uint8_t>(register_type);
  tx_register[0] |= 0x80;  /// To make 7th bit as '1' for reading
  spi_.chip_select(false);
  if (spi_.write_read(tx_register, rx_buf, count) != SPIDeviceStatus::ok) {
    spi_.chip_select(true);
    return SPIDeviceStatus::read_error;
  }
  spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::write(RegisterAddress register_type, uint8_t *tx_buf, size_t count) {
  /* Byte 1 : RW (bit 7 of byte 1)  reset to write the data */
  uint8_t tx_register[2];
  tx_register[0] = static_cast<uint8_t>(register_type);
  tx_register[0] &= 0x7F;
  tx_register[1] = tx_buf[0];

  spi_.chip_select(false);
  if (spi_.write(tx_register, count) != SPIDeviceStatus::ok) {
    spi_.chip_select(true);
    return SPIDeviceStatus::write_error;
  }
  spi_.chip_select(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::get_chip_id(uint8_t &mem_id) {
  static const uint8_t size = 3;
  std::array<uint8_t, size> rx_data = {0};
  /* Read chip id */
  if (this->read(RegisterAddress::chip_id, rx_data.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  mem_id = rx_data[2];
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::get_errors(SensorError &faults) {
  static const uint8_t size = 3;
  std::array<uint8_t, size> rx_data = {0};
  /* Read Sensor faults */
  if (this->read(RegisterAddress::sensor_errors, rx_data.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  faults.fatal = (((rx_data[2] & 0x01) != 0x00) ? true : false);
  faults.command = (((rx_data[2] & 0x02) != 0x00) ? true : false);
  faults.configuration = (((rx_data[2] & 0x04) != 0x00) ? true : false);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::get_data_ready_interrupt(bool &status) {
  static const uint8_t size = 3;
  std::array<uint8_t, size> rx_data = {0};
  /* Read Sensor faults */
  if (this->read(RegisterAddress::interrupt_status, rx_data.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  status = (((rx_data[2] & 0x80) != 0x00) ? true : false);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::get_sensor_status(SensorStatus &status) {
  static const uint8_t size = 3;
  std::array<uint8_t, size> rx_data = {0};
  /* Read Sensor faults */
  if (this->read(RegisterAddress::sensor_status, rx_data.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  status.cmd_ready = (((rx_data[2] & 0x10) != 0x00) ? true : false);
  status.pressure_ready = (((rx_data[2] & 0x20) != 0x00) ? true : false);
  status.temperature_ready = (((rx_data[2] & 0x40) != 0x00) ? true : false);
  return SPIDeviceStatus::ok;
}

/// Triggers a reset, all user configuration setting are overwritten with their default state
SPIDeviceStatus BMP388::reset() {
  const uint8_t size = 1;
  uint8_t soft_reset = 0xB6;
  /* Write the sensor CMD for reset */
  if (this->write(RegisterAddress::command, &soft_reset, size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /// TBD: delay time needs to be updated
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::enable_spi_communication(SPIInterfaceType const mode) {
  uint8_t data = static_cast<uint8_t>(CommunicationType::spi) | static_cast<uint8_t>(mode);
  uint8_t bit_pos = bit_manipulation(static_cast<uint8_t>(CommunicationType::spi));
  /* Write the sensor CMD for reset */
  if (this->set_register(RegisterAddress::if_config, bit_pos, data) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::set_register(RegisterAddress address, uint8_t const bit_mask, uint8_t value) {
  const uint8_t tx_size = 2;
  uint8_t tx_data;
  static const uint8_t rx_size = 3;
  std::array<uint8_t, rx_size> rx_data = {0};
  /* Read data from register 8*/
  if (this->read(address, rx_data.data(), rx_size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  tx_data = rx_data[2];
  tx_data &= ~(bit_mask);
  tx_data |= (value & bit_mask);
  /* Write data to the register */
  if (this->write(address, &tx_data, tx_size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::enable_pressure(RegisterSet const status) {
  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bit_pos = bit_manipulation(static_cast<uint8_t>(PowerControl::pressure_enable));
  data = data << (bit_pos - 1);
  SPIDeviceStatus ret = this->set_register(RegisterAddress::power_ctrl, bit_pos, data);
  /* Write the sensor CMD for reset */
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::enable_temperature(RegisterSet const status) {
  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bit_pos = bit_manipulation(static_cast<uint8_t>(PowerControl::temperature_enable));
  data = data << (bit_pos - 1);
  SPIDeviceStatus ret = this->set_register(RegisterAddress::power_ctrl, bit_pos, data);
  /* Write the sensor CMD for reset */
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::select_power_mode(PowerModes const mode) {
  uint8_t data = static_cast<uint8_t>(mode);
  uint8_t bit0 = bit_manipulation(static_cast<uint8_t>(PowerControl::mode_bit0));
  uint8_t bit1 = bit_manipulation(static_cast<uint8_t>(PowerControl::mode_bit1));
  uint8_t bit_pos = bit0 | bit1;
  data = data << 4;
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::power_ctrl, bit_pos, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::interrupt_pin_output_type(RegisterSet const status) {
  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bit_pos = bit_manipulation(static_cast<uint8_t>(Interrupts::od_int));
  data = data << (bit_pos - 1);
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::interrupt_control, bit_pos, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::level_of_interrupt_pin(RegisterSet const level) {
  uint8_t data = static_cast<uint8_t>(level);
  uint8_t bit_pos = bit_manipulation(static_cast<uint8_t>(Interrupts::level_of_int));
  data = data << (bit_pos - 1);
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::interrupt_control, bit_pos, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::data_ready_interrupt(RegisterSet const status) {
  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bit_pos = bit_manipulation(static_cast<uint8_t>(Interrupts::latch_int));
  data = data << (bit_pos - 1);
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::interrupt_control, bit_pos, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::pressure_over_sampling(Oversampling const rate) {
  uint8_t data = static_cast<uint8_t>(rate);
  uint8_t bit_mask = 0x07;
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::over_sampling_control, bit_mask, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::temperature_over_sampling(Oversampling const rate) {
  uint8_t data = static_cast<uint8_t>(rate);
  uint8_t bit_mask = 0x38;
  data = data << 3;
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::over_sampling_control, bit_mask, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::output_data_rate(TimeStandby const pre_scaler) {
  uint8_t data = static_cast<uint8_t>(pre_scaler);
  uint8_t bit_mask = 0x1F;

  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::output_data_rates, bit_mask, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::set_iiR_filter(const FilterCoefficient coefficient) {
  uint8_t data = static_cast<uint8_t>(coefficient);
  uint8_t bit_mask = 0x0E;
  data = data << 1;
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->set_register(RegisterAddress::interrupt_control, bit_mask, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::write_error;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::read_calibration_data(TrimValues &data) {
  static const uint8_t len = 23;
  std::array<uint8_t, len> rx_nvm = {0};
  /* Read  data from the register */
  if (this->read(RegisterAddress::calibration_data, rx_nvm.data(), len) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  data.par_t1 = concat_two_bytes(rx_nvm[3], rx_nvm[2]);
  data.par_t2 = concat_two_bytes(rx_nvm[5], rx_nvm[4]);
  data.par_t3 = static_cast<int8_t>(rx_nvm[6]);
  data.par_p1 = static_cast<int16_t>(concat_two_bytes(rx_nvm[8], rx_nvm[7]));
  data.par_p2 = static_cast<int16_t>(concat_two_bytes(rx_nvm[10], rx_nvm[9]));
  data.par_p3 = static_cast<int8_t>(rx_nvm[11]);
  data.par_p4 = static_cast<int8_t>(rx_nvm[12]);
  data.par_p5 = concat_two_bytes(rx_nvm[14], rx_nvm[13]);
  data.par_p6 = concat_two_bytes(rx_nvm[16], rx_nvm[15]);
  data.par_p7 = static_cast<int8_t>(rx_nvm[17]);
  data.par_p8 = static_cast<int8_t>(rx_nvm[18]);
  data.par_p9 = static_cast<int16_t>(concat_two_bytes(rx_nvm[20], rx_nvm[19]));
  data.par_p10 = static_cast<int8_t>(rx_nvm[21]);
  data.par_p11 = static_cast<int8_t>(rx_nvm[22]);
  return SPIDeviceStatus::ok;
}

uint16_t BMP388::concat_two_bytes(uint8_t msb, uint8_t lsb) {
  uint16_t result;
  /* combine two 8 bit data's */
  return result = static_cast<uint16_t>(msb << 8) | static_cast<uint16_t>(lsb);
}

SPIDeviceStatus BMP388::read_raw_data(RawSensorData &samples) {
  static const uint8_t len = 8;
  /* Store the pressure and temperature data read from the sensor to array */
  std::array<uint8_t, len> rx_data = {0};

  /* Read 6 bytes sensor data */
  if (this->read(RegisterAddress::pressure_data, rx_data.data(), len) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  samples.data0 = rx_data[2];
  samples.data1 = rx_data[3];
  samples.data2 = rx_data[4];
  samples.data3 = rx_data[5];
  samples.data4 = rx_data[6];
  samples.data5 = rx_data[7];
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::raw_pressure(uint32_t &pressure) {
  RawSensorData sensor_data;
  uint32_t data_xlsb = 0, data_lsb = 0, data_msb = 0;
  /* Convert 3 bytes pressure data to raw pressure*/
  if (this->read_raw_data(sensor_data) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  data_xlsb = static_cast<uint32_t>(sensor_data.data0);
  data_lsb = static_cast<uint32_t>(sensor_data.data1 << 8);
  data_msb = static_cast<uint32_t>(sensor_data.data2 << 16);
  pressure = data_xlsb | data_lsb | data_msb;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::raw_temperature(uint32_t &temperature) {
  RawSensorData sensor_data;
  uint32_t data_xlsb = 0, data_lsb = 0, data_msb = 0;
  /* Convert 3 bytes Temperature data to raw Temperature*/
  if (this->read_raw_data(sensor_data) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  data_xlsb = static_cast<uint32_t>(sensor_data.data3);
  data_lsb = static_cast<uint32_t>(sensor_data.data4 << 8);
  data_msb = static_cast<uint32_t>(sensor_data.data5 << 16);
  temperature = data_xlsb | data_lsb | data_msb;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::calc_calibration_coefficient(CalibrationData &coefficients) {
  TrimValues data;
  /** Calibration structure is updated from register used for
   * pressure and temperature Trim coefficient
   */
  if (this->read_calibration_data(data) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  coefficients.par_t1 = (static_cast<double>(data.par_t1) / pow(2, -8));
  coefficients.par_t2 = (static_cast<double>(data.par_t2) / pow(2, 30));
  coefficients.par_t3 = (static_cast<double>(data.par_t3) / pow(2, 48));
  coefficients.par_p1 = (static_cast<double>(data.par_p1 - pow(2, 14)) / powf(2, 20));
  coefficients.par_p2 = (static_cast<double>(data.par_p2 - pow(2, 14)) / powf(2, 29));
  coefficients.par_p3 = (static_cast<double>(data.par_p3) / pow(2, 32));
  coefficients.par_p4 = (static_cast<double>(data.par_p4) / pow(2, 37));
  coefficients.par_p5 = (static_cast<double>(data.par_p5) / pow(2, -3));
  coefficients.par_p6 = (static_cast<double>(data.par_p6) / pow(2, 6));
  coefficients.par_p7 = (static_cast<double>(data.par_p7) / pow(2, 8));
  coefficients.par_p8 = (static_cast<double>(data.par_p8) / pow(2, 15));
  coefficients.par_p9 = (static_cast<double>(data.par_p9) / pow(2, 48));
  coefficients.par_p10 = (static_cast<double>(data.par_p10) / pow(2, 48));
  coefficients.par_p11 = (static_cast<double>(data.par_p11) / pow(2, 65));
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::read_compensate_temperature(double &compensated_data) {
  uint32_t uncomp_temperature;
  CalibrationData calib_data;
  /* Read raw temperature */
  if (this->raw_temperature(uncomp_temperature) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Read calibration data */
  if (this->calc_calibration_coefficient(calib_data) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Compensate the temperature using raw temperature and calibration data */
  double const partial_data1 = static_cast<double>(uncomp_temperature - calib_data.par_t1);
  double const partial_data2 = static_cast<double>(partial_data1 * calib_data.par_t2);
  /* Update the compensated temperature in structure since this is needed for pressure calculation*/
  compensated_data = partial_data2 + (partial_data1 * partial_data1) * calib_data.par_t3;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::read_compensate_pressure(double &compensated_pressure) {
  uint32_t uncomp_pressure;
  CalibrationData calib_data;
  double compensated_data;
  /* Read raw Pressure */
  if (this->raw_pressure(uncomp_pressure) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Read calibration data */
  if (this->calc_calibration_coefficient(calib_data) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  /* Read calibration data */
  if (this->read_compensate_temperature(compensated_data) != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::read_error;
  }
  /* Compensate the pressure using raw temperature, raw pressure and calibration data */
  double partial_data1 = calib_data.par_p6 * compensated_data;
  double partial_data2 = calib_data.par_p7 * pow(compensated_data, 2);
  double partial_data3 = calib_data.par_p8 * pow(compensated_data, 3);
  double const partial_out = calib_data.par_p1 + partial_data1 + partial_data2 + partial_data3;
  double const partial_out1 = calib_data.par_p5 + partial_data1 + partial_data2 + partial_data3;

  partial_data1 = calib_data.par_p2 * compensated_data;
  partial_data2 = calib_data.par_p3 * pow(compensated_data, 2);
  partial_data3 = calib_data.par_p4 * pow(compensated_data, 3);
  double const partial_out2 = static_cast<double>(uncomp_pressure) * partial_out;
  partial_data1 = static_cast<double>(uncomp_pressure * uncomp_pressure);
  partial_data2 = calib_data.par_p9 + calib_data.par_p10 * compensated_data;
  partial_data3 = partial_data1 * partial_data2;
  double const partial_out3 = partial_data3 + pow(uncomp_pressure, 3) * calib_data.par_p11;
  compensated_pressure = (partial_out1 + partial_out2 + partial_out3) / 100.0;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::get_sensor_time(uint32_t &time) {
  static const uint8_t len = 5;
  /* Store the pressure and temperature data read from the sensor to array */
  std::array<uint8_t, len> rx_data = {0};
  uint32_t byte1 = 0, byte2 = 0, byte3 = 0;
  /* Read 3 bytes sensor data */
  if (this->read(RegisterAddress::sensor_time, rx_data.data(), len) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  byte1 = static_cast<uint32_t>(rx_data[2]);
  byte2 = static_cast<uint32_t>(rx_data[3]) << 8;
  byte3 = static_cast<uint32_t>(rx_data[4]) << 16;
  time = byte1 | byte2 | byte3;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::get_power_on_reset(bool &flag) {
  static const uint8_t size = 3;
  std::array<uint8_t, size> rx_data = {0};

  /*Read Event flag register */
  if (this->read(RegisterAddress::event, rx_data.data(), size) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::read_error;
  }
  flag = ((rx_data[2] & 0x01) != 0x00) ? true : false;
  return SPIDeviceStatus::ok;
}

}  // namespace Pufferfish::Driver::SPI
