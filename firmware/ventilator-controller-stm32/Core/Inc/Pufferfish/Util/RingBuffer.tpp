/// \file
/// \brief An interrupt-safe byte stream backed by a circular buffer.

// Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.

// This is an adaptation of code published at
// https://github.com/hexagon5un/embed_with_elliot-circular_buffer
// by Elliot Williams under the MIT License. This implementation makes
// significant implementations to provide a C++ interface.
// Third-party works included this file are licensed as follows:
//
// Copyright (c) 2015 Elliot Williams
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "RingBuffer.h"

#include <cstring>

namespace Pufferfish {
namespace Util {

template<HAL::AtomicSize BufferSize>
RingBuffer<BufferSize>::RingBuffer() {}

template<HAL::AtomicSize BufferSize>
BufferStatus RingBuffer<BufferSize>::read(uint8_t &readByte) volatile {
  if (newestIndex == oldestIndex) {
      return BufferStatus::empty;
  }

  readByte = buffer[oldestIndex];
  oldestIndex = (oldestIndex + 1) % maxSize;
  return BufferStatus::ok;
}

template<HAL::AtomicSize BufferSize>
BufferStatus RingBuffer<BufferSize>::peek(uint8_t &peekByte) const volatile {
  if (newestIndex == oldestIndex) {
      return BufferStatus::empty;
  }

  peekByte = buffer[oldestIndex];
  return BufferStatus::ok;
}

template<HAL::AtomicSize BufferSize>
BufferStatus RingBuffer<BufferSize>::write(uint8_t writeByte) volatile {
  HAL::AtomicSize nextIndex = (newestIndex + 1) % maxSize;
  if (nextIndex == oldestIndex) {
      return BufferStatus::full;
  }

  buffer[newestIndex] = writeByte;
  newestIndex = nextIndex;
  return BufferStatus::ok;
}

} // namespace Util
} // namespace Pufferfish
