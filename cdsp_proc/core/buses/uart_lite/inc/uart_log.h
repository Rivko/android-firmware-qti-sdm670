#ifndef UART_LOG_H
#define UART_LOG_H
/*==================================================================================================

FILE: Uart_log.h

DESCRIPTION: Defines the logging feature APIs for the UART driver.

Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/

/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart_lite/inc/uart_log.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/10/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "uart_defs.h"

void uart_log (uint32 data_count, const char *format_str, ...);

#define UART_LOG_NUM_ARGS(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) (N - 1)
#define UART_LOG(level, msg, args...) UART_LOG_##level(msg, ##args)

#ifdef UART_LOG_ALL
#define UART_LOG_INFO(...) uart_log(UART_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define UART_LOG_INFO(...)
#endif

#if ( defined(UART_LOG_ALL) || defined(UART_LOG_ERROR))
#define UART_LOG_ERROR(...) uart_log(UART_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define UART_LOG_ERROR(...)
#endif

#endif


