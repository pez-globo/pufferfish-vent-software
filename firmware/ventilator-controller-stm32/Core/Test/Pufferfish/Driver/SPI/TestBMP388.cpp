/// SPIFlash.cpp
/// Unit tests to confirm the behavior of SPIFlash checks.

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

#include "catch2/catch.hpp"
#include "Pufferfish/HAL/Mock/MockSPIDevice.h"
#include "Pufferfish/Driver/SPI/BMP388.h"

namespace PF = Pufferfish;

SCENARIO("Validate the raw data ", "[BMP388]") {
  GIVEN("Raw data") {
    const uint8_t rx_data[8] = {0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
    PF::Driver::SPI::RawSensorData sample;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Invoke set_write_read to write raw data */
    mockSPiDevice.set_write_read(rx_data, 8);
    WHEN("Invoke read_raw_data to read the raw data from sensor") {
      Spi_device_status = test_bmp388.read_raw_data(sample);
      THEN("SPI device shall return ok") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::ok);
        REQUIRE(sample.data0 == rx_data[2]);
        REQUIRE(sample.data1 == rx_data[3]);
        REQUIRE(sample.data2 == rx_data[4]);
        REQUIRE(sample.data3 == rx_data[5]);
        REQUIRE(sample.data4 == rx_data[6]);
        REQUIRE(sample.data5 == rx_data[7]);
      }
    }
  }

  GIVEN("Raw data is not able to read from sensor - read_error") {
    const uint8_t rx_data[8] = {0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
    PF::Driver::SPI::RawSensorData sample;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Invoke set_write_read to write raw data */
    mockSPiDevice.set_write_read(rx_data, 8);
    mockSPiDevice.set_return_status(PF::SPIDeviceStatus::read_error);
    WHEN("Invoke read_raw_data to read the raw data from sensor") {
      Spi_device_status = test_bmp388.read_raw_data(sample);
      THEN("SPI device shall return read error") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::read_error);
      }
    }
  }
}

SCENARIO("Validate the pressure ", "[BMP388]") {
  GIVEN("Pressure") {
    const uint8_t tx_buf[5] = {0x00, 0x00, 0x00, 0x01, 0x02};
    uint32_t buf[3] = {0x00};
    uint32_t pressure;

    buf[0] = static_cast<uint32_t>(tx_buf[2]);
    buf[1] = static_cast<uint32_t>(tx_buf[3] << 8);
    buf[2] = static_cast<uint32_t>(tx_buf[4] << 16);
    uint32_t expected_pressure = buf[0] | buf[1] | buf[2];

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Set pressure */
    mockSPiDevice.set_write_read(tx_buf, 5);
    WHEN("Invoke raw_pressure to read the pressure") {
      Spi_device_status = test_bmp388.raw_pressure(pressure);
      THEN("SPI device shall return ok") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::ok);
        REQUIRE(expected_pressure == pressure);
      }
    }
  }

  GIVEN("Pressure  is not able to read from sensor - read_error") {
    const uint8_t tx_buf[5] = {0x00, 0x00, 0x00, 0x01, 0x02};
    uint32_t pressure;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Set pressure */
    mockSPiDevice.set_write_read(tx_buf, 5);
    mockSPiDevice.set_return_status(PF::SPIDeviceStatus::read_error);
    WHEN("Invoke raw_pressure to read the pressure") {
      Spi_device_status = test_bmp388.raw_pressure(pressure);
      THEN("SPI device shall return read error") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::read_error);
      }
    }
  }
}

