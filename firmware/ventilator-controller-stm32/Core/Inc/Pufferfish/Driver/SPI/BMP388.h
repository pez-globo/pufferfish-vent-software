/*
 * BMP388.h
 * 
 * Created on: Aug 29, 2020
 *      Author: pradip.jm
 */

#pragma once

#include "Pufferfish/HAL/STM32/HALSPIDevice.h"

namespace Pufferfish {
namespace Driver {
namespace BMP388 {

class SPIBMP388 {
public:
  SPIBMP388 (HAL::HALSPIDevice &spi):mSpi(spi){
   }

  /**
   * An outcome of performing an operation on BMP388 mode
   */
  enum class BMP388Mode{
    sleepMode = 0,
    normalMode,
    forcedMode
  };

  enum class IIRFilterCoefficient  {
    iirFilterOff = 0, /// Infinite Impulse Response (IIR) filter bit field in the configuration register
    iirFilter1,
    iirFilter3,
    iirFilter7,
    iirFilter15,
    iirFilter31,
    iirFilter63,
    iirFilter127
  };

  /**
   * @brief bmp388 sensor structure for temperature and pressure and mode settings.
   */
  struct bmp388Settings{
    bool preasureEnable;
    bool temperatureEnable;
    bool sleepMode;
    bool ForcedMode;
    bool NormalMode;
  };

  /**
   * @brief Register Trim Variables
   */
  struct bmp388RegCalibData {
    uint16_t par_t1;
    uint16_t par_t2;
    int8_t par_t3;
    int16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int8_t par_p4;
    uint16_t par_p5;
    uint16_t par_p6;
    int8_t par_p7;
    int8_t par_p8;
    int16_t par_p9;
    int8_t par_p10;
    int8_t par_p11;
    int64_t t_lin;
  };

  /**
  * @ Quantized Trim Variables
  */
  struct bmp388QuantizedCalibData {
    double par_t1;
    double par_t2;
    double par_t3;
    double par_p1;
    double par_p2;
    double par_p3;
    double par_p4;
    double par_p5;
    double par_p6;
    double par_p7;
    double par_p8;
    double par_p9;
    double par_p10;
    double par_p11;
    double t_lin;
  };

  /**
   * @brief  Gets the chip id
   * @param  chip_id updates the chip id of BMP388
   * @return SPIBMP388Status returus ok/readError
   */
  SPIBMP388Status getChipId(uint8_t &chip_id);

  /**
   * @brief  Enables/disables the setIIRFilter based on IIRFilter Coefficient
   * @param  iirFilterEnabel Enables/disables the IIRFilter
   * @param  IIRFilterCoeff input coefficient for IIR filer for enabling
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status setIIRFilter(bool iirFilterEnabel, IIRFilterCoefficient IIRFilterCoeff);

  /**
   * @brief  Sets the power control settings using PWR_CTRL reg
   * @param  bmp388Value structure contains the power control parameters
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status setPowerCtrlSettings(struct bmp388Settings bmp388Value);

  /**
   * @brief  Writes the data into the register
   * @param  buff buffer data to write
   * @param  count size of buffer
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status write(uint8_t *buff, size_t count);

  /**
   * @brief  Writes and read the data from the register
   * @param  txbuf data to write into DI pin
   * @param  rxbuf data read from DO pin
   * @return SPIBMP388Status returns ok/readError
   */
  SPIBMP388Status writeRead(uint8_t *txbuf, uint8_t *rxbuf, size_t count);

  /**
   * @brief  Resets the sensor device
   * @param  None
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status spiReset();

  /**
   * @brief  reads the CalibrationData data from NVM
   * @param  QuantizedCalibData updates the structure bmp388QuantizedCalibData
   * @return None
   */
  void ReadCalibrationData(struct bmp388QuantizedCalibData QuantizedCalibData);

  /**
   * @brief  reads the temperature data from sensor
   * @param  None
   * @return returns the temperature data
   */
  uint32_t ReadTemperatureData();

  /**
   * @brief  reads the pressure data from sensor
   * @param  None
   * @return returns the pressure data
   */
  uint32_t ReadPressureData();

 private:
       HAL::HALSPIDevice &mSpi;
       /*
        * TODO: Few variables still taken to update for other functionalities
        */
       static const uint8_t Bmp388ChipId                = 0x50;
       static const uint8_t bmp388PreasureEnable        = 0x01;
       static const uint8_t bmp388TempratureEnable      = 0x02;
       static const uint8_t bmp388SleepMode             = 0x00;
       static const uint8_t bmp388ForcedMode            = 0x20;
       static const uint8_t bmp388NormalMode            = 0x30;
       static const uint8_t BMP388_CHIP_ID_ADDR         = 0x00;
       static const uint8_t BMP388_ERR_REG_ADDR         = 0x02;
       static const uint8_t BMP388_SENS_STATUS_REG_ADDR = 0x03;
       static const uint8_t BMP388_Pre_DATA0_ADDR       = 0x04;
       static const uint8_t BMP388_Pre_DATA1_ADDR       = 0x05;
       static const uint8_t BMP388_Pre_DATA2_ADDR       = 0x06;
       static const uint8_t BMP388_Temp_DATA0_ADDR      = 0x07;
       static const uint8_t BMP388_Temp_DATA1_ADDR      = 0x08;
       static const uint8_t BMP388_Temp_DATA2_ADDR      = 0x09;
       static const uint8_t BMP388_EVENT_ADDR           = 0x10;
       static const uint8_t BMP388_INT_STATUS_REG_ADDR  = 0x11;
       static const uint8_t BMP388_FIFO_0_LENGTH_ADDR   = 0x12;
       static const uint8_t BMP388_FIFO_1_LENGTH_ADDR   = 0x13;
       static const uint8_t BMP388_FIFO_DATA_ADDR       = 0x14;
       static const uint8_t BMP388_FIFO_WM_ADDR         = 0x15;
       static const uint8_t BMP388_FIFO_CONFIG_1_ADDR   = 0x17;
       static const uint8_t BMP388_FIFO_CONFIG_2_ADDR   = 0x18;
       static const uint8_t BMP388_INT_CTRL_ADDR        = 0x19;
       static const uint8_t BMP388_IF_CONF_ADDR         = 0x1A;
       static const uint8_t bmp388PrCtrlAddr            = 0x1B;
       static const uint8_t BMP388_OSR_ADDR             = 0X1C;
       static const uint8_t BMP388_CALIB_DATA_ADDR      = 0x31;
       static const uint8_t BMP388_CMD_ADDR             = 0x7E;
       static const uint8_t E_OK                        = 0;

       static const uint8_t Bmp388PressEnSel      = 0x02;
       static const uint8_t Bmp388TempEnSel       = 0x04;
       static const uint8_t Bmp388DrdyEnSel       = 0x08;
       static const uint8_t Bmp388PressOsSel      = 0x10;
       static const uint8_t Bmp388TempOS_SEL      = 0x20;
       static const uint8_t Bmp388IIRFilterSel    = 0x40;
       static const uint8_t Bmp388OdrSel          = 0x80;
       static const uint16_t Bmp388OutputModeSel   = 0x100;
       static const uint16_t Bmp388LevelSel        = 0x200;
       static const uint32_t Bmp388LatchSel        = 0x10000;
       static const uint32_t Bmp388AllSettings     = 0x7FF;

};

}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish
