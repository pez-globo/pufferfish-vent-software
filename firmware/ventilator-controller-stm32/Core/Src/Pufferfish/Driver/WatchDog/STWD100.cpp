
/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * STWD100.cpp
 *
 *   Author: Laxmanrao R
 *
 *  A driver for the STWD100 External watchdog
 */

#include "Pufferfish/Driver/WatchDog/STWD100.h"

namespace Pufferfish::Driver::WatchDog {

void STWD100::enable() {
  enable_pin_.write(false);
}

void STWD100::disable() {
  enable_pin_.write(true);
}

void STWD100::toggle_input() {
  input_pin_.write(true);
  HAL::delay_micros(0);
  input_pin_.write(false);
}

}  // namespace Pufferfish::Driver::WatchDog