SCENARIO("Validate the temperature ", "[BMP388]") {
  GIVEN("Temperature") {
    const uint8_t tx_buf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    uint32_t buf[3] = {0x00};
    uint32_t temperature;

    buf[0] = static_cast<uint32_t>(tx_buf[5]);
    buf[1] = static_cast<uint32_t>(tx_buf[6] << 8);
    buf[2] = static_cast<uint32_t>(tx_buf[7] << 16);
    uint32_t expected_temperature = buf[0] | buf[1] | buf[2];

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Set temperature */
    mockSPiDevice.set_write_read(tx_buf, 8);
    WHEN("Invoke raw_temperature to read the temperature") {
      Spi_device_status = test_bmp388.raw_temperature(temperature);
      THEN("SPI device shall return ok") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::ok);
        REQUIRE(expected_temperature == temperature);
      }
    }
  }

  GIVEN("Temperature is not able to read from sensor - read_error") {
    const uint8_t tx_buf[8] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    uint32_t temperature;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Set temperature */
    mockSPiDevice.set_write_read(tx_buf, 8);
    mockSPiDevice.set_return_status(PF::SPIDeviceStatus::read_error);
    WHEN("Invoke raw_temperature to read the temperature") {
      Spi_device_status = test_bmp388.raw_temperature(temperature);
      THEN("SPI device shall return read error") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::read_error);
      }
    }
  }
}

SCENARIO("Read calibration data ", "[BMP388]") {
  GIVEN("Calibration data") {
    const uint8_t tx_buf[23] = {0x01};
    PF::Driver::SPI::TrimValues data;
    uint16_t temp1 = static_cast<uint16_t>(tx_buf[3]) << 8;
    uint16_t temp2 = static_cast<uint16_t>(tx_buf[5]) << 8;
    uint16_t pres1 = static_cast<uint16_t>(tx_buf[8] << 8;
    uint16_t pres2 = static_cast<uint16_t>(tx_buf[10] << 8;
    uint16_t pres9 = static_cast<uint16_t>(tx_buf[20] << 8;

    uint16_t temperature1 = temp1 | static_cast<uint16_t>(tx_buf[2]);
    uint16_t temperature2 = temp2 | static_cast<uint16_t>(tx_buf[4]);
    int8_t temperature3 = static_cast<int8_t>(tx_buf[6]);

    int16_t pressure1 = static_cast<int16_t>(pres1) | static_cast<uint16_t>(tx_buf[7]);
    int16_t pressure2 = static_cast<int16_t>(pres2) | static_cast<uint16_t>(tx_buf[9]);
    int8_t pressure3 = static_cast<int8_t>(tx_buf[11]);
    int8_t pressure4 = static_cast<int8_t>(tx_buf[12]);
    uint16_t pressure5 = static_cast<uint16_t>(tx_buf[14] << 8) | static_cast<uint16_t>(tx_buf[13]);
    uint16_t pressure6 = static_cast<uint16_t>(tx_buf[16] << 8) | static_cast<uint16_t>(tx_buf[15]);
    int8_t pressure7 = static_cast<int8_t>(tx_buf[17]);
    int8_t pressure8 = static_cast<int8_t>(tx_buf[18]);
    int16_t pressure9 = static_cast<int16_t>(pres9) | static_cast<uint16_t>(tx_buf[19]);
    int8_t pressure10 = static_cast<int8_t>(tx_buf[21]);
    int8_t pressure11 = static_cast<int8_t>(tx_buf[22]);

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Send compensate temperature */
    mockSPiDevice.set_write_read(tx_buf, 23);
    WHEN("Invoke read_calibration_data to read the calibration data") {
      Spi_device_status = test_bmp388.read_calibration_data(data);
      THEN("SPI device shall return ok") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::ok);
        REQUIRE(data.par_t1 == temperature1);
        REQUIRE(data.par_t2 == temperature2);
        REQUIRE(data.par_t3 == temperature3);
        REQUIRE(data.par_p1 == pressure1);
        REQUIRE(data.par_p2 == pressure2);
        REQUIRE(data.par_p3 == pressure3);
        REQUIRE(data.par_p4 == pressure4);
        REQUIRE(data.par_p5 == pressure5);
        REQUIRE(data.par_p6 == pressure6);
        REQUIRE(data.par_p7 == pressure7);
        REQUIRE(data.par_p8 == pressure8);
        REQUIRE(data.par_p9 == pressure9);
        REQUIRE(data.par_p10 == pressure10);
        REQUIRE(data.par_p11 == pressure11);
      }
    }
  }

  GIVEN("Calibration data is not able to read from sensor - read_error") {
    const uint8_t tx_buf[23] = {0x01};
    PF::Driver::SPI::TrimValues data;
    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Send compensate temperature */
    mockSPiDevice.set_write_read(tx_buf, 23);
    mockSPiDevice.set_return_status(PF::SPIDeviceStatus::read_error);
    WHEN("Invoke read_calibration_data to read the calibration data") {
      Spi_device_status = test_bmp388.read_calibration_data(data);
      THEN("SPI device shall return read error") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::read_error);
      }
    }
  }
}

