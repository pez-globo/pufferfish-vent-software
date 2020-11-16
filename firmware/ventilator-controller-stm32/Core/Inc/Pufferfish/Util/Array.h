/*
 * Array.h
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 *
 *  Utilities for working with arrays.
 */

#pragma once

#include <array>

namespace Pufferfish::Util {

template <typename ElementType, typename... Arg>
constexpr auto make_array(Arg&&... arg) {
  return std::array<ElementType, sizeof...(Arg)>{{std::forward<ElementType>(arg)...}};
}

}  // namespace Pufferfish::Util
