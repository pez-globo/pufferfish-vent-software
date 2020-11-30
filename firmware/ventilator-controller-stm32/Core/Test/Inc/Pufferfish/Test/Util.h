/*
 * Util.h
 *
 *  Created on: Nov 17, 2020
 *      Author: Renji Panicker
 *
 *  A set of helper functins for writing and debugging test cases
 */

#pragma once

#include <string>
#include <cmath>
#include "Pufferfish/Util/Vector.h"

#include "catch2/catch.hpp"

namespace Pufferfish::Util {

template <size_t payload_size>
inline bool operator==(const Pufferfish::Util::ByteVector<payload_size> &lhs, const std::string& rhs) {
  if(lhs.size() != rhs.size()) {
    return false;
  }
  for(size_t i = 0; i < lhs.size(); ++i) {
    if((uint8_t)lhs[i] != (uint8_t)rhs.at(i)) {
      return false;
    }
  }
  return true;
}

template <size_t payload_size>
inline bool operator!=(const Pufferfish::Util::ByteVector<payload_size> &lhs, const std::string& rhs) {
  return !(lhs == rhs);
}

inline bool isEqualFloat(const float &lhs, const float& rhs, const float& epsilon = 0.0002) {
  return std::fabs(rhs - lhs) < epsilon;
}

inline bool isLessFloat(const float &lhs, const float& rhs) {
  Catch::StringMaker<float>::precision = 14;
  if (isEqualFloat(lhs, rhs)) {
    return false;
  }
  return (lhs < rhs);
}

template <size_t payload_size>
inline bool convertStringToByteVector(const std::string& input_string, Pufferfish::Util::ByteVector<payload_size> &output_buffer) {
  if(input_string.size() >= payload_size) {
    return false;
  }
  for(auto& ch : input_string) {
    output_buffer.push_back(ch);
  }
  return true;
}

template <size_t payload_size>
inline std::string convertByteVectorToHexString(const Pufferfish::Util::ByteVector<payload_size> &input_buffer) {
  std::string output_string;
  for(size_t i = 0; i < input_buffer.size(); ++i) {
    auto& ch = input_buffer[i];
    output_string += "\\x";
    int c1 = ((ch & 0xf0) >> 4);
    if((c1 >= 0) && (c1 <= 9)) {
      output_string += (c1 + '0');
    }else if((c1 >= 10) && (c1 <= 15)) {
      output_string += ((c1-10) + 'A');
    }else{
      output_string += ('?');
    }

    c1 = (ch & 0x0f);
    if((c1 >= 0) && (c1 <= 9)) {
      output_string += (c1 + '0');
    }else if((c1 >= 10) && (c1 <= 15)) {
      output_string += ((c1-10) + 'A');
    }else{
      output_string += ('?');
    }
  }
  return output_string;
}

}