SCENARIO("Read calculated calibration coefficients ", "[BMP388]") {
  GIVEN("Calibration coefficients") {
    const uint8_t tx_buf[23] = {0x02};
    PF::Driver::SPI::CalibrationData data;
    uint16_t temp1 = static_cast<uint16_t>(tx_buf[3]) << 8;
    uint16_t temp2 = static_cast<uint16_t>(tx_buf[5]) << 8;
    uint16_t pres1 = static_cast<uint16_t>(tx_buf[8] << 8;
    uint16_t pres2 = static_cast<uint16_t>(tx_buf[10] << 8;
    uint16_t pres9 = static_cast<uint16_t>(tx_buf[20] << 8;

    double temperature1 = static_cast<double>(temp1) | static_cast<uint16_t>(tx_buf[2]) / pow(2, -8);
    double temperature2 = static_cast<double>(temp2) | static_cast<uint16_t>(tx_buf[4]) / pow(2, 30);
    double temperature3 = static_cast<double>(static_cast<int8_t>(tx_buf[6])) / pow(2, 48);

    double pressure1 = static_cast<double>(static_cast<int16_t>(pres1) | static_cast<uint16_t>(tx_buf[7])) - pow(2, 14) / powf(2, 20);
    double pressure2 = static_cast<double>(static_cast<int16_t>(pres2) | static_cast<uint16_t>(tx_buf[9])) - pow(2, 14) / powf(2, 29);
    double pressure3 = static_cast<double>(static_cast<int8_t>(tx_buf[11])) / pow(2, 32);
    double pressure4 = static_cast<double>(static_cast<int8_t>(tx_buf[12])) / pow(2, 37);
    double pressure5 = static_cast<double>(static_cast<uint16_t>(tx_buf[14] << 8) | static_cast<uint16_t>(tx_buf[13])) / pow(2, -3);
    double pressure6 = static_cast<double>(static_cast<uint16_t>(tx_buf[16] << 8) | static_cast<uint16_t>(tx_buf[15])) / pow(2, 6);
    double pressure7 = static_cast<double>(static_cast<int8_t>(tx_buf[17])) / pow(2, 8);
    double pressure8 = static_cast<double>(static_cast<int8_t>(tx_buf[18])) / pow(2, 15);
    double pressure9 = static_cast<double>(static_cast<int16_t>(pres9) | static_cast<uint16_t>(tx_buf[19])) / pow(2, 48);
    double pressure10 = static_cast<double>(static_cast<int8_t>(tx_buf[21])) / pow(2, 48);
    double pressure11 = static_cast<double>(static_cast<int8_t>(tx_buf[22])) / pow(2, 65);

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Send compensate temperature */
    mockSPiDevice.set_write_read(tx_buf, 23);
    WHEN("Invoke read_compensate_temperature to read the compensated temperature") {
      Spi_device_status = test_bmp388.calc_calibration_coefficient(data);
      THEN("SPI device shall return ok") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::ok);
        REQUIRE(data.par_t1 == temperature1);
        REQUIRE(data.par_t2 == temperature2);
        REQUIRE(data.par_t3 == temperature3);
        REQUIRE(data.par_p1 == pressure1);
        REQUIRE(data.par_p2 == pressure2);
        REQUIRE(data.par_p3 == pressure3);
        REQUIRE(data.par_p4 == pressure4);
        REQUIRE(data.par_p5 == pressure5);
        REQUIRE(data.par_p6 == pressure6);
        REQUIRE(data.par_p7 == pressure7);
        REQUIRE(data.par_p8 == pressure8);
        REQUIRE(data.par_p9 == pressure9);
        REQUIRE(data.par_p10 == pressure10);
        REQUIRE(data.par_p11 == pressure11);
      }
    }
  }

  GIVEN("Calibration coefficients is not able to read from sensor - read_error") {
    const uint8_t tx_buf[23] = {0x03};
    PF::Driver::SPI::CalibrationData data;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Send compensate temperature */
    mockSPiDevice.set_write_read(tx_buf, 23);
    mockSPiDevice.set_return_status(PF::SPIDeviceStatus::read_error);
    WHEN("Invoke read_compensate_temperature to read the compensated temperature") {
      Spi_device_status = test_bmp388.calc_calibration_coefficient(data);
      THEN("SPI device shall return read error") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::read_error);
      }
    }
  }
}

