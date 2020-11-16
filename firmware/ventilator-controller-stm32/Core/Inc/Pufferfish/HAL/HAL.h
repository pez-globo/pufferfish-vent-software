/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 *
 * A common hardware-abstraction-layer header file
 */

#pragma once

#include "CRCChecker.h"
#include "Types.h"

// Interfaces

#include "Interfaces/AnalogInput.h"
#include "Interfaces/BufferedUART.h"
#include "Interfaces/DigitalOutput.h"
#include "Interfaces/I2CDevice.h"
#include "Interfaces/PWM.h"
#include "Interfaces/SPIDevice.h"
#include "Interfaces/Time.h"

// STM32 HAL

#include "STM32/Endian.h"
#include "STM32/HALAnalogInput.h"
#include "STM32/HALBufferedUART.h"
#include "STM32/HALDigitalInput.h"
#include "STM32/HALDigitalOutput.h"
#include "STM32/HALI2CDevice.h"
#include "STM32/HALPWM.h"
#include "STM32/HALSPIDevice.h"
#include "STM32/HALTime.h"
