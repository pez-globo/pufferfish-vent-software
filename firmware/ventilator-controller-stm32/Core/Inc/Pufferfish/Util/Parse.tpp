/*
 * Parse.tpp
 *
 *  Created on: Aug 31, 2020
 *      Author: Ethan Li
 */

#include <climits>

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
    typename std::enable_if<std::is_arithmetic<T>::value, std::nullptr_t>::type /*unused*/) {
  T value = 0;
  for (size_t i = 0; i < size; ++i) {
    value += buffer[i] << (i * CHAR_BIT);
  }
  return value;
}

}  // namespace Util
}  // namespace Pufferfish
