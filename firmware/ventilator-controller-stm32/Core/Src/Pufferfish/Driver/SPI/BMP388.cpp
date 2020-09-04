/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: pradip.jm
 */
#include "Pufferfish/Driver/SPI/BMP388.h"

namespace Pufferfish {
namespace Driver {
namespace SPI {

SPIBMP388Status BMP388::writeRead(uint8_t *txbuf, uint8_t *rxbuf, size_t count){
  txbuf[0] = txbuf[0] | 0x80;

  if (mSpi.writeRead(txbuf, rxbuf,  count) == SPIDeviceStatus::ok) {
    return SPIBMP388Status::ok;
  } else {
    return SPIBMP388Status::readError;
  }
}

SPIBMP388Status BMP388::write(uint8_t *buff, size_t count){
  /* Byte 1 : RW (bit 7 of byte 1) is reset to write the data */
  buff[0] = buff[0] & 0x7F;

  if (mSpi.write(buff, count) == SPIDeviceStatus::ok) {
    return SPIBMP388Status::ok;
  } else {
    return SPIBMP388Status::writeError;
  }
}

SPIBMP388Status BMP388::bmp388ConfigSettings(const bool enableIIRFilter,
                                  const IIRFilterCoefficient IIRFilterCoeff,
                                  const struct bmp388Settings setBMP388PwrControl) {
 uint8_t chip_id  = 0x00;
  /* Reset the BMP388 */
  if(SPIBMP388Status::ok != spiReset()){
    /* Unable to reset return writeError */
    return SPIBMP388Status::writeError;
  }

  /* Get the chip ID */
  if(SPIBMP388Status::ok != getChipId(chip_id)){
    /* Unable to get chip id from register*/
    return SPIBMP388Status::readError;
  }
  if (Bmp388ChipId != chip_id ){
    /* The ID is incorrect return chipIdError */
    return SPIBMP388Status::chipIdError;
  }

  /* reads the CalibrationData data from NVM */
  ReadCalibrationData();

  /* Initialize the BMP388 IIR filter register */
  if(SPIBMP388Status::ok != setIIRFilter(enableIIRFilter , IIRFilterCoeff)){
    /*  Unable to set IIRFilter */
    return SPIBMP388Status::writeError;
  }
  ;

  /* Set power control register to enable pressure */
  if(SPIBMP388Status::ok != setPowerCtrlSettings(setBMP388PwrControl)) {
    /*  Unable to set power control setting to the register */
    return SPIBMP388Status::writeError;
  }

  return SPIBMP388Status::ok;
}



SPIBMP388Status BMP388::getChipId(uint8_t &chipId){
  /*
   * txbuf : transmit buffer of 2 bytes
   * Byte 1 : 0x00 is the register address of CHIP_ID.
   * Byte 2 : 0x00 is dummy data to write and read the chip id in rxbuf
   */
  SPIBMP388Status returnStatus;
  uint8_t txbuf[]={0x00,0x00};
  uint8_t rxbuf[]={};

  const uint8_t regDataSize = 1;

  /* Read the chip-id of bmp388 sensor */
  returnStatus = this->writeRead(txbuf, rxbuf, regDataSize);

  /* Update the chip ID from the rx buffer data */
  chipId = rxbuf[1];

  return returnStatus;
}

void BMP388::ReadCalibrationData(){
  const size_t writeReadSize = 2;

  double temp_var;
  int8_t signedCalibData8;
  int16_t signedCalibData16;
  uint16_t unsignedCalibData16;

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
  signedCalibData8 = static_cast<int8_t>(rxNvmParP11[1]);
  temp_var = 36893488147419103232.0f;  /* 2^65 */
  QuantizedCalibData.par_p11 = (static_cast<double>(signedCalibData8) / temp_var);

  /* Read NVM_PAR_P10 data from the register */
  this->writeRead(nvmParP10Addr, rxNvmParP10, writeReadSize);
  signedCalibData8 = rxNvmParP10[1];
  temp_var = 281474976710656.0f;  /* 2^48 */
  QuantizedCalibData.par_p10 = (static_cast<double>(signedCalibData8) / temp_var);

  /* Read NVM_PAR_P9 data from the register */
  this->writeRead(nvmParP9MAddr, rxNvmParP9, writeReadSize);
  signedCalibData16 = (static_cast<int16_t>(rxNvmParP9[1]) << 8) & 0xFF00;
  this->writeRead(nvmParP9lAddr, rxNvmParP9, writeReadSize);
  signedCalibData16 = signedCalibData16 | rxNvmParP9[1];
  temp_var = 281474976710656.0f;  /* 2^48 */
  QuantizedCalibData.par_p9 = (static_cast<double>(signedCalibData16) / temp_var);

  /* Read NVM_PAR_P8 data from the register */
  this->writeRead(nvmParP8Addr, rxNvmParP8, writeReadSize);
  signedCalibData8 = rxNvmParP8[1];
  temp_var = 32768.0f;  /* 2^15 */
  QuantizedCalibData.par_p8 = (static_cast<double>(signedCalibData8) / temp_var);

  /* Read NVM_PAR_P7 data from the register */
  this->writeRead(nvmParP7Addr, rxNvmParP7, writeReadSize);
  signedCalibData8 = rxNvmParP7[1];
  temp_var = 256.0f;  /* 2^8 */
  QuantizedCalibData.par_p7 = (static_cast<double>(signedCalibData8) / temp_var);

  /* Read NVM_PAR_P6 data from the register */
  this->writeRead(nvmParP6MAddr, rxNvmParP6, writeReadSize);
  unsignedCalibData16 = (static_cast<int16_t>(rxNvmParP6[1]) << 8) & 0xFF00;
  this->writeRead(nvmParP6lAddr, rxNvmParP6, writeReadSize);
  unsignedCalibData16 = unsignedCalibData16 | rxNvmParP6[1];
  temp_var = 64.0f;  /* 1 / 2^6 */
  QuantizedCalibData.par_p6 = (static_cast<double>(unsignedCalibData16) / temp_var);

  /* Read NVM_PAR_P5 data from the register */
  this->writeRead(nvmParP5MAddr, rxNvmParP5, writeReadSize);
  unsignedCalibData16 = (static_cast<int16_t>(rxNvmParP5[1]) << 8) & 0xFF00;
  this->writeRead(nvmParP5lAddr, rxNvmParP5, writeReadSize);
  unsignedCalibData16 = unsignedCalibData16 | rxNvmParP5[1];
  temp_var = 0.125f;   /* 1 / 2^(-3) */
  QuantizedCalibData.par_p5 = (static_cast<double>(unsignedCalibData16) / temp_var);

  /* Read NVM_PAR_P4 data from the register */
  this->writeRead(nvmParP4Addr, rxNvmParP4, writeReadSize);
  signedCalibData8 = rxNvmParP4[1];
  temp_var = 137438953472.0f;  /* 1 / 2^37 */
  QuantizedCalibData.par_p4 = (static_cast<double>(signedCalibData8) / temp_var);

  /* Read NVM_PAR_P3 data from the register */
  this->writeRead(nvmParP3Addr, rxNvmParP3, writeReadSize);
  signedCalibData8 = rxNvmParP3[1];
  temp_var = 4294967296.0f;   /* 1 / 2^32 */
  QuantizedCalibData.par_p3 = (static_cast<double>(signedCalibData8) / temp_var);

  /* Read NVM_PAR_P2 data from the register */
  this->writeRead(nvmParP2MAddr, rxNvmParP2, writeReadSize);
  signedCalibData16 = (static_cast<int16_t>(rxNvmParP2[1]) << 8) & 0xFF00;
  uint8_t nvmParP2lAddr[2] = {0x38, 0x00};
  this->writeRead(nvmParP2lAddr, rxNvmParP2, writeReadSize);
  signedCalibData16 = signedCalibData16 | rxNvmParP2[1];
  temp_var = 536870912.0f; /* 1 / 2^29 */
  QuantizedCalibData.par_p2 = (static_cast<double>(signedCalibData16 - 16384) / temp_var);   /* 1 / 2^14 */

  /* Read NVM_PAR_P1 data from the register */
  this->writeRead(nvmParP1MAddr, rxNvmParP1, writeReadSize);
  signedCalibData16 = (static_cast<int16_t>(rxNvmParP1[1]) << 8) & 0xFF00;
  this->writeRead(nvmParP1lAddr, rxNvmParP1, writeReadSize);
  signedCalibData16 = signedCalibData16 | rxNvmParP1[1];
  temp_var = 1048576.0f; /* 1 / 2^20 */
  QuantizedCalibData.par_p1 = (static_cast<double>(signedCalibData16 - 16384) / temp_var);   /* 1 / 2^14 */

  /* Read NVM_PAR_T3 data from the register */
  this->writeRead(nvmParT3Addr, rxNvmParT3, writeReadSize);
  signedCalibData8 = rxNvmParT3[1];
  temp_var = 281474976710656.0f;   /* 1 / 2^48 */
  QuantizedCalibData.par_t3 = (static_cast<double>(signedCalibData8) / temp_var);

  /* Read NVM_PAR_T2 data from the register */
  this->writeRead(nvmParT2MAddr, rxNvmParT2, writeReadSize);
  unsignedCalibData16 = (static_cast<int16_t>(rxNvmParT2[1]) << 8) & 0xFF00;
  this->writeRead(nvmParT2lAddr, rxNvmParT2, writeReadSize);
  unsignedCalibData16 = unsignedCalibData16 | rxNvmParT2[1];
  temp_var = 1073741824.0f;   /* 1 / 2^30 */
  QuantizedCalibData.par_t2 = (static_cast<double>(unsignedCalibData16) / temp_var);

  /* Read NVM_PAR_T1 data from the register */
  this->writeRead(nvmParT1MAddr, rxNvmParT1, writeReadSize);
  unsignedCalibData16 = (static_cast<int16_t>(rxNvmParT1[1]) << 8) & 0xFF00;
  this->writeRead(nvmParT1lAddr, rxNvmParT1, writeReadSize);
  unsignedCalibData16 = unsignedCalibData16 | rxNvmParT1[1];
  temp_var = 0.00390625f;   /* 1 / 2^(-8) */
  QuantizedCalibData.par_t1 = (static_cast<double>(unsignedCalibData16) / temp_var);
}

SPIBMP388Status BMP388::setIIRFilter(bool iirFilterEnable, const IIRFilterCoefficient IIRFilterCoeff){
  /**
   * iirFilterData:
   * Byte 1 : write 0x1F register address for CONFIG controls the IIR filter coefficient.
   * */
  const uint8_t regDataSize = 1;
  uint8_t iirFilterData[2];
  iirFilterData[0] = 0x1F;
  if (iirFilterEnable == false){
    /* Byte 2 : filter coefficient is bypass mode */
    iirFilterData[1] = 0x00;
    return this->write( iirFilterData, regDataSize);
   }
   else{
     switch(IIRFilterCoeff){
       /* Byte 2 : filter coefficient is 1 */
       case IIRFilterCoefficient::iirFilter1:
                       iirFilterData[1] = 0x02;
                       return this->write( iirFilterData, regDataSize);
                       break;
       /* Byte 2 : filter coefficient is 3 */
       case IIRFilterCoefficient::iirFilter3:
                       iirFilterData[1] = 0x04;
                       return this->write( iirFilterData, regDataSize);
                       break;
       /* Byte 2 : filter coefficient is 7 */
       case IIRFilterCoefficient::iirFilter7:
                       iirFilterData[1] = 0x06;
                       return this->write( iirFilterData, regDataSize);
                       break;
       /* Byte 2 : filter coefficient is 15 */
       case IIRFilterCoefficient::iirFilter15:
                       iirFilterData[1] = 0x08;
                       return this->write( iirFilterData, regDataSize);
                       break;
       /* Byte 2 : filter coefficient is 31 */
       case IIRFilterCoefficient::iirFilter31:
                       iirFilterData[1] = 0x0A;
                       return this->write( iirFilterData, regDataSize);
                       break;
       /* Byte 2 : filter coefficient is 63 */
       case IIRFilterCoefficient::iirFilter63:
                       iirFilterData[1] = 0x0C;
                       return this->write( iirFilterData, regDataSize);
                       break;
       /* Byte 2 : filter coefficient is 127 */
       case IIRFilterCoefficient::iirFilter127:
                       iirFilterData[1] = 0x0E;
                       return this->write( iirFilterData, regDataSize);
                       break;

       /* Byte 2 : filter coefficient is 0 */
       default :
               iirFilterData[1] = 0x00;
               return this->write( iirFilterData, regDataSize);
               break;
         }
     }
}

SPIBMP388Status BMP388::spiReset() {
  /**
   * Byte 1 : write 0x7E address for CMD
   * Byte 2 : B6 for soft reset
   * */
  uint8_t resetCmd[2] = {0x7E, 0xB6};

  const uint8_t resetCmdSize = 2;

  return this->write(resetCmd, resetCmdSize);
}

SPIBMP388Status BMP388::setPowerCtrlSettings(const struct bmp388Settings setBMP388PwrControl)
{
  const uint8_t regDataSize = 2;
  uint8_t regData = 0x00;
  uint8_t txRegData[2];

  /* For pressure enable*/
  if (setBMP388PwrControl.pressureEnable == true){
    regData = regData | bmp388PressureEnable;
  }
  /* For temperature enable*/
  if (setBMP388PwrControl.temperatureEnable == true){
    regData = regData | bmp388TemperatureEnable;
  }
  /* For Mode select */
  if (setBMP388PwrControl.sleepMode == true){
      regData = regData | bmp388SleepMode;
    }
    else if (setBMP388PwrControl.ForcedMode == true){
      regData = regData | bmp388ForcedMode;
    }
    else if (setBMP388PwrControl.NormalMode == true){
      regData = regData | bmp388NormalMode;
   }
  /**
   * Byte 1 : write 0x1B register address for PWR_CTRL
   * Byte 2 : set the pressure, temperature and mode
   * */
   txRegData[0] = 0x1B;
   txRegData[1] = regData;

  return this->write( txRegData, regDataSize);
}

SPIBMP388Status BMP388::ReadTemperatureData(){
  /* Temporary variables to store the temperature sensor data */
  uint32_t tempratureXlsb;
  uint32_t tempratureLsb;
  uint32_t tempratureMsb;

  uint8_t rxbuf[2]={};
  uint8_t txbuf[2] = {0x00, 0x00};

  /* Update the register with 0x07 to read temperature XLSB */
  txbuf[0] = 0x07;
  if (this->writeRead(txbuf, rxbuf, 2) == SPIBMP388Status::ok){
    tempratureXlsb = ((uint32_t)rxbuf[1]) & 0x000000FF;

    /* Update the register with 0x08 to read temperature LSB */
    txbuf[0] = 0x08;
    if ( this->writeRead(txbuf, rxbuf, 2) == SPIBMP388Status::ok){
      tempratureLsb = ((uint32_t)rxbuf[1] << 8) & 0x0000FF00;

      /* Update the register with 0x09 to read temperature MSB */
      txbuf[0] = 0x09;
      if (this->writeRead(txbuf, rxbuf, 2) == SPIBMP388Status::ok){
        tempratureMsb = ((uint32_t)rxbuf[1] << 16) & 0x00FF0000;
        uncompensatedTemperature = (tempratureMsb | tempratureLsb | tempratureXlsb);
        return SPIBMP388Status::ok;
      }
    }
  }
  return SPIBMP388Status::readError;
}

SPIBMP388Status BMP388::ReadPressureData(){
  /* Temporary variables to store the pressure sensor data */
  uint32_t pressureXlsb;
  uint32_t pressureLsb;
  uint32_t pressureMsb;

  uint8_t rxbuf[2]={};
  uint8_t txbuf[2] = {0x00, 0x00};

  /* Update the register with 0x04 to read pressure XLSB */
  txbuf[0] = 0x04;
  if (this->writeRead(txbuf, rxbuf, 2) == SPIBMP388Status::ok){
    pressureXlsb = ((uint32_t)rxbuf[1]) & 0x000000FF;

    /* Update the register with 0x05 to read pressure LSB */
    txbuf[0] = 0x05;
    if ( this->writeRead(txbuf, rxbuf, 2) == SPIBMP388Status::ok){
      pressureLsb = ((uint32_t)rxbuf[1] << 8) & 0x0000FF00;

      /* Update the register with 0x06 to read pressure MSB */
      txbuf[0] = 0x06;
      if (this->writeRead(txbuf, rxbuf, 2) == SPIBMP388Status::ok){
        pressureMsb = ((uint32_t)rxbuf[1] << 16) & 0x00FF0000;
        uncompensatedPressure = (pressureMsb | pressureLsb | pressureXlsb);
        return SPIBMP388Status::ok;
      }
    }
  }
  return SPIBMP388Status::readError;
}

SPIBMP388Status BMP388::setTimeStandby(enum TimeStandby timeStandby){
  /**
   * Byte 1 : write 0x1D address for odr
   * Byte 2 : timeStandby for odr_sel
   * */
  const uint8_t regDataSize = 1;
  uint8_t odrCmd[2];
  odrCmd[0] = 0x1D;
   switch(timeStandby){
   /* odr_sel: Sampling Period 5ms */
   case TimeStandby::TimeStandby5Ms :
                   odrCmd[1] = 0x00;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 10ms */
   case TimeStandby::TimeStandby10Ms:
                   odrCmd[1] = 0x01;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 20ms */
   case TimeStandby::TimeStandby20Ms:
                   odrCmd[1] = 0x02;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 40ms */
   case TimeStandby::TimeStandby40Ms:
                   odrCmd[1] = 0x03;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 80ms */
   case TimeStandby::TimeStandby80Ms:
                   odrCmd[1] = 0x04;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 160ms */
   case TimeStandby::TimeStandby160Ms:
                   odrCmd[1] = 0x05;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 320ms */
   case TimeStandby::TimeStandby320Ms:
                   odrCmd[1] = 0x06;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 640ms */
   case TimeStandby::TimeStandby640Ms:
                   odrCmd[1] = 0x07;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 1280ms */
   case TimeStandby::TimeStandby1280Ms:
                   odrCmd[1] = 0x08;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 2560ms */
   case TimeStandby::TimeStandby2560Ms:
                   odrCmd[1] = 0x09;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 5120ms */
   case TimeStandby::TimeStandby5120Ms:
                   odrCmd[1] = 0x0A;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 10240ms */
   case TimeStandby::TimeStandby10240Ms:
                   odrCmd[1] = 0x0B;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 20480ms */
   case TimeStandby::TimeStandby20480Ms:
                   odrCmd[1] = 0x0C;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 40960ms */
   case TimeStandby::TimeStandby40960Ms:
                   odrCmd[1] = 0x0D;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 81920ms */
   case TimeStandby::TimeStandby81920Ms:
                   odrCmd[1] = 0x0E;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 163840ms */
   case TimeStandby::TimeStandby163840Ms:
                   odrCmd[1] = 0x0F;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 327680ms */
   case TimeStandby::TimeStandby327680Ms:
                   odrCmd[1] = 0x10;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 655360ms */
   case TimeStandby::TimeStandby655360Ms:
                   odrCmd[1] = 0x11;
                   return this->write( odrCmd, regDataSize);
                   break;
   /* odr_sel: Sampling Period 5ms */
   default :       odrCmd[1] = 0x00;
                   return this->write( odrCmd, regDataSize);
                   break;
   }
}

SPIBMP388Status BMP388::setOversamplingRegister(uint8_t presOversampling, uint8_t tempOversampling){
   /**
     * Byte 1 : write 0x1C address for osr
     * Byte 2 : pressure and temperature over sampling
     * */
    const uint8_t regDataSize = 1;
    uint8_t osrCmd[2];
    osrCmd[0] = 0x1C;
    osrCmd[1] = ((static_cast<uint8_t>(tempOversampling) << 3 )| static_cast<uint8_t>(presOversampling));
    return this->write( osrCmd, regDataSize);
}

SPIBMP388Status BMP388::disableFIFO(){
  /**
    * Byte 1 : write 0x17 address for FIFO CONFIG 1
    * Byte 2 : Disable FIFO mode
    * */
   const uint8_t regDataSize = 1;
   uint8_t disablefifoCmd[2];
   disablefifoCmd[0] = 0x17;
   disablefifoCmd[1] = 0x00;
   return this->write( disablefifoCmd, regDataSize);
  }

SPIBMP388Status BMP388::enableFIFO(bool tempEnable,
                                      bool pressEnable,
                                      bool timeEnable,
                                      bool stopOnFull){
  /**
    * Byte 1 : write 0x17 address for FIFO CONFIG 1
    * Byte 2 : Enable FIFO mode and Stop writing samples in FIFO when FIFO is full and
    *          Store pressure in FIFO data and Store temperature in FIFO data
    * */
  const uint8_t regDataSize = 1;
  uint8_t enablefifoCmd[2];
          enablefifoCmd[0] = 0x17;
          enablefifoCmd[1] = ((tempEnable << 4)  | (pressEnable << 3) | (timeEnable << 2) | (stopOnFull << 1)| 1);
          return this->write(enablefifoCmd, regDataSize);

 }

SPIBMP388Status BMP388::getFIFOLength() {
  /**
    * Byte 1 : write 0x12 register address for FIFO LENGTH 0
    * Byte 2 : 0x00 dummy data to read the lenght in rxbuf
    * */
  SPIBMP388Status returnStatus;
  const uint8_t regDataSize = 1;
  uint8_t fifoLengthCmd[2] = {0x12, 0x00};
  uint8_t rxbuf[]={};
  returnStatus = this->writeRead(fifoLengthCmd, rxbuf, regDataSize);
  fifoLength = rxbuf[1];
  return returnStatus;
}

SPIBMP388Status BMP388::readCompensateTemperature(float &compensateTemperature) {
  /* Temporary variables used for compensation */
  float partialData1, partialData2;

  if(this->ReadTemperatureData() != SPIBMP388Status::ok)
  {
    /* return error in reading temperature data */
      return SPIBMP388Status::readError;
  }

  /* Calibration data for temperature */
  partialData1 = static_cast<float>(uncompensatedTemperature) - QuantizedCalibData.par_t1;
  partialData2 = partialData1 * QuantizedCalibData.par_t2;

  /* Update the compensated temperature */
  compensateTemperature = partialData2 + ((partialData1 * partialData1) * QuantizedCalibData.par_t3);
  QuantizedCalibData.t_lin = compensateTemperature;

  return SPIBMP388Status::ok;
}

SPIBMP388Status BMP388::readCompensatePressure(float &compensatePressure) {
  /* Temporary variables used for compensation */
  float partialOut1, partialOut2, partialOut3;
  float partialData1, partialData2, partialData3;
  /* Read the pressure */
  if(this->ReadPressureData() != SPIBMP388Status::ok)
  {
    /* return error in reading pressure data */
      return SPIBMP388Status::readError;
  }

  /* Calibration data for pressure */
  partialData1 = QuantizedCalibData.par_p6 * QuantizedCalibData.t_lin;
  partialData2 = QuantizedCalibData.par_p7 * QuantizedCalibData.t_lin * QuantizedCalibData.t_lin;
  partialData3 = QuantizedCalibData.par_p8 * QuantizedCalibData.t_lin * QuantizedCalibData.t_lin * QuantizedCalibData.t_lin;
  partialOut1 = QuantizedCalibData.par_p5 + partialData1 + partialData2 + partialData3;

  partialData1 = QuantizedCalibData.par_p2 * QuantizedCalibData.t_lin;
  partialData2 = QuantizedCalibData.par_p3 * QuantizedCalibData.t_lin * QuantizedCalibData.t_lin;
  partialData3 = QuantizedCalibData.par_p4 * QuantizedCalibData.t_lin * QuantizedCalibData.t_lin * QuantizedCalibData.t_lin;
  partialOut2  = static_cast<float>(uncompensatedPressure) * (QuantizedCalibData.par_p1 +
                  partialData1 + partialData2 + partialData3);

  partialData1 = static_cast<float>(uncompensatedPressure) * static_cast<float>(uncompensatedPressure);
  partialData2 = QuantizedCalibData.par_p9 + QuantizedCalibData.par_p10 * QuantizedCalibData.t_lin;
  partialData3 = partialData1 * partialData2;

  partialOut3 = partialData3 + static_cast<float>(uncompensatedPressure) *
      static_cast<float>(uncompensatedPressure) * static_cast<float>(uncompensatedPressure) * QuantizedCalibData.par_p11;

  /* Update compensation pressure */
  compensatePressure = partialOut1 + partialOut2 + partialOut3;

  return SPIBMP388Status::ok;
}

}  // namespace BMP388
}  // namespace Driver
}  // namespace Pufferfish

