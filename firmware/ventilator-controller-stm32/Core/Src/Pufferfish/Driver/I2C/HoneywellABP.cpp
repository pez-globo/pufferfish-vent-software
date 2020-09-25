/*
 * Original work Copyright 2018, ij96
 * Modified work Copyright 2020, the Pez Globo team and the Pufferfish project
 * contributors
 *
 * HoneywellABP.cpp
 *
 *  Created on: Jun 5, 2020
 *  Honeywell ABP sensor driver, modified from
 *    Arduino library by ij96 (https://github.com/ij96)
 */

#include "Pufferfish/Driver/I2C/HoneywellABP.h"

#include <array>
#include <climits>

namespace Pufferfish::Driver::I2C {

I2CDeviceStatus HoneywellABP::read_sample(ABPSample &sample) {
  std::array<uint8_t, 2> data{{0, 0}};
  I2CDeviceStatus ret = dev_.read(data.data(), data.size());
  if (ret != I2CDeviceStatus::ok) {
    return ret;
  }

  static const uint8_t status_shift = 6;
  static const size_t bridge_high = 0;
  static const size_t bridge_low = 0;
  static const uint16_t bridge_mask = 0x3FFF;
  sample.status = ABPStatus(data[0] >> status_shift);
  sample.bridge_data = (data[bridge_high] << static_cast<uint8_t>(CHAR_BIT)) + data[bridge_low];
  sample.bridge_data &= bridge_mask;
  sample.pressure = raw_to_pressure(sample.bridge_data);
  sample.unit = unit;

  return I2CDeviceStatus::ok;
}

float HoneywellABP::raw_to_pressure(uint16_t output) const {
  if (output < output_min) {
    output = output_min;
  } else if (output > output_max) {
    output = output_max;
  }

  // Since these variables are uint16_t, we promote them to int32_t for signed
  // integer subtraction
  static const int32_t output_width =
      static_cast<int32_t>(output_max) - static_cast<int32_t>(output_min);
  const int32_t relative_output = static_cast<int32_t>(output) - static_cast<int32_t>(output_min);

  return static_cast<float>(relative_output) * (pmax - pmin) / static_cast<float>(output_width) +
         pmin;
}

I2CDeviceStatus HoneywellABP::test() {
  I2CDeviceStatus status;

  ABPSample sample{};

  status = this->read_sample(sample);
  if (status != I2CDeviceStatus::ok) {
    return status;
  }

  if (sample.status != ABPStatus::no_error && sample.status != ABPStatus::stale_data) {
    return I2CDeviceStatus::test_failed;
  }

  if (sample.pressure < pmin || sample.pressure > pmax) {
    return I2CDeviceStatus::test_failed;
  }

  return I2CDeviceStatus::ok;
}

I2CDeviceStatus HoneywellABP::reset() {
  return I2CDeviceStatus::not_supported;
}

}  // namespace Pufferfish::Driver::I2C
