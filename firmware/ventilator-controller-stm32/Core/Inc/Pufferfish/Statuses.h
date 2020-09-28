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
  blockLock /// when block is locked
};

/**
 * SPI Instructions
 */
enum class SPIInstruction {
  deviceId = 0x90,     /// Instruction for read Device ID
  JEDECId = 0x9F,      /// Instruction for read JEDEC ID
  writeEnable = 0x06,  /// Instruction for write enable
  writeDisable = 0x04, /// Instruction for write disable
  writeByte = 0x02,    /// Instruction for write byte
  readByte = 0x03,     /// Instruction for read byte
  lockBlock = 0x36,    /// Instruction for individual lock block
  unlockBlock = 0x39,  /// Instruction for individual Unlock block
  globalLock = 0x7E,   /// Instruction for global lock block
  globalUnLock = 0x98,          /// Instruction for global Unlock block
  readBlockStatus = 0x3D,       /// Instruction for read block status
  chipErase = 0xC7,             /// Instruction for chip erase
  sectorErase4KB = 0x20,        /// Instruction for sector erase of 4KB
  blockErase32KB = 0x52,        /// Instruction for block erase of 32KB
  blockErase64KB = 0xD8,        /// Instruction for block erase of 64KB
  writeStatusRegister1 = 0x01,  /// Instruction for write status register 1
  readStatusRegister1 = 0x05,   /// Instruction for read status register 1
  writeStatusRegister2 = 0x31,  /// Instruction for write status register 2
  readStatusRegister2 = 0x35,   /// Instruction for read status register 2
  writeStatusRegister3 = 0x11,  /// Instruction for write status register 3
  readStatusRegister3 = 0x15,   /// Instruction for read status register 3
  powerDown = 0xB9,             /// Instruction for power down
  releasePowerDown = 0xAB,      /// Instruction for release power down
  resetEnable = 0x66,           /// Instruction for enable reset
  resetDevice = 0x99            /// Instruction for reset device
};

 /**
  * An outcome of performing an operation on ADC
  */
enum class ADCStatus {
  ok = 0, ///success
  error   /// error in ADC input
};

}  // namespace Pufferfish
