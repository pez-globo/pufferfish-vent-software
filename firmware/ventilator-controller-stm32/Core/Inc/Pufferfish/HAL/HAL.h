/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 *
 * A common hardware-abstraction-layer header file
 */

#pragma once

#include "Pufferfish/HAL/STM32/HALPWM.h"
#include "Pufferfish/HAL/Interfaces/I2CDevice.h"
#include "Pufferfish/HAL/CRC.h"

#include "Pufferfish/HAL/STM32/HALDigitalInput.h"
#include "Pufferfish/HAL/STM32/HALDigitalOutput.h"
#include "Pufferfish/HAL/STM32/Endian.h"
#include "Pufferfish/HAL/STM32/HALI2CDevice.h"
#include "Pufferfish/HAL/STM32/Time.h"
#include "Pufferfish/HAL/Mock/MockDigitalInput.h"
#include "Pufferfish/HAL/Mock/MockDigitalOutput.h"
