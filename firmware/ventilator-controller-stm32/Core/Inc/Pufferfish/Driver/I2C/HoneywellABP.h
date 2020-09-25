/*
 * Original work Copyright 2018, ij96
 * Modified work Copyright 2020, the Pez Globo team and the Pufferfish project
 * contributors
 *
 *  Honeywell ABP sensor driver, modified from
 *    Arduino library by ij96 (https://github.com/ij96)
 */

#pragma once

#include "Pufferfish/Driver/Testable.h"
#include "Pufferfish/HAL/HAL.h"
#include "Pufferfish/Types.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {

/**
 * Configuration parameters for HoneywellABP constructor
 */
struct ABPConfig {
  uint16_t i2c_addr;
  float pmin;
  float pmax;
  PressureUnit unit;
};

/**
 * Status code reported by the Honeywell ABP pressure sensor as part of its
 * reading
 */
enum class ABPStatus {
  no_error = 0,
  command_mode = 1,
  stale_data = 2,
  diagnostic = 3,
};

/**
 * All data in a reading from the Honeywell ABP pressure sensor.
 */
struct ABPSample {
  ABPStatus status;
  uint16_t bridge_data;
  float pressure;
  PressureUnit unit;
};

static const ABPConfig abpxxxx001pg2a3 = {0x28, 0.0, 1.0, PressureUnit::psi};
static const ABPConfig abpxxxx005pg2a3 = {0x28, 0.0, 5.0, PressureUnit::psi};
static const ABPConfig abpxxxx030pg2a3 = {0x28, 0.0, 30.0, PressureUnit::psi};

/**
 * Driver for Honeywell ABP pressure sensor
 */
class HoneywellABP : public Testable {
 public:
  HoneywellABP(HAL::I2CDevice &dev, const ABPConfig &cfg)
      : dev_(dev), pmin(cfg.pmin), pmax(cfg.pmax), unit(cfg.unit) {}

  [[nodiscard]] float raw_to_pressure(uint16_t output) const;

  /**
   * Reads out the pressure from the sensor
   * @param sample[out] the sensor reading; only valid on success
   * @return ok on success, error code otherwise
   */
  I2CDeviceStatus read_sample(ABPSample &sample);

  I2CDeviceStatus test() override;
  I2CDeviceStatus reset() override;

 private:
  Pufferfish::HAL::I2CDevice &dev_;

  // pressure range (refer to datasheet)
  const float pmin;  // minimum pressure
  const float pmax;  // maximum pressure

  // sensor 14-bit output range
  const uint16_t output_min = 0x0666;  // 10% of 2^14
  const uint16_t output_max = 0x399A;  // 90% of 2^14
  const PressureUnit unit;
};

}  // namespace I2C
}  // namespace Driver
}  // namespace Pufferfish
