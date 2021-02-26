#ifndef UART_OS_H
#define UART_OS_H
/*==================================================================================================

FILE: uart_os.h

DESCRIPTION: This module provides the OS specific APIs to the UART driver software.

Copyright (c) 2016 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UartQupv3Lib/UartOs.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/7/16    VV       Initial revision

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "UartApi.h"
#include "UartDefs.h"

void        uart_busy_wait(uint32 uSecs);
uart_result uart_clock_close(uart_context* h);
uart_result uart_clock_open(uart_context* h,uint32 input_freq);
uart_result uart_get_driver_properties(void);
uart_result uart_get_properties(uart_context* h);
uart_result uart_interrupt_close(uart_context* h);
uart_result uart_interrupt_done(uart_context* h);
uart_result uart_interrupt_open(uart_context* h, void* isr);
uart_result uart_tlmm_open(uart_context* h);
uart_result uart_tlmm_close(uart_context* h);

#endif

