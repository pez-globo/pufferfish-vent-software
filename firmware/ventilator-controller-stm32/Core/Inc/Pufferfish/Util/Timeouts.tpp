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

namespace Pufferfish {
namespace Util {

template <typename T>
inline bool within_timeout(T start_time, T timeout, T test_time) {
  T duration = test_time - start_time;
  return duration < timeout;
}

}  // namespace Util
}  // namespace Pufferfish
