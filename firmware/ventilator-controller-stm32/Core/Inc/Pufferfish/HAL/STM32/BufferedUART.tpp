/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 *      Author: Ethan Li
 */

#include "BufferedUART.h"
#include "Time.h"

namespace Pufferfish {
namespace HAL {

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
BufferedUART<rx_buffer_size, tx_buffer_size>::BufferedUART(UART_HandleTypeDef &huart)
    : huart_(huart) {}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
BufferStatus BufferedUART<rx_buffer_size, tx_buffer_size>::read(uint8_t &read_byte) volatile {
  return rx_buffer_.read(read_byte);
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
BufferStatus BufferedUART<rx_buffer_size, tx_buffer_size>::write(uint8_t write_byte) volatile {
  BufferStatus status = tx_buffer_.write(write_byte);
  __HAL_UART_ENABLE_IT(&huart_, UART_IT_TXE);  // write a byte on the next TX empty interrupt
  return status;
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
BufferStatus BufferedUART<rx_buffer_size, tx_buffer_size>::write(
    const uint8_t *write_bytes, AtomicSize write_size, HAL::AtomicSize &written_size) volatile {
  for (written_size = 0; written_size < write_size; ++written_size) {
    if (write(write_bytes[written_size]) != BufferStatus::ok) {
      break;
    }
  }
  if (write_size == written_size) {
    return BufferStatus::ok;
  }
  return BufferStatus::partial;
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
BufferStatus BufferedUART<rx_buffer_size, tx_buffer_size>::write_block(
    uint8_t write_byte, uint32_t timeout) volatile {
  uint32_t start = millis();
  while (true) {
    if (write(write_byte) == BufferStatus::ok) {
      return BufferStatus::ok;
    }
    if ((timeout > 0) && ((millis() - start) > timeout)) {
      return BufferStatus::full;
    }
  }
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
BufferStatus BufferedUART<rx_buffer_size, tx_buffer_size>::write_block(
    const uint8_t *write_bytes,
    AtomicSize write_size,
    uint32_t timeout,
    HAL::AtomicSize &written_size) volatile {
  uint32_t start = millis();
  while (written_size < write_size) {
    AtomicSize just_written = 0;
    write(write_bytes + written_size, write_size - written_size, just_written);
    written_size += just_written;
    if ((timeout > 0) && ((millis() - start) > timeout)) {
      break;
    }
  }
  if (write_size == written_size) {
    return BufferStatus::ok;
  }
  return BufferStatus::partial;
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
void BufferedUART<rx_buffer_size, tx_buffer_size>::setup_irq() volatile {
  UART_MASK_COMPUTATION(&huart_);
  __HAL_UART_ENABLE_IT(&huart_, UART_IT_RXNE);
  // We only enable TXE when after we write to txBuffer
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
void BufferedUART<rx_buffer_size, tx_buffer_size>::handle_irq() volatile {
  handle_irq_rx();
  handle_irq_tx();
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
uint32_t BufferedUART<rx_buffer_size, tx_buffer_size>::rx_dropped() const volatile {
  return rx_dropped_;
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
void BufferedUART<rx_buffer_size, tx_buffer_size>::handle_irq_rx() volatile {
  bool rxne_enabled = __HAL_UART_GET_IT_SOURCE(&huart_, UART_IT_RXNE) != RESET;
  bool rxne_flagged = __HAL_UART_GET_FLAG(&huart_, UART_FLAG_RXNE) != RESET;
  if (!rxne_enabled || !rxne_flagged) {  // check for RX not empty interrupt
    return;
  }

  auto rx_byte = static_cast<uint8_t>(huart_.Instance->RDR & huart_.Mask);  // assumes 8-bit byte
  if (rx_buffer_.write(rx_byte) != BufferStatus::ok) {
    ++rx_dropped_;
  }
  __HAL_UART_SEND_REQ(&huart_, UART_RXDATA_FLUSH_REQUEST);  // clear RXNE flag
}

template <AtomicSize rx_buffer_size, AtomicSize tx_buffer_size>
void BufferedUART<rx_buffer_size, tx_buffer_size>::handle_irq_tx() volatile {
  bool txe_enabled = __HAL_UART_GET_IT_SOURCE(&huart_, UART_IT_TXE) != RESET;
  bool txe_flagged = __HAL_UART_GET_FLAG(&huart_, UART_FLAG_TXE) != RESET;
  if (!txe_enabled || !txe_flagged) {  // check for TX empty interrupt
    return;
  }

  uint8_t tx_byte = 0;
  if (tx_buffer_.read(tx_byte) == BufferStatus::empty) {
    // stop receiving TX empty interrupts until we have more data for TX
    __HAL_UART_DISABLE_IT(&huart_, UART_IT_TXE);
    return;
  }

  huart_.Instance->TDR = tx_byte;
}

}  // namespace HAL
}  // namespace Pufferfish
