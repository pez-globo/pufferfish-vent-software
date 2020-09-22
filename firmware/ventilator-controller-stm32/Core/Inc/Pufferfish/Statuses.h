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
  empty,  /// buffer is empty so no read is possible
  full,  /// buffer is full so no write is possible
  partial  /// only a partial buffer operation was possible
};

/**
 * Possible alarms that could be raised by the system, must by sorted by
 * priority in ascending order
 */
enum class AlarmStatus {
  highPriority = 0,  /// High Priority Alarm as defined in IEC 60601-1-8
  mediumPriority,  /// Medium Priority Alarm as defined in IEC 60601-1-8
  lowPriority,  /// Low Priority Alarm as defined in IEC 60601-1-8
  technical1,  /// Technical Alarm 1 for debugging
  technical2,  /// Technical Alarm 2 for debugging
  noAlarm  /// no alarm is present, must be last
};

/**
 * Error code of the Alarm Manager module
 */
enum class AlarmManagerStatus {
  ok = 0, /// no error
  invalidAlarm, /// invalid alarm in argument
  notSet, /// the specified alarm has not been set
  hardwareError /// there is a hardware error when driving an alarm
};
  
/**
 * An outcome of performing an operation on I2C bus
 */
enum class I2CDeviceStatus {
  ok = 0, /// success
  notSupported, /// the operation is not supported
  writeError, /// an error occur when writing to an I2C device
  readError, /// an error occur when reading from an I2C device
  invalidArguments, /// invalid input to the function
  crcCheckFailed, /// The CRC code received is inconsistent
  invalidExtSlot, /// The MUX slot of ExtendedI2CDevice is invalid
  testFailed, /// unit tests are failing
  noNewData /// no new data is received from the sensor
};

/**
 * An outcome of performing an operation on PWM
 */
enum class PWMStatus {
  ok = 0, /// success
  invalidDutyCycle, /// invalid duty cycle input to the function
  halError /// error starting or stopping the PWM generator
};

/**
 * An outcome of performing an operation on SPI bus
 */
enum class SPIDeviceStatus {
  ok = 0, /// success
  writeError, /// an error occur when writing to an SPI device
  readError, /// an error occur when reading from an SPI device
  busy, /// when SPI device is not ready
  error, /// fail
};

 /**
  * An outcome of performing an operation on ADC
  */
enum class ADCStatus {
  ok = 0, ///success
  error   /// error in ADC input
};

}  // namespace Pufferfish
