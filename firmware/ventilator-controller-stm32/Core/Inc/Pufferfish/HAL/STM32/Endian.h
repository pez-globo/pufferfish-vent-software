/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Ethan Li
 *
 *  Endianness conversions between host and network order.
 *  STM32 is little-endian, while network order is big-endian, so
 *  the conversion functions reverse the order of bytes.
 *
 *  NOTE: this implementation relies on builtins provided by GCC/Clang.
 *  You will need to compile using a relatively modern version of either
 *  compiler.
 */

#pragma once

#include <cstdint>

namespace Pufferfish {
namespace HAL {

/**
 * Convert endianness of a 64-bit data in host-order to network-order
 * @param hostValue a 64-bit number in host-order
 * @return the given number in network-order
 */
inline uint64_t hton(uint64_t host_value) {
  return __builtin_bswap64(host_value);
}

/**
 * Convert endianness of a 64-bit data in host-order to network-order
 * @param networkValue a 64-bit number in network-order
 * @return the given number in host-order
 */
inline uint64_t ntoh(uint64_t network_value) {
  return __builtin_bswap64(network_value);
}

/**
 * Convert endianness of a 32-bit data in host-order to network-order
 * @param hostValue a 32-bit number in host-order
 * @return the given number in network-order
 */
inline uint32_t hton(uint32_t host_value) {
  return __builtin_bswap32(host_value);
}

/**
 * Convert endianness of a 32-bit data in host-order to network-order
 * @param networkValue a 32-bit number in network-order
 * @return the given number in host-order
 */
inline uint32_t ntoh(uint32_t network_value) {
  return __builtin_bswap32(network_value);
}

/**
 * Convert endianness of a 16-bit data in host-order to network-order
 * @param hostValue a 16-bit number in host-order
 * @return the given number in network-order
 */
inline uint16_t hton(uint16_t host_value) {
  return __builtin_bswap16(host_value);
}

/**
 * Convert endianness of a 16-bit data in host-order to network-order
 * @param networkValue a 16-bit number in network-order
 * @return the given number in host-order
 */
inline uint16_t ntoh(uint16_t network_value) {
  return __builtin_bswap16(network_value);
}

}  // namespace HAL
}  // namespace Pufferfish
