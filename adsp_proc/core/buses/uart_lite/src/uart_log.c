#if ( defined(UART_LOG_ALL) || defined(UART_LOG_ERROR) )
/*==================================================================================================

FILE: Uart_log.c

DESCRIPTION: This module provides the logging feature for the UART driver Software.

Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/

/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/uart_lite/src/uart_log.c#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/10/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "uart_log.h"
#include "ULogFront.h"
#include <stdarg.h>
#include "qurt_island.h"

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static uint32 uart_ulog_buffer_size = 8 * 1024;
ULogHandle uart_ulog_handle;

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

static boolean uart_ulog_init (void)
{
    ULogResult          res2;

    res2 = ULogFront_RealTimeInit(&uart_ulog_handle,
                                  "UART",
                                  uart_ulog_buffer_size,
                                  ULOG_MEMORY_LOCAL,
                                  ULOG_LOCK_OS);
    if (!res2)
    {
        return FALSE;
    }

    return TRUE;
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

void uart_log (uint32 data_count, const char *format_str, ...)
{
   va_list args;
   va_start(args, format_str);

   if (!qurt_island_get_status ())
   {
      if(!uart_ulog_handle)
      {
         if(!uart_ulog_init())
         {
            return;
         }
      }

      ULogFront_RealTimeVprintf (uart_ulog_handle,
                                 data_count,
                                 format_str,
                                 args);
   }
   va_end(args);
}

#endif
