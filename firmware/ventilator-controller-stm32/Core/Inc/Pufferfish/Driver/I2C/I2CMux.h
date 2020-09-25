/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: March Boonyapaluk
 *
 *  An abstract class for I2C Extender MUX
 */

#pragma once

#include "Pufferfish/Statuses.h"
#include "Pufferfish/Types.h"

namespace Pufferfish {
namespace Driver {
namespace I2C {

/**
 * An abstract class for a device multiplexer
 */
class I2CMux {
 public:
  /**
   * Changes the device slot of the multiplexer
   * @param slot    a slot of the mux
   * @return ok if the updating is success, error code otherwise
   */
  virtual I2CDeviceStatus select_slot(uint8_t slot) = 0;

  /**
   * Gets a current slot multiplexer is in
   * @return current activated slot of the mux, or 0xFF if not connected/unknown
   */
  [[nodiscard]] virtual uint8_t get_current_slot() const = 0;
};

}  // namespace I2C
}  // namespace Driver
}  // namespace Pufferfish