SCENARIO("Read compensate temperature ", "[BMP388]") {
  GIVEN("Compensated temperature data") {
    const uint8_t tx_buf[23] = {0x0A};
    uint32_t buf[3] = {0x00};
    double compensated_temperature;

    buf[0] = static_cast<uint32_t>(tx_buf[5]);
    buf[1] = static_cast<uint32_t>(tx_buf[6] << 8);
    buf[2] = static_cast<uint32_t>(tx_buf[7] << 16);
    uint32_t temperature = (buf[0] | buf[1] | buf[2]);

    uint16_t temp1 = static_cast<uint16_t>(tx_buf[3]) << 8;
    uint16_t temp2 = static_cast<uint16_t>(tx_buf[5]) << 8;

    double part1 = static_cast<double>(temp1) | static_cast<uint16_t>(tx_buf[2]) / pow(2, -8);
    double part2 = static_cast<double>(temp2) | static_cast<uint16_t>(tx_buf[4]) / pow(2, 30);
    double part3 = static_cast<double>(static_cast<int8_t>(tx_buf[6])) / pow(2, 48);
    double const data1 = static_cast<double>(temperature - part1);
    double const data2 = static_cast<double>(data1 * part2);
    double expected_compensated_temperature = (data2 + (data1 * data1) * part3);

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Send compensate temperature */
    mockSPiDevice.set_write_read(tx_buf, 23);
    WHEN("Invoke read_compensate_temperature to read the compensated temperature") {
      Spi_device_status = test_bmp388.read_compensate_temperature(compensated_temperature);
      THEN("SPI device shall return ok") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::ok);
        REQUIRE(expected_compensated_temperature == compensated_temperature);
      }
    }
  }

  GIVEN("Compensated temperature is not able to read from sensor - read_error") {
    const uint8_t tx_buf[23] = {0x08};
    double compensated_temperature;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Send compensate temperature */
    mockSPiDevice.set_write_read(tx_buf, 23);
    mockSPiDevice.set_return_status(PF::SPIDeviceStatus::read_error);
    WHEN("Invoke read_compensate_temperature to read the compensated temperature") {
      Spi_device_status = test_bmp388.read_compensate_temperature(compensated_temperature);
      THEN("SPI device shall return read error") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::read_error);
      }
    }
  }
}

