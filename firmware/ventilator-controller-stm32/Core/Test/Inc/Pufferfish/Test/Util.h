/*
 * Util.h
 *
 *  Created on: Nov 17, 2020
 *      Author: Renji Panicker
 *
 *  A set of helper functins for test cases
 */

#pragma once

#include <string>
#include "Pufferfish/Util/Vector.h"

namespace Pufferfish::Util {

template <size_t payload_size>
inline bool operator==(const Pufferfish::Util::ByteVector<payload_size> &lhs, const std::string& rhs) {
  if(lhs.size() != rhs.size()) {
    return false;
  }
  for(size_t i = 0; i < lhs.size(); ++i) {
    if(lhs[i] != rhs.at(i)) {
      return false;
    }
  }
  return true;
}

template <size_t payload_size>
inline bool operator!=(const Pufferfish::Util::ByteVector<payload_size> &lhs, const std::string& rhs) {
  return !(lhs == rhs);
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

}
