#ifndef UART_OS_H
#define UART_OS_H
/*==================================================================================================

FILE: uart_os.h

DESCRIPTION: This module provides the OS specific APIs to the UART driver software.

Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/uart_lite/inc/uart_os.h#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Included interrupt APIs to support driver on DAL env.
06/24/13   VV      Initial revision.

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "uart.h"
#include "uart_defs.h"

void        uart_busy_wait(uint32 uSecs);
uart_result uart_clock_close(uart_context* h);
uart_result uart_clock_open(uart_context* h,uint32 input_freq);
uart_result uart_get_driver_properties(void);
uart_result uart_get_properties(uart_context* h);
uart_result uart_interrupt_close(uart_context* h);
uart_result uart_interrupt_done(uart_context* h);
uart_result uart_interrupt_open(uart_context* h, void* isr, boolean enable_island_mode);
uart_result uart_tlmm_open(uart_context* h);
uart_result uart_tlmm_close(uart_context* h);
void uart_mutex_init(uart_context* h);
void uart_mutex_lock(uart_context* h);
void uart_mutex_unlock(uart_context* h);
void uart_mutex_destroy(uart_context* h);

uart_result uart_wakeup_interrupt_register(uart_context *h,
                                           void* isr,
                                           boolean enable_at_pdc,
                                           boolean enable_interrupt,
                                           boolean island_intr);

uart_result uart_wakeup_interrupt_unregister(uart_context *h, boolean disable_at_pdc);

#endif

