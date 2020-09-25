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

SPIDeviceStatus BMP388::read(RegisterAddress registerType, uint8_t *rxBuf,
                             size_t count) {

  uint8_t txRegister[2];
  txRegister[0] = static_cast<uint8_t>(registerType);
  txRegister[0] |= 0x80; /// To make 7th bit as '1' for reading

  mSpi.chipSelect(false);
  if (mSpi.writeRead(txRegister, rxBuf, count) != SPIDeviceStatus::ok) {
    mSpi.chipSelect(true);
    return SPIDeviceStatus::readError;
  }
  mSpi.chipSelect(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::write(RegisterAddress registerType, uint8_t *txBuff,
                              size_t count) {

  /* Byte 1 : RW (bit 7 of byte 1)  reset to write the data */
  uint8_t txRegister[2];
  txRegister[0] = static_cast<uint8_t>(registerType);
  txRegister[0] &= 0x7F;
  txRegister[1] = txBuff[0];

  mSpi.chipSelect(false);
  if (mSpi.write(txRegister, count) != SPIDeviceStatus::ok) {
    mSpi.chipSelect(true);
    return SPIDeviceStatus::writeError;
  }
  mSpi.chipSelect(true);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::getChipId(uint8_t &memId) {

  const uint8_t size = 3;
  uint8_t rxData[size];

  /* Read chip id */
  if (this->read(RegisterAddress::chipID, rxData,size)
      != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  memId = rxData[2];
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::getErrors(SensorError faults) {

  const uint8_t size = 3;
  uint8_t rxData[size];
  /* Read Sensor faults */
  if (this->read(RegisterAddress::sensorErrors,rxData,size)
      != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  faults.fatal = (((rxData[2] & 0x01) != 0x00) ? true : false);
  faults.command = (((rxData[2] & 0x02) != 0x00) ? true : false);
  faults.configuration = (((rxData[2] & 0x04) != 0x00) ? true : false);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::getDataReadyInterrupt(bool &status) {

  const uint8_t size = 3;
  uint8_t rxData[size];
  /* Read Sensor faults */
  if (this->read(RegisterAddress::interruptstatus,rxData,size)
      != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  status = (((rxData[2] & 0x80) != 0x00) ? true : false);
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::getSensorstatus(SensorStatus status) {

  const uint8_t size = 3;
   uint8_t rxData[size];
   /* Read Sensor faults */
   if (this->read(RegisterAddress::sensorStatus,rxData,size)
       != SPIDeviceStatus::ok) {
     return SPIDeviceStatus::readError;
   }
   status.cmdReady = (((rxData[2] & 0x10) != 0x00) ? true : false);
   status.pressureReady = (((rxData[2] & 0x20) != 0x00) ? true : false);
   status.temperatureReady = (((rxData[2] & 0x40) != 0x00) ? true : false);
   return SPIDeviceStatus::ok;
}

/// Triggers a reset, all user configuration setting are overwritten with their default state
SPIDeviceStatus BMP388::reset() {

  const uint8_t size = 1;
  uint8_t softReset = 0xB6;
  /* Write the sensor CMD for reset */
  if (this->write(RegisterAddress::command, &softReset, size)
      != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  /// TBD: delay time needs to be updated
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::enableSPIcommunication(SPIInterfaceType const mode) {

  uint8_t data = static_cast<uint8_t>(CommunicationType::spi)| static_cast<uint8_t>(mode);
  uint8_t bitPos = bitManipulation(static_cast<uint8_t>(CommunicationType::spi));

   /* Write the sensor CMD for reset */
  if(this->setRegister(RegisterAddress::ifConfig, bitPos, data) != SPIDeviceStatus::ok ){
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::setRegister(RegisterAddress address,uint8_t const bitmask, uint8_t value) {

  const uint8_t txSize = 2;
  const uint8_t rxSize = 3;
  uint8_t txData;
  uint8_t rxData[rxSize];
  /* Read data from register 8*/
  if (this->read(address, rxData, rxSize) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  txData = rxData[2];
  txData &= ~(bitmask);
  txData |= (value & bitmask);
  /* Write data to the register */
  if (this->write(address, &txData, txSize) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::enablePressure(RegisterSet const status) {

  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bitPos = bitManipulation(static_cast<uint8_t>(PowerControl::pressureEnable));
  data = data << (bitPos - 1);
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::powerCtrl, bitPos, data);
  /* Write the sensor CMD for reset */
  if(ret != SPIDeviceStatus::ok ){
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::enableTemperature(RegisterSet const status) {

  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bitPos = bitManipulation(static_cast<uint8_t>(PowerControl::temperatureEnable));
  data = data << (bitPos - 1);
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::powerCtrl, bitPos, data);
   /* Write the sensor CMD for reset */
   if( ret != SPIDeviceStatus::ok ){
     return SPIDeviceStatus::writeError;
   }
   return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::selectPowerMode(PowerModes const mode) {

  uint8_t data = static_cast<uint8_t>(mode);
  uint8_t bitPos = bitManipulation(static_cast<uint8_t>(PowerControl::modeBit0))
                   |bitManipulation(static_cast<uint8_t>(PowerControl::modeBit1));
  data = data << 4;
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::powerCtrl, bitPos,
                                          data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::InterruptPinOutputType(RegisterSet const status) {

  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bitPos = bitManipulation(static_cast<uint8_t>(Interrupts::odInt));
  data = data << (bitPos - 1);
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::interruptControl, bitPos, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::levelOfInterruptPin(RegisterSet const level) {

  uint8_t data = static_cast<uint8_t>(level);
  uint8_t bitPos = bitManipulation(static_cast<uint8_t>(Interrupts::levelOfInt));
  data = data << (bitPos - 1);
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::interruptControl,
                                          bitPos, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::dataReadyInterrupt(RegisterSet const status) {

  uint8_t data = static_cast<uint8_t>(status);
  uint8_t bitPos = bitManipulation(static_cast<uint8_t>(Interrupts::latchInt));
  data = data << (bitPos - 1);
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::interruptControl,
                                          bitPos, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::pressureOverSampling(Oversampling const rate) {

  uint8_t data = static_cast<uint8_t>(rate);
  uint8_t bitMask = 0x07;
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::overSamplingControl,
                                          bitMask, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::temperatureOverSampling(Oversampling const rate) {

  uint8_t data = static_cast<uint8_t>(rate);
  uint8_t bitMask = 0x38;
  data = data << 3;
  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::overSamplingControl,
                                          bitMask, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::outputDataRate(TimeStandby const prescaler) {

  uint8_t data = static_cast<uint8_t>(prescaler);
  uint8_t bitMask = 0x1F;

  /* Write the sensor CMD for reset */
  SPIDeviceStatus ret = this->setRegister(RegisterAddress::outputDataRates,
                                          bitMask, data);
  if (ret != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::writeError;
  }
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::setIIRFilter(const FilterCoefficient coefficient) {

  uint8_t data = static_cast<uint8_t>(coefficient);
  uint8_t bitMask = 0x0E;
  data = data << 1;
    /* Write the sensor CMD for reset */
    SPIDeviceStatus ret = this->setRegister(RegisterAddress::interruptControl,
                                            bitMask, data);
    if (ret != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::writeError;
    }
    return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::readCalibrationData(TrimValues &data){
  const uint8_t len = 23;
  uint8_t rxNvm[len]={0};

  /* Read  data from the register */
  if(this->read(RegisterAddress::calibrationData, rxNvm, len) != SPIDeviceStatus::ok){
    return SPIDeviceStatus::readError;
  }
  data.parT1 = ConcatTwoBytes(rxNvm[3], rxNvm[2]);
  data.parT2 = ConcatTwoBytes(rxNvm[5], rxNvm[4]);
  data.parT3 =  static_cast<int8_t>(rxNvm[6]);
  data.parP1 = static_cast<int16_t>(ConcatTwoBytes(rxNvm[8], rxNvm[7]));
  data.parP2 = static_cast<int16_t>(ConcatTwoBytes(rxNvm[10], rxNvm[9]));
  data.parP3 =  static_cast<int8_t>(rxNvm[11]);
  data.parP4 =  static_cast<int8_t>(rxNvm[12]);
  data.parP5 = ConcatTwoBytes(rxNvm[14], rxNvm[13]);
  data.parP6 = ConcatTwoBytes(rxNvm[16], rxNvm[15]);
  data.parP7 =  static_cast<int8_t>(rxNvm[17]);
  data.parP8 =  static_cast<int8_t>(rxNvm[18]);
  data.parP9 = static_cast<int16_t>(ConcatTwoBytes(rxNvm[20], rxNvm[19]));
  data.parP10 =  static_cast<int8_t>(rxNvm[21]);
  data.parP11 =  static_cast<int8_t>(rxNvm[22]);
  return SPIDeviceStatus::ok;
}

uint16_t BMP388::ConcatTwoBytes(uint8_t msb, uint8_t lsb){
  uint16_t reult;
  /* combine two 8 bit data's */
  return reult = static_cast<uint16_t>(msb << 8) | static_cast<uint16_t>(lsb);
}

SPIDeviceStatus  BMP388::readRawData(RawSensorData &samples){
  const uint8_t len = 8;
  /* Store the pressure and temperature data read from the sensor to array */
   uint8_t rxdata[len] = { 0 };
  /* Read 6 bytes sensor data */
  if (this->read(RegisterAddress::pressureData, rxdata, len)
        != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::readError;
    }
  samples.data0 = rxdata[2];
  samples.data1 = rxdata[3];
  samples.data2 = rxdata[4];
  samples.data3 = rxdata[5];
  samples.data4 = rxdata[6];
  samples.data5 = rxdata[7];
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::rawPressure(uint32_t &pressure) {

  RawSensorData sensorData;
  uint32_t dataXlsb = 0, dataLsb = 0, dataMsb = 0;
  /* Convert 3 bytes pressure data to raw pressure*/
  if (this->readRawData(sensorData)!= SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  dataXlsb = static_cast<uint32_t>(sensorData.data0);
  dataLsb = static_cast<uint32_t>(sensorData.data1 << 8);
  dataMsb = static_cast<uint32_t>(sensorData.data2 << 16);
  pressure = dataXlsb | dataLsb | dataMsb;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::rawTemperature(uint32_t &temperature) {

  RawSensorData sensorData;
  uint32_t dataXlsb = 0, dataLsb = 0, dataMsb = 0;
  /* Convert 3 bytes Temperature data to raw Temperature*/
  if (this->readRawData(sensorData) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  dataXlsb = static_cast<uint32_t>(sensorData.data3);
  dataLsb = static_cast<uint32_t>(sensorData.data4 << 8);
  dataMsb = static_cast<uint32_t>(sensorData.data5 << 16);
  temperature = dataXlsb | dataLsb | dataMsb;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::calcCalibrationCoefficient(
    CalibrationData &cofficients) {

  TrimValues data;
  /** Calibration structure is updated from register used for
   * pressure and temperature Trim coefficient
   */
  if (this->readCalibrationData(data) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  cofficients.parT1 = (static_cast<double>(data.parT1) / pow(2, -8));
  cofficients.parT2 = (static_cast<double>(data.parT2) / pow(2, 30));
  cofficients.parT3 = (static_cast<double>(data.parT3) / pow(2, 48));
  cofficients.parP1 = (static_cast<double>(data.parP1 - pow(2, 14)) / powf(2, 20));
  cofficients.parP2 = (static_cast<double>(data.parP2 - pow(2, 14)) / powf(2, 29));
  cofficients.parP3 = (static_cast<double>(data.parP3) / pow(2, 32));
  cofficients.parP4 = (static_cast<double>(data.parP4) / pow(2, 37));
  cofficients.parP5 = (static_cast<double>(data.parP5) / pow(2, -3));
  cofficients.parP6 = (static_cast<double>(data.parP6) / pow(2, 6));
  cofficients.parP7 = (static_cast<double>(data.parP7) / pow(2, 8));
  cofficients.parP8 = (static_cast<double>(data.parP8) / pow(2, 15));
  cofficients.parP9 = (static_cast<double>(data.parP9) / pow(2, 48));
  cofficients.parP10 = (static_cast<double>(data.parP10) / pow(2, 48));
  cofficients.parP11 = (static_cast<double>(data.parP11) / pow(2, 65));
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::readCompensateTemperature(double &compensatedData) {

  uint32_t uncompTemperature;
  CalibrationData calibData;
  /* Read raw temperature */
  if (this->rawTemperature(uncompTemperature) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  /* Read calibration data */
  if (this->calcCalibrationCoefficient(calibData) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  /* Compensate the temperature using raw temperature and calibration data */
  double const partialData1 = (double) (uncompTemperature - calibData.parT1);
  double const partialData2 = (double) (partialData1 * calibData.parT2);
  /* Update the compensated temperature in structure since this is needed for pressure calculation*/
  calibData.compTemperature = (partialData2 + (partialData1 * partialData1) * calibData.parT3);
  compensatedData = calibData.compTemperature;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::readCompensatePressure(double &compensatedPressure) {

  uint32_t uncompPressure;
  CalibrationData calibData;
  /* Read raw Pressure */
  if (this->rawPressure(uncompPressure) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
  }
  /* Read calibration data */
  if (this->calcCalibrationCoefficient(calibData) != SPIDeviceStatus::ok) {
    return SPIDeviceStatus::readError;
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
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::getSensorTime(uint32_t &time){

  const uint8_t len = 5;
  /* Store the pressure and temperature data read from the sensor to array */
  uint8_t rxdata[len] = { 0 };
  uint32_t byte1 = 0, byte2 = 0, byte3 = 0;
  /* Read 3 bytes sensor data */
  if (this->read(RegisterAddress::sensorTime, rxdata, len)
        != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::readError;
    }
  byte1 = static_cast<uint32_t>(rxdata[2]);
  byte2 = static_cast<uint32_t>(rxdata[3]) << 8;
  byte3 = static_cast<uint32_t>(rxdata[4]) << 16;
  time = byte1 | byte2 | byte3;
  return SPIDeviceStatus::ok;
}

SPIDeviceStatus BMP388::getPowerOnReset(bool &flag){
  const uint8_t size = 3;
  uint8_t rxData[size];
  /*Read Event flag register */
  if (this->read(RegisterAddress::event, rxData, size)
        != SPIDeviceStatus::ok) {
      return SPIDeviceStatus::readError;
    }
  flag = ((rxData[2] & 0x01) != 0x00) ? true : false;
  return SPIDeviceStatus::ok;
}

}  // namespace BMP388
}  // namespace Driver
}  // namespace Pufferfish
