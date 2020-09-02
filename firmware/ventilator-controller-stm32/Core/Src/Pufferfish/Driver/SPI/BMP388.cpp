/*
 * BMP388.c
 *
 *  Created on: Aug 29, 2020
 *      Author: pradip.jm
 */
#include "Pufferfish/Driver/SPI/BMP388.h"

namespace Pufferfish {
namespace Driver {
namespace BMP388{

SPIBMP388Status SPIBMP388::writeRead(uint8_t *txbuf, uint8_t *rxbuf, size_t count)
{
  txbuf[0] = txbuf[0] | 0x80;

  if (mSpi.writeRead(txbuf, rxbuf,  count) == SPIDeviceStatus::ok) {
    return SPIBMP388Status::ok;
  } else {
    return SPIBMP388Status::readError;
  }
}

SPIBMP388Status SPIBMP388::write(uint8_t *buff, size_t count)
{
  /* Byte 1 : RW (bit 7 of byte 1) is reset to write the data */
  buff[0] = buff[0] & 0x7F;

  if (mSpi.write(buff, count) == SPIDeviceStatus::ok) {
    return SPIBMP388Status::ok;
  } else {
    return SPIBMP388Status::writeError;
  }
}

SPIBMP388Status SPIBMP388::getChipId(uint8_t &chip_id){

  SPIBMP388Status returnStatus;

  uint8_t txbuf[]={0x00,0x00};

  uint8_t rxbuf[]={};

  const uint8_t regDataSize = 1;

  /* Read the chip-id of bmp3 sensor */
  returnStatus = this->writeRead(txbuf, rxbuf, regDataSize);

  /* Update the chip ID from the rx buffer data */
  chip_id = rxbuf[1];

  return returnStatus;
}

void SPIBMP388::ReadCalibrationData(struct bmp388QuantizedCalibData QuantizedCalibData){
  const size_t writeReadSize = 2;
  double temp_var;
  struct bmp388RegCalibData CalibData;
  ;

  uint8_t rxNvmParP11[2]={};
  uint8_t nvmParP11Addr[2] = {0x45, 0x00};

  uint8_t rxNvmParP10[2]={};
  uint8_t nvmParP10Addr[2] = {0x44, 0x00};

  uint8_t rxNvmParP9[2]={};
  uint8_t nvmParP9MAddr[2] = {0x43, 0x00};
  uint8_t nvmParP9lAddr[2] = {0x42, 0x00};

  uint8_t rxNvmParP8[2]={};
  uint8_t nvmParP8Addr[2] = {0x41, 0x00};

  uint8_t rxNvmParP7[2]={};
  uint8_t nvmParP7Addr[2] = {0x40, 0x00};

  uint8_t rxNvmParP6[2]={};
  uint8_t nvmParP6MAddr[2] = {0x3F, 0x00};
  uint8_t nvmParP6lAddr[2] = {0x3E, 0x00};

  uint8_t rxNvmParP5[2]={};
  uint8_t nvmParP5MAddr[2] = {0x3D, 0x00};
  uint8_t nvmParP5lAddr[2] = {0x3C, 0x00};

  uint8_t rxNvmParP4[2]={};
  uint8_t nvmParP4Addr[2] = {0x3B, 0x00};

  uint8_t rxNvmParP3[2]={};
  uint8_t nvmParP3Addr[2] = {0x3A, 0x00};

  uint8_t rxNvmParP2[2]={};
  uint8_t nvmParP2MAddr[2] = {0x39, 0x00};

  uint8_t rxNvmParP1[2]={};
  uint8_t nvmParP1MAddr[2] = {0x37, 0x00};
  uint8_t nvmParP1lAddr[2] = {0x36, 0x00};

  uint8_t rxNvmParT3[2]={};
  uint8_t nvmParT3Addr[2] = {0x35, 0x00};

  uint8_t rxNvmParT2[2]={};
  uint8_t nvmParT2MAddr[2] = {0x32, 0x00};
  uint8_t nvmParT2lAddr[2] = {0x31, 0x00};

  uint8_t rxNvmParT1[2]={};
  uint8_t nvmParT1MAddr[2] = {0x32, 0x00};
  uint8_t nvmParT1lAddr[2] = {0x31, 0x00};

  /* Read NVM_PAR_P11 data from the register */
  this->writeRead(nvmParP11Addr, rxNvmParP11, writeReadSize);
  CalibData.par_p11 = rxNvmParP11[1];
  temp_var = 36893488147419103232.0f;  /* 1 / 2^65 */
  QuantizedCalibData.par_p11 = ((double)CalibData.par_p11 / temp_var);

  this->writeRead(nvmParP10Addr, rxNvmParP10, writeReadSize);
  CalibData.par_p10 = rxNvmParP10[1];
  temp_var = 281474976710656.0f;  /* 1 / 2^48 */
  QuantizedCalibData.par_p10 = ((double)CalibData.par_p10 / temp_var);

  this->writeRead(nvmParP9MAddr, rxNvmParP9, writeReadSize);
  CalibData.par_p9 =(int16_t)rxNvmParP9[1]<<8;
  this->writeRead(nvmParP9lAddr, rxNvmParP9, writeReadSize);
  CalibData.par_p9 = CalibData.par_p9 | rxNvmParP9[1];
  temp_var = 281474976710656.0f;  /* 1 / 2^48 */
  QuantizedCalibData.par_p9 = ((double)CalibData.par_p9 / temp_var);

  this->writeRead(nvmParP8Addr, rxNvmParP8, writeReadSize);
  CalibData.par_p8 = rxNvmParP8[1];
  temp_var = 32768.0f;  /* 1 / 2^15 */
  QuantizedCalibData.par_p8 = ((double)CalibData.par_p8 / temp_var);

  this->writeRead(nvmParP7Addr, rxNvmParP7, writeReadSize);
  CalibData.par_p7 = rxNvmParP7[1];
  temp_var = 256.0f;  /* 1 / 2^8 */
  QuantizedCalibData.par_p7 = ((double)CalibData.par_p7 / temp_var);

  this->writeRead(nvmParP6MAddr, rxNvmParP6, writeReadSize);
  CalibData.par_p6 =(int16_t)rxNvmParP6[1]<<8;
  this->writeRead(nvmParP6lAddr, rxNvmParP6, writeReadSize);
  CalibData.par_p6 = CalibData.par_p6 | rxNvmParP6[1];
  temp_var = 64.0f;  /* 1 / 2^6 */
  QuantizedCalibData.par_p6 = ((double)CalibData.par_p6 / temp_var);

  this->writeRead(nvmParP5MAddr, rxNvmParP5, writeReadSize);
  CalibData.par_p5 =(int16_t)rxNvmParP5[1]<<8;
  this->writeRead(nvmParP5lAddr, rxNvmParP5, writeReadSize);
  CalibData.par_p5 = CalibData.par_p5 | rxNvmParP5[1];
  temp_var = 0.125f;   /* 1 / 2^(-3) */
  QuantizedCalibData.par_p5 = ((double)CalibData.par_p5 / temp_var);

  this->writeRead(nvmParP4Addr, rxNvmParP4, writeReadSize);
  CalibData.par_p4 = rxNvmParP4[1];
  temp_var = 137438953472.0f;  /* 1 / 2^37 */
  QuantizedCalibData.par_p4 = ((double)CalibData.par_p4 / temp_var);

  this->writeRead(nvmParP3Addr, rxNvmParP3, writeReadSize);
  CalibData.par_p3 = rxNvmParP3[1];
  temp_var = 4294967296.0f;   /* 1 / 2^32 */
  QuantizedCalibData.par_p3 = ((double)CalibData.par_p3 / temp_var);

  this->writeRead(nvmParP2MAddr, rxNvmParP2, writeReadSize);
  CalibData.par_p2  =(int16_t)rxNvmParP2[1]<<8;
  uint8_t nvmParP2lAddr[2] = {0x38, 0x00};
  this->writeRead(nvmParP2lAddr, rxNvmParP2, writeReadSize);
  CalibData.par_p2  = CalibData.par_p2  | rxNvmParP2[1];
  temp_var = 536870912.0f; /* 1 / 2^29 */
  QuantizedCalibData.par_p2 = ((double)CalibData.par_p2 - (16384) / temp_var);  /* 1 / 2^14 */

  this->writeRead(nvmParP1MAddr, rxNvmParP1, writeReadSize);
  CalibData.par_p1 =(int16_t)rxNvmParP1[1]<<8;
  this->writeRead(nvmParP1lAddr, rxNvmParP1, writeReadSize);
  CalibData.par_p1 = CalibData.par_p1 | rxNvmParP1[1];
  temp_var = 1048576.0f; /* 1 / 2^20 */
  QuantizedCalibData.par_p1 = ((double)CalibData.par_p1 - (16384) / temp_var);  /* 1 / 2^14 */

  this->writeRead(nvmParT3Addr, rxNvmParT3, writeReadSize);
  CalibData.par_t3 = rxNvmParT3[1];

  temp_var = 281474976710656.0f;   /* 1 / 2^48 */
  QuantizedCalibData.par_t3 = ((double)CalibData.par_t3 / temp_var);

  this->writeRead(nvmParT2MAddr, rxNvmParT2, writeReadSize);
  CalibData.par_t2 =(int16_t)rxNvmParT2[1]<<8;
  this->writeRead(nvmParT2lAddr, rxNvmParT2, writeReadSize);
  CalibData.par_t2 = CalibData.par_t2 | rxNvmParT2[1];

  temp_var = 1073741824.0f;   /* 1 / 2^30 */
  QuantizedCalibData.par_t2 = ((double)CalibData.par_t2 / temp_var);

  this->writeRead(nvmParT1MAddr, rxNvmParT1, writeReadSize);
  CalibData.par_t1 =(int16_t)rxNvmParT1[1]<<8;
  this->writeRead(nvmParT1lAddr, rxNvmParT1, writeReadSize);
  CalibData.par_t1 =  CalibData.par_t1|rxNvmParT1[1];

  temp_var = 0.00390625f;   /* 1 / 2^(-8) */
  QuantizedCalibData.par_t1 = ((double)CalibData.par_t1 / temp_var);
}

SPIBMP388Status SPIBMP388::setIIRFilter(bool iirFilterEnabel, SPIBMP388::IIRFilterCoefficient IIRFilterCoeff){
  const uint8_t regDataSize = 1;
  uint8_t iirFilterData[2];
  iirFilterData[0] = 0x1F;
  if (iirFilterEnabel == false){
    iirFilterData[1] = 0x00;
    return this->write( iirFilterData, regDataSize);
   }
   else{
     switch(IIRFilterCoeff){
       case IIRFilterCoefficient::iirFilter1:
                       iirFilterData[1] = 0x02;
                       return this->write( iirFilterData, regDataSize);
                       break;
       case IIRFilterCoefficient::iirFilter3:
                       iirFilterData[1] = 0x04;
                       return this->write( iirFilterData, regDataSize);
                       break;
       case IIRFilterCoefficient::iirFilter7:
                       iirFilterData[1] = 0x06;
                       return this->write( iirFilterData, regDataSize);
                       break;
       case IIRFilterCoefficient::iirFilter15:
                       iirFilterData[1] = 0x08;
                       return this->write( iirFilterData, regDataSize);
                       break;
       case IIRFilterCoefficient::iirFilter31:
                       iirFilterData[1] = 0x0A;
                       return this->write( iirFilterData, regDataSize);
                       break;
       case IIRFilterCoefficient::iirFilter63:
                       iirFilterData[1] = 0x0C;
                       return this->write( iirFilterData, regDataSize);
                       break;
       case IIRFilterCoefficient::iirFilter127:
                       iirFilterData[1] = 0x0E;
                       return this->write( iirFilterData, regDataSize);
                       break;
       default :
               iirFilterData[1] = 0x00;
               return this->write( iirFilterData, regDataSize);
               break;
         }
     }
}

SPIBMP388Status SPIBMP388::spiReset() {
  /**
   * Byte 1 : write 0x7E address for CMD
   * Byte 2 : B6 for soft reset
   * */
  uint8_t resetCmd[2] = {0x7E, 0xB6};

  const uint8_t resetCmdSize = 2;

  return this->write(resetCmd, resetCmdSize);
}

SPIBMP388Status SPIBMP388::setPowerCtrlSettings(struct bmp388Settings bmp388Value)
{
  const uint8_t regDataSize = 2;
  uint8_t regData = 0x00;
  uint8_t txRegData[2];

  if (bmp388Value.preasureEnable == true){
    regData = regData | bmp388PreasureEnable;
  }
  if (bmp388Value.temperatureEnable == true){
    regData = regData | bmp388TempratureEnable;
  }
  if (bmp388Value.sleepMode == true){
      regData = regData | bmp388SleepMode;
    }
    else if (bmp388Value.ForcedMode == true){
      regData = regData | bmp388ForcedMode;
    }
    else if (bmp388Value.NormalMode == true){
      regData = regData | bmp388NormalMode;
   }
   txRegData[0] = 0x1B;
   txRegData[1] = regData;

  return this->write( txRegData, regDataSize);
}

uint32_t SPIBMP388::ReadTemperatureData(){
  /* Temporary variables to store the temperature sensor data */
  uint32_t data_xlsb;
  uint32_t data_lsb;
  uint32_t data_msb;

  uint8_t rxbuf[2]={};
  uint8_t txbuf1[2] = {0x07, 0x00};

  this->writeRead(txbuf1, rxbuf, 2);
  data_xlsb = ((uint32_t)rxbuf[1]) & 0x000000FF;
  uint8_t txbuf2[2] = {0x08, 0x00};

  this->writeRead(txbuf2, rxbuf, 2);
  data_lsb = ((uint32_t)rxbuf[1] << 8) & 0x0000FF00;
  uint8_t txbuf3[2] = {0x09, 0x00};

  this->writeRead(txbuf3, rxbuf, 2);
  data_msb = ((uint32_t)rxbuf[1] << 16) & 0x00FF0000;

  return (data_msb | data_lsb | data_xlsb);
}

uint32_t SPIBMP388::ReadPressureData(){
  /* Temporary variables to store the pressure sensor data */
  uint32_t data_xlsb;
  uint32_t data_lsb;
  uint32_t data_msb;

  uint8_t rxbuf[2]={};
  uint8_t txbuf1[2] = {0x04, 0x00};
  this->writeRead(txbuf1, rxbuf, 2);
  data_xlsb =(uint32_t)rxbuf[1];
  uint8_t txbuf2[2] = {0x05, 0x00};
  this->writeRead(txbuf2, rxbuf, 2);
  data_lsb =(uint32_t)rxbuf[1] << 8;
  uint8_t txbuf3[2] = {0x06, 0x00};
  this->writeRead(txbuf3, rxbuf, 2);
  data_msb =(uint32_t)rxbuf[1] << 16;

  return (data_msb | data_lsb | data_xlsb);
}

}  // namespace BMP388
}  // namespace Driver
}  // namespace Pufferfish

