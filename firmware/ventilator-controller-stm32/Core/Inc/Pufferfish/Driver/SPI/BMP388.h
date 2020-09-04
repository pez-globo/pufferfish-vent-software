/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 * 
 *      Author: pradip.jm
 */

#pragma once

#include "Pufferfish/HAL/Interfaces/SPIDevice.h"

namespace Pufferfish {
namespace Driver {
namespace SPI {

class BMP388 {
public:
  /**
   * @brief bmp388 sensor structure for temperature and pressure and mode settings.
   */
  struct bmp388Settings{
    bool pressureEnable;
    bool temperatureEnable;
    bool sleepMode;
    bool ForcedMode;
    bool NormalMode;
  };

  /**
   *
   */
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
  * @ Time standby in the Output Data Rate (ODR) register
  */
  enum class TimeStandby {
    TimeStandby5Ms,
    TimeStandby10Ms,
    TimeStandby20Ms,
    TimeStandby40Ms,
    TimeStandby80Ms,
    TimeStandby160Ms,
    TimeStandby320Ms,
    TimeStandby640Ms,
    TimeStandby1280Ms,
    TimeStandby2560Ms,
    TimeStandby5120Ms,
    TimeStandby10240Ms,
    TimeStandby20480Ms,
    TimeStandby40960Ms,
    TimeStandby81920Ms,
    TimeStandby163840Ms,
    TimeStandby327680Ms,
    TimeStandby655360Ms
  };

  BMP388 (HAL::SPIDevice &spi):mSpi(spi){
   }
  /**
   * @brief  Settings
   * @param
   * @return SPIBMP388Status returns ok/writeError
   */
   SPIBMP388Status bmp388ConfigSettings(const bool enableIIRFilter,
                                    const IIRFilterCoefficient IIRFilterCoeff,
                                    const struct bmp388Settings setBMP388PwrControl);

  /**
   * @brief  Get the chip id
   * @param  Update the Chip Id
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status getChipId(uint8_t &chipId);

  /**
   * @brief  Enables/disables the setIIRFilter based on IIRFilter Coefficient
   * @param  iirFilterEnabel Enables/disables the IIRFilter
   * @param  IIRFilterCoeff input coefficient for IIR filer for enabling
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status setIIRFilter(bool iirFilterEnable, const IIRFilterCoefficient IIRFilterCoeff);

  /**
   * @brief  Sets the power control settings using PWR_CTRL reg
   * @param  bmp388Value structure contains the power control parameters
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status setPowerCtrlSettings(const struct bmp388Settings setBMP388PwrControl);

  /**
   * @brief  Resets the sensor device
   * @param  None
   * @return SPIBMP388Status returns ok/writeError
   */
  SPIBMP388Status spiReset();

  /**
   * @brief  reads the temperature data from sensor
   * @param  None
   * @return returns the temperature data
   */
  SPIBMP388Status ReadTemperatureData();

  /**
   * @brief  reads the pressure data from sensor
   * @param  None
   * @return returns the pressure data
   */
  SPIBMP388Status ReadPressureData();

  /**
   * @brief  Set the time standby measurement interval
   * @param  5, 62, 125, 250, 500ms, 1s, 2s, 4s
   * @return None
   */
  SPIBMP388Status setTimeStandby(enum TimeStandby timeStandby);

  /**
   * @brief  Set the pressure and temperature over sampling
   * @param  over sampling for pressure : OFF | X1 | X2 | X4 | X8 | X16 | X32
   * @param  over sampling for temperature : OFF | X1 | X2 | X4 | X8 | X16 | X32
   * @return SPIBMP388Status returns OK/writeError
   */
  SPIBMP388Status setOversamplingRegister(uint8_t presOversampling, uint8_t tempOversampling);

  /**
   * @brief  Disable the FIFO
   * @param  None
   * @return SPIBMP388Status returns OK/writeError
   */
  SPIBMP388Status disableFIFO();

  /**
   * @brief  Enable the FIFO
   * @param  tempEnable
   * @param  pressEnable
   * @param  timeEnable
   * @param  stopOnFull
   * @param  dataSelect
   * @return SPIBMP388Status returns OK/writeError
   */
  SPIBMP388Status enableFIFO(bool tempEnable,
                            bool pressEnable,
                            bool timeEnable,
                            bool stopOnFull);
  /**
   * @brief  Get the FIFO length
   * @param  address of FIFO length variable
   * @return SPIBMP388Status returns OK/writeError
   */
  SPIBMP388Status getFIFOLength();

  /**
   * @brief  To get the compensate temperature data
   * @param  compensateTemperature updated with the compensate temperature
   * @return SPIBMP388Status ok/readError
   */
  SPIBMP388Status readCompensateTemperature(float &compensateTemperature);

  /**
   * @brief  To get the compensate pressure data
   * @param  compensatePressure updated with the compensate pressure
   * @return SPIBMP388Status ok/readError
   */
  SPIBMP388Status readCompensatePressure(float &compensatePressure);

 private:

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
   * @brief  reads the CalibrationData data from NVM
   * @param  QuantizedCalibData updates the structure bmp388QuantizedCalibData
   * @return None
   */
  void ReadCalibrationData();

  /* mSpi object is updated by constructor used to read/write data through SPI */
  HAL::SPIDevice &mSpi;

  /* QuantizedCalibData structure is updated from register used for
   * pressure and temperature compensation */
  struct bmp388QuantizedCalibData QuantizedCalibData;

  /* Uncompensated temperature read from sensor */
  uint32_t uncompensatedTemperature;

  /* Uncompensated pressure read from sensor */
  uint32_t uncompensatedPressure;

  /* Fifo buffer length of the BMP388 */
  uint16_t fifoLength;

  static const uint8_t Bmp388ChipId = 0x50;
  static const uint8_t bmp388PressureEnable = 0x01;
  static const uint8_t bmp388TemperatureEnable = 0x02;
  static const uint8_t bmp388SleepMode = 0x00;
  static const uint8_t bmp388ForcedMode = 0x20;
  static const uint8_t bmp388NormalMode = 0x30;

};

}  // namespace SPI
}  // namespace Driver
}  // namespace Pufferfish
