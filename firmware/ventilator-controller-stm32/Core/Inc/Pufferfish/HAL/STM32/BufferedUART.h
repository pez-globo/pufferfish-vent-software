/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Ethan Li
 *
 *  An interrupt-backed UART I/O endpoint, exposing a buffered read/write interface.
 */

#pragma once

#include "stm32h7xx_hal.h"
#include "Pufferfish/Types.h"
#include "Pufferfish/Statuses.h"
#include "Pufferfish/Util/RingBuffer.h"

namespace Pufferfish {
namespace HAL {

/**
 * UART RX and TX with non-blocking queue interface.
 *
 * Backed by ring buffers serviced by interrupt handlers.
 * This class allows a "polling" approach to reading and writing
 * data which is sent/received asynchronously from the other peer.
 * This provides a comparable interface to Arduino's Serial class,
 * but with a non-blocking interface.
 */
template <AtomicSize RXBufferSize, AtomicSize TXBufferSize>
class BufferedUART {
public:
  BufferedUART(UART_HandleTypeDef &huart);

  /**
   * Attempt to "pop" the next received byte from the RX queue.
   *
   * Gives up without causing any side-effects if the RX queue is empty;
   * if it gives up, readByte will be left unmodified.
   * @param readByte[[out] the byte popped from the RX queue
   * @return ok on success, empty otherwise
   */
  BufferStatus read(uint8_t &readByte) volatile;

  /**
   * Attempt to "push" the provided byte onto the TX queue.
   *
   * Gives up without causing any side-effects if the TX queue is full.
   * @param writeByte the byte to push onto the TX queue
   * @return ok on success, full otherwise
   */
  BufferStatus write(uint8_t writeByte) volatile;

  /**
   * "Push" bytes in the provided buffer onto the TX queue until either
   * all provided bytes are pushed or the TX queue becomes full.
   *
   * Because this may perform a partial write, you can use the writtenSize
   * output parameter to continue writing a buffer later: just advance the
   * writeBytes buffer pointer by writtenSize, and subtract writtenSize from
   * writeSize in the next method call to attempt to write the remaining bytes.
   * If the TX buffer is full, this will return partial and writtenSize will be 0.
   * @param writeBytes a pointer to the start of the buffer of bytes to push
   * @param writeSize the number of bytes in the buffer to push
   * @param writtenSize[out] the number of bytes successfully pushed onto the TX queue
   * @return ok if all provided bytes were added to the queue, partial otherwise
   */
  BufferStatus write(
      const uint8_t *writeBytes, AtomicSize writeSize,
      HAL::AtomicSize &writtenSize
  ) volatile;

  /**
   * Persistently attempt to "push" the provided byte onto the TX queue
   * until the byte gets pushed or the timeout has elapsed.
   *
   * If the TX queue is initially full, retry up until the timeout,
   * but give up after the timeout.
   * @param writeByte the byte to push onto the TX queue
   * @param uint32_t timeout the length of time in ms to retry pushing the byte if the TX queue is full
   * @return ok on success, full otherwise
   */
  BufferStatus writeBlock(uint8_t writeByte, uint32_t timeout) volatile;

  /**
   * Persistently attempt to "push" bytes in the provided buffer onto the TX queue
   * until either all provided bytes are pushed or the timeout has elapsed.
   *
   * Because this may perform a partial write, you can use the writtenSize
   * output parameter to continue writing a buffer later: just advance the
   * writeBytes buffer pointer by writtenSize, and subtract writtenSize from
   * writeSize in the next method call to attempt to write the remaining bytes.
   * If the TX buffer is full, this will return partial and writtenSize will be 0.
   * @param writeBytes a pointer to the start of the buffer of bytes to push
   * @param writeSize the number of bytes in the buffer to push
   * @param timeout the length of time in ms to retry pushing bytes when the TX queue is full
   * @param writtenSize[out] the number of bytes successfully pushed onto the TX queue
   * @return ok if all provided bytes were added to the queue, partial otherwise
   */
  BufferStatus writeBlock(
      const uint8_t *writeBytes, AtomicSize writeSize, uint32_t timeout,
      HAL::AtomicSize &writtenSize
  ) volatile;


  /**
   * Set up the UART interrupt to service the RX queue.
   */
  void setupIRQ() volatile;


  /**
   * Handle the UART interrupt which occurs when the RX or TX queue should be serviced.
   */
  void handleIRQ() volatile;

  /**
   * A counter of the number of received UART bytes which were discarded.
   *
   * Received UART bytes are discarded when the RX queue is full. If you are
   * seeing many dropped bytes, you are not consuming bytes from the RX queue
   * quickly enough. If you save the returned value from a previous call of this
   * method, you can compare it against the returned value from the latest call
   * of this method to determine how many bytes were dropped since you last called
   * this method.
   * @return the total number of received UART bytes which were discarded.
   */
  uint32_t rxDropped() const volatile;

protected:
  UART_HandleTypeDef &huart;

  volatile Util::RingBuffer<RXBufferSize> rxBuffer;
  volatile Util::RingBuffer<TXBufferSize> txBuffer;

  void handleIRQRX() volatile;
  void handleIRQTX() volatile;

  volatile uint32_t mRxDropped = 0;
};

using LargeBufferedUART = BufferedUART<4096, 4096>;

} // namespace HAL
} // namespace Pufferfish

#include "BufferedUART.tpp"
