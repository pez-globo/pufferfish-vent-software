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

namespace Pufferfish {
namespace Driver {
namespace SPI {

SPIBMP388Status BMP388::read(RegisterAddress registerType, uint8_t *rxBuf,
                             size_t count) {

  uint8_t txRegister[2];
  txRegister[0] = static_cast<uint8_t>(registerType);
  txRegister[0] |= 0x80; /// To make 7th bit as '1' for reading

  mSpi.chipSelect(false);
  if (mSpi.writeRead(txRegister, rxBuf, count) != SPIDeviceStatus::ok) {
    mSpi.chipSelect(true);
    return SPIBMP388Status::readError;
  }
  mSpi.chipSelect(true);
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::write(RegisterAddress registerType, uint8_t *txBuff,
                              size_t count) {

  /* Byte 1 : RW (bit 7 of byte 1)  reset to write the data */
  uint8_t txRegister[2];
  txRegister[0] = static_cast<uint8_t>(registerType);
  txRegister[0] &= 0x7F;
  txRegister[1] = txBuff[0];

  mSpi.chipSelect(false);
  if (mSpi.write(txRegister, count) != SPIDeviceStatus::ok) {
    mSpi.chipSelect(true);
    return SPIBMP388Status::writeError;
  }
  mSpi.chipSelect(true);
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::getChipId(uint8_t &memId) {
  chipId fixedId;
  /* Read chip id */
  if (this->read(RegisterAddress::chipID, fixedId.reg,sizeof(fixedId.reg))
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  memId = fixedId.Bits.id;
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::getErrors(SensorFaults fault) {

  /* Read Sensor faults */
  if (this->readRegister(RegisterAddress::sensorErrors, fault.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::getSensorstatus(SensorStatus status) {

  /* Read Sensor status */
   if (this->readRegister(RegisterAddress::sensorStatus, status.reg)
       != SPIBMP388Status::ok) {
     return SPIBMP388Status::readError;
   }
  return SPIBMP388Status::ok;
}

/// Triggers a reset, all user configuration setting are overwritten with their default state
SPIBMP388Status BMP388::reset() {

  const uint8_t size = 1;
  uint8_t resetCmd = 0xB6;
  /* Write the sensor CMD for reset */
  if (this->write(RegisterAddress::command, &resetCmd, size)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  /// TBD: delay time needs to be updated
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::setSerialCommunication(SerialInterface type) {

  /* Write the sensor CMD for reset */
  if (this->write(RegisterAddress::ifConfig, &type.reg, sizeof(type.reg))
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  /// TBD: delay time needs to be updated
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::enablePressure(RegisterSet status) {

  PowerControl data;
  /* Read pwr_ctrl register data before setting bits */
  if (this->readRegister(RegisterAddress::powerCtrl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.pressureEnable = static_cast<bool>(status);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::powerCtrl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::readRegister(RegisterAddress address, uint8_t &value) {

  const uint8_t rxNumberOfBytes = 3;
  uint8_t rxData[rxNumberOfBytes];
  /* Read data from register 8*/
  if (this->read(address, rxData, rxNumberOfBytes) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }

  value = rxData[2];
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::setRegister(RegisterAddress address, uint8_t value) {

  const uint8_t txNumberOfBytes = 2;
  /* Write data to the register */
  if (this->write(address, &value, txNumberOfBytes) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  return SPIBMP388Status::ok;
}
SPIBMP388Status BMP388::enableTemperature(RegisterSet status) {

  PowerControl data;
  /* Read pwr_ctrl register data before setting bits */
  if (this->readRegister(RegisterAddress::powerCtrl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.temperatureEnable = static_cast<bool>(status);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::powerCtrl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::selectPowerMode(Modes powerMode) {

  PowerControl data;
  /* Read pwr_ctrl register data before setting bits */
  if (this->readRegister(RegisterAddress::powerCtrl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.mode = static_cast<uint8_t>(powerMode);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::powerCtrl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::InterruptPinOutputType(ConfigureOutput type) {

  InterruptControl data;
  /* Read interrupt status register data before setting bits */
  if (this->readRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.output = static_cast<uint8_t>(type);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::levelOfInterruptPin(RegisterSet level) {

  InterruptControl interrupt;
  /* Read interrupt status register data before setting bits */
  if (this->readRegister(RegisterAddress::interruptControl, interrupt.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  interrupt.Bits.level = static_cast<uint8_t>(level);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::interruptControl, interrupt.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::latchInterrupt(RegisterSet status) {

  InterruptControl data;
  /* Read interrupt status register data before setting bits */
  if (this->readRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.latch = static_cast<uint8_t>(status);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::fifoWaterMarkInterrupt(RegisterSet status) {

  InterruptControl data;
  /* Read interrupt status register data before setting bits */
  if (this->readRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.fwtmEnable = static_cast<uint8_t>(status);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::fifoFullInterrupt(RegisterSet status) {

  InterruptControl data;
  /* Read interrupt status register data before setting bits */
  if (this->readRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.ffullEnable = static_cast<uint8_t>(status);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::dataReadyInterrupt(RegisterSet status) {

  InterruptControl data;
  /* Read interrupt status register data before setting bits */
  if (this->readRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.drdyEnable = static_cast<uint8_t>(status);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::interruptControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::pressureOverSampling(Oversampling const rate) {

  SampleRate data;
  /* Read osr register data before setting bits */
  if (this->readRegister(RegisterAddress::overSamplingControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.pressureOsr = static_cast<uint8_t>(rate);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::overSamplingControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::temperatureOverSampling(Oversampling const rate) {

  SampleRate data;
  /* Read osr register data before setting bits */
  if (this->readRegister(RegisterAddress::overSamplingControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.temperatureOsr = static_cast<uint8_t>(rate);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::overSamplingControl, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::outputDataRate(TimeStandby const prescaler) {

  OutputDataRate data;
  /* Read odr register data before setting bits */
  if (this->readRegister(RegisterAddress::outputDataRates, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.odrSelect = static_cast<uint8_t>(prescaler);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::outputDataRates, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::setIIRFilter(const FilterCoefficient coefficient) {

  IIRFilterConfig data;
  /* Read configuration register data before setting bits */
  if (this->readRegister(RegisterAddress::config, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.Bits.iirFilter = static_cast<uint8_t>(coefficient);
  /* Set bit to the register */
  if (this->setRegister(RegisterAddress::config, data.reg)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::writeError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::readCalibrationData(TrimmingCoefficients &data) {

  /* Read trimming coefficient data from NVM for calibration */
  if (this->read(RegisterAddress::calibrationData, data.buf, sizeof(data.buf))
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  return SPIBMP388Status::ok;
}

SPIBMP388Status  BMP388::readRawData(RawSensorData &data){

  /* Read 6 bytes sensor data */
  if (this->read(RegisterAddress::pressureData, data.buf, sizeof(data.buf))
        != SPIBMP388Status::ok) {
      return SPIBMP388Status::readError;
    }
    return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::rawPressure(uint32_t &pressure) {

  RawSensorData sensorData;
  uint32_t dataXlsb = 0, dataLsb = 0, dataMsb = 0;
  /* Convert 3 bytes pressure data to raw pressure*/
  if (this->readRawData(sensorData)!= SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  dataXlsb = static_cast<uint32_t>(sensorData.reg.data0 << 0);
  dataLsb = static_cast<uint32_t>(sensorData.reg.data1 << 8);
  dataMsb = static_cast<uint32_t>(sensorData.reg.data2 << 16);
  pressure = dataXlsb | dataLsb | dataMsb;
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::rawTemperature(uint32_t &temperature) {

  RawSensorData sensorData;
  uint32_t dataXlsb = 0, dataLsb = 0, dataMsb = 0;
  /* Convert 3 bytes Temperature data to raw Temperature*/
  if (this->readRawData(sensorData) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  dataXlsb = static_cast<uint32_t>(sensorData.reg.data3 << 0);
  dataLsb = static_cast<uint32_t>(sensorData.reg.data4 << 8);
  dataMsb = static_cast<uint32_t>(sensorData.reg.data5 << 16);
  temperature = dataXlsb | dataLsb | dataMsb;
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::calcCalibrationCoefficient(
    CalibrationCoefficient &data) {

  TrimmingCoefficients trimValues;
  /** Calibration structure is updated from register used for
   * pressure and temperature Trim coefficient
   */
  if (this->readCalibrationData(trimValues) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  data.parT1 = (static_cast<double>(trimValues.data.nvmParT1) / pow(2, -8));
  data.parT2 = (static_cast<double>(trimValues.data.nvmParT2) / pow(2, 30));
  data.parT3 = (static_cast<double>(trimValues.data.nvmParT3) / pow(2, 48));
  data.parP1 = (static_cast<double>(trimValues.data.nvmParP1 - pow(2, 14))
      / powf(2, 20));
  data.parP2 = (static_cast<double>(trimValues.data.nvmParP2 - pow(2, 14))
      / powf(2, 29));
  data.parP3 = (static_cast<double>(trimValues.data.nvmParP3) / pow(2, 32));
  data.parP4 = (static_cast<double>(trimValues.data.nvmParP4) / pow(2, 37));
  data.parP5 = (static_cast<double>(trimValues.data.nvmParP5) / pow(2, -3));
  data.parP6 = (static_cast<double>(trimValues.data.nvmParP6) / pow(2, 6));
  data.parP7 = (static_cast<double>(trimValues.data.nvmParP7) / pow(2, 8));
  data.parP8 = (static_cast<double>(trimValues.data.nvmParP8) / pow(2, 15));
  data.parP9 = (static_cast<double>(trimValues.data.nvmParP9) / pow(2, 48));
  data.parP10 = (static_cast<double>(trimValues.data.nvmParP10) / pow(2, 48));
  data.parP11 = (static_cast<double>(trimValues.data.nvmParP11) / pow(2, 65));
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::readCompensateTemperature(double &compensatedData) {

  uint32_t uncompTemperature;
  CalibrationCoefficient calibData;
  /* Read raw temperature */
  if (this->rawTemperature(uncompTemperature) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  /* Read calibration data */
  if (this->calcCalibrationCoefficient(calibData) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  /* Compensate the temperature using raw temperature and calibration data */
  double const partialData1 = (double) (uncompTemperature - calibData.parT1);
  double const partialData2 = (double) (partialData1 * calibData.parT2);
  /* Update the compensated temperature in structure since this is needed for pressure calculation*/
  calibData.compTemperature = (partialData2 + (partialData1 * partialData1) * calibData.parT3);
  compensatedData = calibData.compTemperature;
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::readCompensatePressure(double &compensatedPressure) {

  uint32_t uncompPressure;
  CalibrationCoefficient calibData;
  /* Read raw Pressure */
  if (this->rawPressure(uncompPressure) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  /* Read calibration data */
  if (this->calcCalibrationCoefficient(calibData) != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  /* Compensate the pressure using raw temperature, raw pressure and calibration data */
  double partialData1 = calibData.parP6 * calibData.compTemperature;
  double partialData2 = calibData.parP7 * pow(calibData.compTemperature, 2);
  double partialData3 = calibData.parP8 * pow(calibData.compTemperature, 3);
  double const partialOut1 = calibData.parP5 + partialData1 + partialData2 + partialData3;

  partialData1 = calibData.parP2 * calibData.compTemperature;
  partialData2 = calibData.parP3 * pow(calibData.compTemperature, 2);
  partialData3 = calibData.parP4 * pow(calibData.compTemperature, 3);
  double const partialOut2 = static_cast<double>(uncompPressure)
      * (calibData.parP1 + partialData1 + partialData2 + partialData3);

  partialData1 = static_cast<double>(uncompPressure * uncompPressure);
  partialData2 = calibData.parP9 + calibData.parP10 * calibData.compTemperature;
  partialData3 = partialData1 * partialData2;
  double const partialOut3 = partialData3
      + pow(uncompPressure, 3) * calibData.parP11;

  compensatedPressure = (partialOut1 + partialOut2 + partialOut3) / 100.0;
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::getPowerOnReset(bool status){

  uint8_t size = 3;
  uint8_t rxbuf[size];
/* Read the bmp388 sensor status */
  if (this->read(RegisterAddress::sensorStatus, rxbuf, size)
      != SPIBMP388Status::ok) {
    return SPIBMP388Status::readError;
  }
  status = rxbuf[2];
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::getSensorTime(uint32_t time){

  SensorTime data;
  uint32_t byte1 = 0, byte2 = 0, byte3 = 0;
  /* Read 3 bytes sensor timings */
  if (this->read(RegisterAddress::sensorTime, data.buf, sizeof(data.buf))
        != SPIBMP388Status::ok) {
      return SPIBMP388Status::readError;
    }
  byte1 = static_cast<uint32_t> (data.reg.time0);
  byte2 = static_cast<uint32_t> (data.reg.time1) << 8;
  byte3 = static_cast<uint32_t> (data.reg.time2) << 16;
  time = byte1 | byte2 | byte3;
  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::powerOnReset(Event flag){

  /*Read Event flag register */
  if (this->readRegister(RegisterAddress::sensorTime, flag.reg)
        != SPIBMP388Status::ok) {
      return SPIBMP388Status::readError;
    }
  return SPIBMP388Status::ok;
}

}  // namespace BMP388
}  // namespace Driver
}  // namespace Pufferfish
