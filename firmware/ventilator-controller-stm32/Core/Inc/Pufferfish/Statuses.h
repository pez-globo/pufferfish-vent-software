/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Ethan Li
 */

#pragma once

namespace Pufferfish {

/**
 * An outcome of attempting to perform an operation on a buffer
 */
enum class BufferStatus {
  ok = 0,  /// buffer operation succeeded
  empty,   /// buffer is empty so no read is possible
  full,    /// buffer is full so no write is possible
  partial  /// only a partial buffer operation was possible
};

/**
 * Possible alarms that could be raised by the system, must by sorted by
 * priority in ascending order
 */
enum class AlarmStatus {
  high_priority = 0,  /// High Priority Alarm as defined in IEC 60601-1-8
  medium_priority,    /// Medium Priority Alarm as defined in IEC 60601-1-8
  low_priority,       /// Low Priority Alarm as defined in IEC 60601-1-8
  technical1,         /// Technical Alarm 1 for debugging
  technical2,         /// Technical Alarm 2 for debugging
  no_alarm            /// no alarm is present, must be last
};

/**
 * Error code of the Alarm Manager module
 */
enum class AlarmManagerStatus {
  ok = 0,         /// no error
  invalid_alarm,  /// invalid alarm in argument
  not_set,        /// the specified alarm has not been set
  hardware_error  /// there is a hardware error when driving an alarm
};

/**
 * An outcome of performing an operation on I2C bus
 */
enum class I2CDeviceStatus {
  ok = 0,             /// success
  not_supported,      /// the operation is not supported
  write_error,        /// an error occur when writing to an I2C device
  read_error,         /// an error occur when reading from an I2C device
  invalid_arguments,  /// invalid input to the function
  crc_check_failed,   /// The CRC code received is inconsistent
  invalid_ext_slot,   /// The MUX slot of ExtendedI2CDevice is invalid
  test_failed,        /// unit tests are failing
  no_new_data         /// no new data is received from the sensor
};

/**
 * An outcome of performing an operation on PWM
 */
enum class PWMStatus {
  ok = 0,              /// success
  invalid_duty_cycle,  /// invalid duty cycle input to the function
  hal_error            /// error starting or stopping the PWM generator
};

/**
 * An outcome of performing an operation on SPI bus
 */
enum class SPIDeviceStatus {
  ok = 0,       /// success
  write_error,  /// an error occur when writing to an SPI device
  read_error,   /// an error occur when reading from an SPI device
  busy,         /// when SPI device is not ready
  error,        /// fail
};

/**
 * An outcome of performing an operation on ADC
 */
enum class ADCStatus {
  ok = 0,  /// success
  error    /// error in ADC input
};

}  // namespace Pufferfish
