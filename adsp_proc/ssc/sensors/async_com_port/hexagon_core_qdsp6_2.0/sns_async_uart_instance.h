#pragma once
/*=============================================================================
  @file sns_async_uart_instance.h

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_signal.h"
#include "uart.h"

/*=============================================================================
  Type definitions
  ===========================================================================*/

typedef struct
{
  // UART handle
  uart_handle handle;

  // TX buffer
  // PEND: Turn this into a list of transmit buffers for true asynchronous behavior
  void *tx_buffer;

  // TX transaction ID
  uint32_t tx_transaction_id;
  bool tx_transaction_id_is_valid;

  // TX buffer size
  uint32_t tx_buffer_size;

  // RX Buffer
  // This should only be allocated and deallocated at the beginning and end of
  // the sensor instance's lifetime, respectively.
  void *rx_buffer;

  // RX buffer size
  // The RX buffer should always be this size.
  uint32_t rx_buffer_size;

  uint32_t rx_bytes_received;

  bool tx_transaction_finished;
  bool rx_transaction_finished;

  // Thread handle provided by the signal utility.
  sns_signal_thread *signal_thread;

  // Signal flag assigned by the signal utility
  uint32_t signal_flag;
} sns_async_uart_instance_state;
