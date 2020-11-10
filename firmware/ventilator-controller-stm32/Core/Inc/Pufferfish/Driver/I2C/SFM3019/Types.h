/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * SFM3000.h
 *
 *      Author: March Boonyapaluk
 *
 *  A driver for the SFM3000 Low Pressure Drop Digital Flow Meter
 */

#pragma once

#include <cstdint>

#include "Pufferfish/Util/Bytes.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {
namespace SFM3019 {

enum class GasType : uint16_t { o2 = 0x3603, air = 0x3608, mixture = 0x3632 };

enum class Command : uint16_t {
  start_measure_o2 = static_cast<uint16_t>(GasType::o2),
  start_measure_air = static_cast<uint16_t>(GasType::air),
  start_measure_mixture = static_cast<uint16_t>(GasType::mixture),
  update_concentration = 0xe17d,
  reset_i2c_addr_ptr = 0xe000,
  stop_measure = 0x3ff9,
  set_averaging = 0x366a,
  read_conversion = 0x3661,
  reset = 0x0006,
  sleep = 0x3677,
  read_product_id = 0xe102
};

enum class UnitPrefix : uint16_t {
  n = 3,
  u = 4,
  m = 5,
  c = 6,
  d = 7,
  none = 8,
  da = 9,
  h = 10,
  k = 11,
  M = 12,  // NOLINT(readability-identifier-naming)
  G = 13   // NOLINT(readability-identifier-naming)
};
static const size_t unit_prefix_shift = 0;

enum class TimeBase : uint16_t {
  none = 0,
  per_us = 1,
  per_ms = 2,
  per_s = 3,
  per_min = 4,
  per_h = 5,
  per_day = 6
};
static const size_t time_base_shift = 4;

enum class Unit : uint16_t {
  norm_liter = 0,
  standard_liter_20deg = 1,
  standard_liter_15deg = 2,
  standard_liter_25deg = 3,
  liter = 8,
  gram = 9
};
static const size_t unit_shift = 8;

constexpr uint16_t make_flow_unit(UnitPrefix prefix, TimeBase time, Unit unit) {
  return (static_cast<uint16_t>(prefix) << unit_prefix_shift) +
         (static_cast<uint16_t>(time) << time_base_shift) +
         (static_cast<uint16_t>(unit) << unit_shift);
}

/**
 * All data in a reading from the Sensirion SFM3000 mass flow meter.
 */
struct Sample {
  int16_t raw_flow;
  float flow;
};

struct ConversionFactors {
  int16_t scale_factor{};
  int16_t offset{};
  uint16_t flow_unit{};
};

}  // namespace SFM3019
}  // namespace I2C
}  // namespace Driver
}  // namespace Pufferfish