SCENARIO("Read compensate pressure ", "[BMP388]") {
  GIVEN("Compensated pressure data") {
    const uint8_t tx_buf[23] = {0x01};
    uint32_t buf[6] = {0x00};
    double compensated_pressure;
    buf[0] = static_cast<uint32_t>(tx_buf[2]);
    buf[1] = static_cast<uint32_t>(tx_buf[3] << 8);
    buf[2] = static_cast<uint32_t>(tx_buf[4] << 16);
    uint32_t pressure = buf[0] | buf[1] | buf[2];

    buf[3] = static_cast<uint32_t>(tx_buf[5]);
    buf[4] = static_cast<uint32_t>(tx_buf[6] << 8);
    buf[5] = static_cast<uint32_t>(tx_buf[7] << 16);
    uint32_t temperature = buf[3] | buf[4] | buf[5];

    uint16_t temp1 = static_cast<uint16_t>(tx_buf[3]) << 8;
    uint16_t temp2 = static_cast<uint16_t>(tx_buf[5]) << 8;
    uint16_t pres1 = static_cast<uint16_t>(tx_buf[8] << 8;
    uint16_t pres2 = static_cast<uint16_t>(tx_buf[10] << 8;
    uint16_t pres9 = static_cast<uint16_t>(tx_buf[20] << 8;

    double part1 = static_cast<double>(temp1) | static_cast<uint16_t>(tx_buf[2]) / pow(2, -8);
    double part2 = static_cast<double>(temp2) | static_cast<uint16_t>(tx_buf[4]) / pow(2, 30);
    double part3 = static_cast<double>(static_cast<int8_t>(tx_buf[6])) / pow(2, 48);
    double data1 = static_cast<double>(temperature - part1);
    double data2 = static_cast<double>(data1 * part2);
    double const compensated_temperature = (data2 + (data1 * data1) * part3);

    part1 = static_cast<double>(static_cast<int16_t>(pres1) | static_cast<uint16_t>(tx_buf[7])) - pow(2, 14) / powf(2, 20);
    part2 = static_cast<double>(static_cast<int16_t>(pres2) | static_cast<uint16_t>(tx_buf[9])) - pow(2, 14) / powf(2, 29);
    part3 = static_cast<double>(static_cast<int8_t>(tx_buf[11])) / pow(2, 32);
    double part4 = static_cast<double>(static_cast<int8_t>(tx_buf[12])) / pow(2, 37);
    double part5 = static_cast<double>(static_cast<uint16_t>(tx_buf[14] << 8) | static_cast<uint16_t>(tx_buf[13])) / pow(2, -3);
    double part6 = static_cast<double>(static_cast<uint16_t>(tx_buf[16] << 8) | static_cast<uint16_t>(tx_buf[15])) / pow(2, 6);
    double part7 = static_cast<double>(static_cast<int8_t>(tx_buf[17])) / pow(2, 8);
    double part8 = static_cast<double>(static_cast<int8_t>(tx_buf[18])) / pow(2, 15);
    double part9 = static_cast<double>(static_cast<int16_t>(pres9) | static_cast<uint16_t>(tx_buf[19])) / pow(2, 48);
    double part10 = static_cast<double>(static_cast<int8_t>(tx_buf[21])) / pow(2, 48);
    double part11 = static_cast<double>(static_cast<int8_t>(tx_buf[22])) / pow(2, 65);

    data1 = part6 * compensated_temperature;
    data2 = part7 * pow(compensated_temperature, 2);
    double data3 = part8 * pow(compensated_temperature, 3);
    double const temp = part1 + data1 + data2 + data3;
    double const temp1 = part5 + data1 + data2 + data3;
    data1 = part2 * compensated_temperature;
    data2 = part3 * pow(compensated_temperature, 2);
    data3 = part4 * pow(compensated_temperature, 3);
    double const temp2 = static_cast<double>(pressure) * temp;
    data1 = static_cast<double>(pressure * pressure);
    data2 = part9 + part10 * compensated_temperature;
    data3 = data1 * data2;
    double const temp3 = data3 + pow(pressure, 3) * part11;
    double expected_compensated_pressure = (temp1 + temp2 + temp3) / 100.0;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;

    /* Send compensate pressure */
    mockSPiDevice.set_write_read(tx_buf, 23);
    WHEN("Invoke read_compensate_pressure to read the compensated pressure") {
      Spi_device_status = test_bmp388.read_compensate_pressure(compensated_pressure);
      THEN("SPI device shall return ok") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::ok);
        REQUIRE(expected_compensated_pressure == compensated_pressure);
      }
    }
  }

  GIVEN(" Compensated pressure is not able to read from sensor - read_error ") {
    const uint8_t tx_buf[23] = {0x01};
    double compensated_pressure;

    PF::HAL::MockSPIDevice mockSPiDevice;
    PF::Driver::SPI::BMP388 test_bmp388(mockSPiDevice);
    PF::SPIDeviceStatus Spi_device_status;
    /* Send compensate pressure */
    mockSPiDevice.set_write_read(tx_buf, 23);
    mockSPiDevice.set_return_status(PF::SPIDeviceStatus::read_error);
    WHEN("Invoke read_compensate_pressure to read the compensated pressure") {
      Spi_device_status = test_bmp388.read_compensate_pressure(compensated_pressure);
      THEN("SPI device shall return read error") {
        REQUIRE(Spi_device_status == PF::SPIDeviceStatus::read_error);
      }
    }
  }
}
