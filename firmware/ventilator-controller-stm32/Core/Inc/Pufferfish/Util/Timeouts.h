/// \file
/// \brief Convenience functions for safely checking timeouts
///
/// Functions for uint rollover-safe checking of timeouts

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

#pragma once

namespace Pufferfish {
namespace Util {

/**
 * Checks whether the test time is within the timeout, modulo rollover
 * @param start_time the start time for the timeout interval
 * @param timeout the duration of the timeout interval
 * @param test_time the time to test
 * @return true if the test time is within the timeout, false otherwise
 */
template <typename T>
inline bool within_timeout(T start_time, T timeout, T test_time);

}  // namespace Util
}  // namespace Pufferfish

#include "Timeouts.tpp"
