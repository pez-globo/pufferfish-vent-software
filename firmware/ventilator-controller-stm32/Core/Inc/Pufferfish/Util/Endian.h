/*
 * Endian.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 *
 *  Utilities for working with arrays.
 */

#pragma once

#include "Bytes.h"
#include "Pufferfish/HAL/STM32/Endian.h"

namespace Pufferfish::Util {

constexpr void read_ntoh(const uint8_t *network_endian, uint32_t &host_endian) {
  uint32_t network_endian_value =
      (set_byte<3, uint32_t>(network_endian[3]) + set_byte<2, uint32_t>(network_endian[2]) +
       set_byte<1, uint32_t>(network_endian[1]) + set_byte<0, uint32_t>(network_endian[0]));
  host_endian = HAL::ntoh(network_endian_value);
}

constexpr void read_ntoh(const uint8_t *network_endian, uint16_t &host_endian) {
  uint16_t network_endian_value =
      (set_byte<1, uint16_t>(network_endian[1]) + set_byte<0, uint16_t>(network_endian[0]));
  host_endian = HAL::ntoh(network_endian_value);
}

constexpr void write_hton(uint32_t host_endian, uint8_t *network_endian) {
  uint32_t network_endian_value = HAL::hton(host_endian);
  network_endian[0] = get_byte<0>(network_endian_value);
  network_endian[1] = get_byte<1>(network_endian_value);
  network_endian[2] = get_byte<2>(network_endian_value);
  network_endian[3] = get_byte<3>(network_endian_value);
}

constexpr void write_hton(uint16_t host_endian, uint8_t *network_endian) {
  uint16_t network_endian_value = HAL::hton(host_endian);
  network_endian[0] = get_byte<0>(network_endian_value);
  network_endian[1] = get_byte<1>(network_endian_value);
}

}  // namespace Pufferfish::Util
