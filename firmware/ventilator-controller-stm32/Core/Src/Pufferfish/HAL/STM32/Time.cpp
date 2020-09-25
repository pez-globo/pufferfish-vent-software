/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Time.cpp
 *
 *  Created on: Jul 14, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/Time.h"

namespace Pufferfish {
namespace HAL {

uint32_t millis() {
  return HAL_GetTick();
}

void delay(uint32_t ms) {
  HAL_Delay(ms);
}

// magical sequence to turn on the DWT counter
// see: https://stackoverflow.com/questions/36378280/stm32-how-to-enable-dwt-cycle-counter
bool microsDelayInit() {
  // TRCENA off, just in case
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
  // TRCENA on
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  // Unlock debugger
  DWT->LAR = 0xC5ACCE55;
  // reset the cycle counter
  DWT->CYCCNT = 0;
  // enable the counter
  DWT->CTRL |= 1;

  // do some random works to check that the cycle counter actually working
  volatile int i;
  for (i = 2; i < 64; i *= i);

  // verify the cycles
  return DWT->CYCCNT != 0;
}

uint32_t micros() {
  const uint32_t cyclesPerUs = (HAL_RCC_GetHCLKFreq() / 1000000);
  return DWT->CYCCNT / cyclesPerUs;
}

void delayMicros(uint32_t microseconds) {
  const uint32_t start = DWT->CYCCNT;
  microseconds *= HAL_RCC_GetHCLKFreq() / 1000000;
  while (DWT->CYCCNT - start < microseconds);
}

} /* namespace HAL */
} /* namespace Pufferfish */
