/*
 * Util.cpp
 *
 *  Created on: 27 feb, 2020
 *      Author: Renji panicker
 *
 *  A set of helper functions for writing and debugging test cases
 */
#include "Util.h"

namespace Pufferfish::Util {

template <size_t payload_size>
bool operator==(const Pufferfish::Util::ByteVector<payload_size>& lhs, const std::string& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (static_cast<uint8_t>(lhs[i]) != static_cast<uint8_t>(rhs.at(i))) {
      return false;
    }
  }
  return true;
}

template <size_t payload_size>
bool operator!=(const Pufferfish::Util::ByteVector<payload_size>& lhs, const std::string& rhs) {
  return !(lhs == rhs);
}

template <size_t payload_size>
bool operator==(
    const Pufferfish::Util::ByteVector<payload_size>& lhs,
    const Pufferfish::Util::ByteVector<payload_size>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (static_cast<uint8_t>(lhs[i]) != static_cast<uint8_t>(rhs[i])) {
      return false;
    }
  }
  return true;
}

template <size_t payload_size>
bool operator!=(
    const Pufferfish::Util::ByteVector<payload_size>& lhs,
    const Pufferfish::Util::ByteVector<payload_size>& rhs) {
  return !(lhs == rhs);
}

template <size_t payload_size>
bool convert_string_to_byte_vector(
    const std::string& input_string, Pufferfish::Util::ByteVector<payload_size>& output_buffer) {
  if (input_string.size() >= payload_size) {
    return false;
  }
  for (const auto& ch : input_string) {
    output_buffer.push_back(ch);
  }
  return true;
}

template <size_t payload_size>
std::string convert_byte_vector_to_hex_string(
    const Pufferfish::Util::ByteVector<payload_size>& input_buffer) {
  std::string output_string;
  for (size_t i = 0; i < input_buffer.size(); ++i) {
    auto& ch = input_buffer[i];
    output_string += "\\x";
    int c1 = ((ch & 0xf0) >> 4);
    if ((c1 >= 0) && (c1 <= 9)) {
      output_string += std::to_string(c1);
    } else if ((c1 >= 10) && (c1 <= 15)) {
      output_string += static_cast<char>(c1 - 10 + 'A');
    } else {
      output_string += ('?');
    }

    c1 = (ch & 0x0f);
    if ((c1 >= 0) && (c1 <= 9)) {
      output_string += std::to_string(c1);
    } else if ((c1 >= 10) && (c1 <= 15)) {
      output_string += static_cast<char>(c1 - 10 + 'A');
    } else {
      output_string += ('?');
    }
  }
  return output_string;
}

}  // namespace Pufferfish::Util
