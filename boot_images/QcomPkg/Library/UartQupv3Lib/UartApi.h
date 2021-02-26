#ifndef UART_API_H
#define UART_API_H
/*==================================================================================================

FILE: UartApi.h

DESCRIPTION: This module provides the driver software for the UART.

Copyright (c) 2016 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   uart_close
   uart_open
   uart_receive
   uart_transmit
==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UartQupv3Lib/UartApi.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/7/16    VV       Initial revision

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "com_dtypes.h"

/*==================================================================================================
                                             ENUMERATIONS
==================================================================================================*/

typedef enum
{
   UART_FIRST_PORT = 0,
   UART_DEBUG_PORT = UART_FIRST_PORT,

   UART_SECOND_PORT,
   UART_HS_PORT = UART_SECOND_PORT,

   UART_MAX_PORTS,
}uart_port_id;

typedef enum
{
   UART_SUCCESS = 0,
   UART_ERROR,
}uart_result;

typedef enum
{
  UART_5_BITS_PER_CHAR  = 0,
  UART_6_BITS_PER_CHAR  = 1,
  UART_7_BITS_PER_CHAR  = 2,
  UART_8_BITS_PER_CHAR  = 3,
} uart_bits_per_char;

typedef enum
{
  UART_0_5_STOP_BITS    = 0,
  UART_1_0_STOP_BITS    = 1,
  UART_1_5_STOP_BITS    = 2,
  UART_2_0_STOP_BITS    = 3,
} uart_num_stop_bits;

typedef enum
{
  UART_NO_PARITY        = 0,
  UART_ODD_PARITY       = 1,
  UART_EVEN_PARITY      = 2,
  UART_SPACE_PARITY     = 3,
} uart_parity_mode;

typedef void* uart_handle;

typedef void(*UART_CALLBACK)(uint32 num_bytes, void *cb_data);

/*==================================================================================================
                                             STRUCTURES
==================================================================================================*/

typedef struct
{
   uint32                baud_rate;
   uart_parity_mode      parity_mode;
   uart_num_stop_bits    num_stop_bits;
   uart_bits_per_char    bits_per_char;
   uint32                enable_loopback;
   uint32                enable_flow_ctrl;

   // The callbacks will be called from ISR context.
   // Necessary precautions needs to be taken in these funtions to make sure not ISR guidelines
   // are violated.
   // DONT call uart_transmit or uart_receive API from these callbacks.
   UART_CALLBACK         tx_cb_isr;
   UART_CALLBACK         rx_cb_isr;
}uart_open_config;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/


/*!
 * \brief Deinitializes the UART port
 *
 * Releases  clock, interrupt, and gpio handles related to this UART
 * Cancels any pending transfers
 *
 * DON'T call from ISR context
 *
 * \param in uart_handle Handle
 * \return UART_SUCCESS|UART_ERROR
 */

uart_result      uart_close(uart_handle h);

/*!
 * \brief Initializes UART port
 *
 * Opens the UART port and configures the corresponding clocks, interrupts, and gpio
 *
 * DON'T call from ISR context
 *
 * \param in uart_handle* h to get handle
 * \param in uart_port_id id
 * \param in uart_open_config* config structure that holds all config data
 * \return UART_SUCCESS|UART_ERROR
 */

uart_result      uart_open(uart_handle* h, uart_port_id id, uart_open_config* config);

/*!
 * \brief Queues the buffer provided for receiving the data
 *
 * Asynchronous call. The rx_cb_isr will be called when the rx transfer completes.
 * The buffer is owned by the UART driver till the rx_cb_isr is called.
 *
 * There has to be always a pending RX. UART HW has a limited buffer(FIFO) and if there is
 * no SW buffer available,
 *    For HS-UART, the flow control will de-assert the RFR line.
 *    For Debug UART, the data will be lost as there is no HW flow control lines available.
 *
 * Call uart_receive immediately after uart_open to queue a buffer.
 * After every rx_cb_isr, from a different non-ISR thread, queue the next transfer.
 *
 * There can be maximum of 2 buffers queued at a time.
 *
 * DON'T call from ISR context
 *
 * \param in uart_handle Handle
 * \param in char* buf Buffer to be filled with data.
 * \param in uint32 buf_size Size of the buffer being passed. Must be >=4 and a multiple of 4.
 * \param in void* cb_data Call back data to be passed when rx_cb_isr is called during rx completion
 * \return UART_SUCCESS|UART_ERROR
 */

uart_result      uart_receive(uart_handle h, char* buf, uint32 buf_size, void* cb_data);

/*!
 * \brief Transmits the data from given buffer.
 *
 * Asynchronous call. The buffer will be queued for TX and when transmit is completed, the
 * tx_cb_isr will be called.
 *
 * The buffer is owned by the UART driver till the tx_cb_isr is called.
 *
 * DON'T call from ISR context
 *
 * There can be maximum of 2 buffers queued at a time.
 *
 * \param in uart_handle Handle
 * \param in char* buf Buffer with data for transmit.
 * \param in uint32 bytes_to_tx bytes of data to transmit
 * \param in void* cb_data Call back data to be passed when tx_cb_isr is called during tx completion
 * \return UART_SUCCESS|UART_ERROR
 */

uart_result      uart_transmit(uart_handle h, char* buf, uint32 bytes_to_tx, void* cb_data);

#endif
