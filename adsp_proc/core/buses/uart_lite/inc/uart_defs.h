#ifndef UART_DEFS_H
#define UART_DEFS_H
/*==================================================================================================

FILE: Uart_defs.h

DESCRIPTION: This module provides the driver software for the UART.

Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/uart_lite/inc/uart_defs.h#4 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/31/17   RC      Updated the structure contents and buffer types.
12/03/14   VV      Included cb_data
09/25/13   VV      Included DAL environment support.
06/24/13   VV      Initial revision.

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "uart.h"
#include "qurt_mutex.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef struct
{
   uart_port_id port_id;
   uint32 uart_base_phy;
   volatile uint32 uart_base;

   uint32 gpio_tx_config;
   uint32 gpio_rx_config;
   uint32 gpio_cts_config;
   uint32 gpio_rfr_config;

   void*  bus_clock_id;
   void*  core_clock_id;

   uint16 irq_num;
   uint32 tcsr_base;
   uint32 tcsr_offset;
   uint32 tcsr_value;

   uint16 wakeup_irq_num;
}UART_PROPERTIES;

typedef struct uart_context
{
   uint32               port_id;
   boolean              is_port_open;
   UART_PROPERTIES*     properties;
   uart_open_config     open_config;

   uint8*               tx_buf;
   uint32               bytes_to_tx;
   uint32               bytes_txed;
   void*                tx_cb_data;
   volatile void*       tx_signal;


   uint8*               rx_buf;
   uint32               rx_write_offset;
   uint32               rx_buf_size;
   void*                rx_cb_data;

   void*                clock_handle;
   void*                tlmm_handle;
   void*                interrupt_context;
   qurt_mutex_t         uart_mutex;

   boolean              wakeup_interrupt_registered;
   boolean              wakeup_interrupt_enabled;
   UART_WAKEUP_CALLBACK wakeup_cb;
   void*                wakeup_cb_data;

   boolean              clock_on;
   uint32               prev_rx_intr_en_val;

   /* Make it volatile. Will be used as while() condition */
   volatile boolean     isr_running;
}uart_context;

#endif
