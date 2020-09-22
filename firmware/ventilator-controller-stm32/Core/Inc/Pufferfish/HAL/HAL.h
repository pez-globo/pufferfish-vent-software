/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 *
 * A common hardware-abstraction-layer header file
 */

#pragma once

#include "Pufferfish/HAL/Types.h"
#include "Pufferfish/HAL/CRC.h"

// Interfaces

#include "Pufferfish/HAL/Interfaces/DigitalOutput.h"
#include "Pufferfish/HAL/Interfaces/I2CDevice.h"
#include "Pufferfish/HAL/Interfaces/SPIDevice.h"

// STM32-specific definitions

#include "Pufferfish/HAL/STM32/AnalogInput.h"
#include "Pufferfish/HAL/STM32/BufferedUART.h"
#include "Pufferfish/HAL/STM32/DigitalInput.h"
#include "Pufferfish/HAL/STM32/HALDigitalOutput.h"
#include "Pufferfish/HAL/STM32/Endian.h"
#include "Pufferfish/HAL/STM32/HALI2CDevice.h"
#include "Pufferfish/HAL/STM32/HALSPIDevice.h"
#include "Pufferfish/HAL/STM32/PWM.h"
#include "Pufferfish/HAL/STM32/Time.h"
