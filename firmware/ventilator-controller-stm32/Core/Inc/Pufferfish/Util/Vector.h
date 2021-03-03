/*
 * Vector.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 *
 *  A basic statically-allocated variable-length byte buffer.
 *  Backed by an array. Methods use early returns of status codes instead
 *  of exceptions for error handling, for bounds-checking.
 *  Obviously, direct access of the buffer and size fields does not
 *  provide any bounds-checking.
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "Pufferfish/HAL/Types.h"
#include "Pufferfish/Statuses.h"

namespace Pufferfish::Util {

// BufferSize is recommended to be a power of two for compiler optimization.
template <typename Element, size_t array_size>
class Vector {
 public:
  Vector() = default;

  [[nodiscard]] size_t size() const;
  [[nodiscard]] static constexpr size_t max_size() { return array_size; }
  [[nodiscard]] bool empty() const;
  [[nodiscard]] bool full() const;
  [[nodiscard]] size_t available() const;

  void clear();
  IndexStatus resize(size_t new_size);
  IndexStatus push_back(uint8_t new_byte);

  constexpr const Element &operator[](size_t position) const;
  constexpr Element &operator[](size_t position);

  template <size_t source_size>
  void copy_from(const std::array<Element, source_size> &source_bytes, size_t dest_start_index = 0);
  void copy_from(const Vector<Element, array_size> &source_bytes, size_t dest_start_index = 0);
  void copy_from(const Element *source_bytes, size_t source_size, size_t dest_start_index = 0);

  [[nodiscard]] constexpr const Element *buffer() const { return buffer_.data(); }
  constexpr Element *buffer() { return buffer_.data(); }

 private:
  std::array<Element, array_size> buffer_{};
  size_t size_ = 0;
};

template <size_t array_size>
using ByteVector = Vector<uint8_t, array_size>;

}  // namespace Pufferfish::Util

#include "Vector.tpp"
