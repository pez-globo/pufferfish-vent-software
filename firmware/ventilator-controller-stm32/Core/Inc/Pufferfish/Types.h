/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 */

#pragma once

#include <cstdint>

namespace Pufferfish {

/**
 * A unit of pressure read from sensors
 */
enum class PressureUnit { unknown = 0, psi, pa, k_pa, m_pa, m_bar, bar };

}  // namespace Pufferfish
