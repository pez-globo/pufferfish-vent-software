/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Time.cpp
 *
 *  Created on: Jul 14, 2020
 *      Author: March Boonyapaluk
 */

#include "Pufferfish/HAL/STM32/Time.h"

#include "stm32h7xx_hal.h"

namespace Pufferfish::HAL {

uint32_t millis() {
  return HAL_GetTick();
}

void delay(uint32_t ms) {
  HAL_Delay(ms);
}

// magical sequence to turn on the DWT counter
// see:
// https://stackoverflow.com/questions/36378280/stm32-how-to-enable-dwt-cycle-counter
bool micros_delay_init() {
  // The following lines suppress Eclipse CDT's warning about C-style casts and
  // unresolvable fields; these come from the STM32 HAL so we can't do anything

  // TRCENA off, just in case
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  CoreDebug       // @suppress("C-Style cast instead of C++ cast")
      ->DEMCR &=  // @suppress("Field cannot be resolved")
      ~CoreDebug_DEMCR_TRCENA_Msk;
  // TRCENA on
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  CoreDebug       // @suppress("C-Style cast instead of C++ cast")
      ->DEMCR |=  // @suppress("Field cannot be resolved")
      CoreDebug_DEMCR_TRCENA_Msk;
  // Unlock debugger
  static const uint32_t unlock = 0xC5ACCE55;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  DWT->LAR =  // @suppress("C-Style cast instead of C++ cast") // @suppress("Field cannot be resolved")
      unlock;
  // reset the cycle counter
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  DWT->CYCCNT =  // @suppress("C-Style cast instead of C++ cast") // @suppress("Field cannot be resolved")
      0;
  // enable the counter
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  DWT->CTRL |=  // @suppress("C-Style cast instead of C++ cast") // @suppress("Field cannot be resolved")
      1U;

  // do some random works to check that the cycle counter actually working
  static const int limit = 64;
  volatile int i = 0;
  for (i = 2; i < limit; i *= i) {
  }

  // verify the cycles
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  return DWT->CYCCNT !=  // @suppress("C-Style cast instead of C++ cast") // @suppress("Field cannot be resolved")
         0;
}

uint32_t micros() {
  const uint32_t cycles_per_us = (HAL_RCC_GetHCLKFreq() / 1000000);

  // The following lines suppress Eclipse CDT's warning about C-style casts and
  // unresolvable fields; these come from the STM32 HAL so we can't do anything
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  return DWT->CYCCNT /  // @suppress("C-Style cast instead of C++ cast") // @suppress("Field cannot be resolved")
         cycles_per_us;
}

void delay_micros(uint32_t microseconds) {
  // The following lines suppress Eclipse CDT's warning about C-style casts and
  // unresolvable fields; these come from the STM32 HAL so we can't do anything
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const uint32_t start =
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      DWT->CYCCNT;  // @suppress("C-Style cast instead of C++ cast") // @suppress("Field cannot be resolved")
  static const uint32_t clock_scale = 1000000;
  microseconds *= HAL_RCC_GetHCLKFreq() / clock_scale;
  while (
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      DWT->CYCCNT -  // @suppress("Field cannot be resolved") // @suppress("C-Style cast instead of C++ cast")
          start <
      microseconds) {
  }
}

}  // namespace Pufferfish::HAL
