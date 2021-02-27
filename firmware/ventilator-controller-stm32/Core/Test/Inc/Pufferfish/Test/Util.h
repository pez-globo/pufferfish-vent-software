/*
 * Util.h
 *
 *  Created on: Nov 17, 2020
 *      Author: Renji Panicker
 *
 *  A set of helper functions for writing and debugging test cases
 */

#pragma once

#include <string>

#include "Pufferfish/Util/Vector.h"

namespace Pufferfish::Util {

// operator to match ByteVector and HexString
template <size_t payload_size>
bool operator==(const Pufferfish::Util::ByteVector<payload_size>& lhs, const std::string& rhs);

// operator to match ByteVector and HexString
template <size_t payload_size>
bool operator!=(const Pufferfish::Util::ByteVector<payload_size>& lhs, const std::string& rhs);

// operator to match two ByteVector's
template <size_t payload_size>
bool operator==(
    const Pufferfish::Util::ByteVector<payload_size>& lhs,
    const Pufferfish::Util::ByteVector<payload_size>& rhs);

// operator to match two ByteVector's
template <size_t payload_size>
bool operator!=(
    const Pufferfish::Util::ByteVector<payload_size>& lhs,
    const Pufferfish::Util::ByteVector<payload_size>& rhs);

// converts HexString to ByteVector
template <size_t payload_size>
bool convert_string_to_byte_vector(
    const std::string& input_string, Pufferfish::Util::ByteVector<payload_size>& output_buffer);

// converts ByteVector to HexString
template <size_t payload_size>
std::string convert_byte_vector_to_hex_string(
    const Pufferfish::Util::ByteVector<payload_size>& input_buffer);

}  // namespace Pufferfish::Util

#include "Util.tpp"