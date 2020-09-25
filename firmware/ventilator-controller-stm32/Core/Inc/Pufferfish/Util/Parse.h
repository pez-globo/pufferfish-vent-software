/*
 * Parse.h
 *
 *  Created on: Aug 31, 2020
 *      Author: Ethan Li
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Pufferfish {
namespace Util {

/**
 * Parse a buffer as a value in network-order
 * @param buffer a byte buffer
 * @param size the number of bytes to read from the byte buffer
 * @return the given number in network-order
 */
template <typename T>
T parse_network_order(
    const uint8_t *buffer,
    size_t size,
    typename std::enable_if<std::is_arithmetic<T>::value, std::nullptr_t>::type /*unused*/
    = nullptr);

}  // namespace Util
}  // namespace Pufferfish

#include "Parse.tpp"
