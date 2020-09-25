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
  block_lock    /// when block is locked
};

/**
 * SPI Instructions
 */
enum class SPIInstruction {
  device_id = 0x90,               /// Instruction for read Device ID
  jedec_id = 0x9F,                /// Instruction for read JEDEC ID
  write_enable = 0x06,            /// Instruction for write enable
  write_disable = 0x04,           /// Instruction for write disable
  write_byte = 0x02,              /// Instruction for write byte
  read_byte = 0x03,               /// Instruction for read byte
  lock_block = 0x36,              /// Instruction for individual lock block
  unlock_block = 0x39,            /// Instruction for individual Unlock block
  global_lock = 0x7E,             /// Instruction for global lock block
  global_unlock = 0x98,           /// Instruction for global Unlock block
  read_block_status = 0x3D,       /// Instruction for read block status
  chip_erase = 0xC7,              /// Instruction for chip erase
  sector_erase_4kb = 0x20,        /// Instruction for sector erase of 4KB
  block_erase_32kb = 0x52,        /// Instruction for block erase of 32KB
  block_erase_64kb = 0xD8,        /// Instruction for block erase of 64KB
  write_status_register1 = 0x01,  /// Instruction for write status register 1
  read_status_register1 = 0x05,   /// Instruction for read status register 1
  write_status_register2 = 0x31,  /// Instruction for write status register 2
  read_status_register2 = 0x35,   /// Instruction for read status register 2
  write_status_register3 = 0x11,  /// Instruction for write status register 3
  read_status_register3 = 0x15,   /// Instruction for read status register 3
  power_down = 0xB9,              /// Instruction for power down
  release_power_down = 0xAB,      /// Instruction for release power down
  reset_enable = 0x66,            /// Instruction for enable reset
  reset_device = 0x99             /// Instruction for reset device
};

/**
 * An outcome of performing an operation on ADC
 */
enum class ADCStatus {
  ok = 0,  /// success
  error    /// error in ADC input
};

}  // namespace Pufferfish
