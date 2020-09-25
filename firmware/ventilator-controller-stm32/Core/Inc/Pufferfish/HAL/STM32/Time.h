/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *  Created on: Jul 14, 2020
 *      Author: March Boonyapaluk
 */

#pragma once

#include <cstdint>

namespace Pufferfish {
namespace HAL {

/**
 * Returns the number of millisecond since the startup,
 *  will be rolled over every 50 days
 * @returnthe millisecond since start
 */
uint32_t millis();

/**
 * Block the execution for the given number of millisecond
 *  This function provide delay in the range of [ms, ms + 1)
 * @param ms the number of millisecond to delay
 */
void delay(uint32_t ms);

/**
 * Initializes the DWT cycle counter, must be called at the MCU startup if
 * micros or delayMicros is used
 * @return true if the initialization is successful, false otherwise
 */
bool micros_delay_init();

/**
 * Returns the number of microsecond since the startup,
 * will be rolled over every around 67 seconds with 64 MHz system clock
 * @return the number of microsecond
 */
uint32_t micros();

/**
 * Block the execution for the provided microseconds
 *  The delay provided should be relatively accurate [us, us + 3)
 * @param microseconds the number of microseconds to delay,
 *  must be less than 67 seconds with 64 MHz system clock
 */
void delay_micros(uint32_t microseconds);

} /* namespace HAL */
} /* namespace Pufferfish */
