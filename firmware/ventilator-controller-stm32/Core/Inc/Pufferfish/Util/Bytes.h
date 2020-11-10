/*
 * Bytes.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 *
 *  Utilities for working with byte representations of variables.
 */

#pragma once

#include <climits>
#include <cstdint>

namespace Pufferfish::Util {

// Get the nth byte of the number, where byte 0 is the least significant byte
/**
 * Get the byte at some position of a number
 * @param number the number
 * @tparam byte_index the position of the byte, where 0 is the least significant position
 * @tparam T the type of the number
 * @return A number of the specified type with the given byte shifted into the given location
 */
template <size_t byte_index, typename T>
constexpr uint8_t get_byte(T number) {
  return number >> static_cast<size_t>(byte_index * CHAR_BIT);
}

/**
 * Left-shift a byte by some number of byte positions, for use in constexprs
 * @param byte the byte to shift
 * @tparam byte_index the shift index of the byte, where 0 leaves it unshifted (in the least
 * significant byte position)
 * @tparam T the type of the number to return
 * @return A number of the specified type with the given byte shifted into the given location
 */
template <size_t byte_index, typename T>
constexpr T set_byte(uint8_t byte) {
  return byte << static_cast<size_t>(byte_index * CHAR_BIT);
}

/**
 * Left-shift a byte by some number of byte positions
 * @param byte the byte to shift
 * @param byte_index the shift index of the byte, where 0 leaves it unshifted (in the least
 * significant byte position)
 * @tparam T the type of the number to return
 * @return A number of the specified type with the given byte shifted into the given location
 */
template <typename T>
T set_byte(uint8_t byte, size_t byte_index);

}  // namespace Pufferfish::Util

#include "Bytes.tpp"
