/** 
    @file  spi_log.c
    @brief logging implementation
 */
/*=============================================================================
            Copyright (c) 2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_log.h"

// micro ulog
#ifdef SPI_ENABLE_MICRO_ULOG
#include "micro_ULog.h"
#define SPI_UULOG_BUFFER_SIZE 2048
static uint8 spi_uulog_buffer [SPI_UULOG_BUFFER_SIZE];
micro_ULogHandle spi_uulog_handle;
static boolean spi_uulog_init (void)
{
    uint32              actual_buf_size;
    micro_ULogResult    res1;

    res1 = micro_ULog_CreateLockableLog (&spi_uulog_handle,
                                 "uSPI",
                                 (char*) spi_uulog_buffer,
                                 &actual_buf_size,
                                 SPI_UULOG_BUFFER_SIZE);

    if ((res1 != MICRO_ULOG_SUCCESS) || (actual_buf_size == 0))
    {
        return FALSE;
    }

    return TRUE;
}
#endif

// ulog
#ifdef SPI_ENABLE_ULOG
#include "ULogFront.h"
#include "qurt_island.h"
static uint32 spi_ulog_buffer_size = 16384;
ULogHandle spi_ulog_handle;
static boolean spi_ulog_init (void)
{
    ULogResult          res2;

    res2 = ULogFront_RealTimeInit(&spi_ulog_handle,
                                  "SPI",
                                  spi_ulog_buffer_size,
                                  ULOG_MEMORY_LOCAL,
                                  ULOG_LOCK_OS);
    if (!res2)
    {
        return FALSE;
    }

    return TRUE;
}
#endif

boolean spi_log_init (void)
{
    boolean ulog_res    = TRUE;
    boolean uulog_res   = TRUE;

#ifdef SPI_ENABLE_ULOG
    ulog_res  = spi_ulog_init();
#endif
#ifdef SPI_ENABLE_MICRO_ULOG
    uulog_res = spi_uulog_init();
#endif

    return ulog_res && uulog_res;
}

void spi_log (uint32 data_count, const char *format_str, ...)
{
    va_list args;
    va_start(args, format_str);
    if (qurt_island_get_status ())
    {
#ifdef SPI_ENABLE_MICRO_ULOG
        micro_ULog_RealTimeVprintf(spi_uulog_handle,
                                   data_count,
                                   format_str,
                                   args);
#endif
    }
    else
    {
#ifdef SPI_ENABLE_ULOG
        ULogFront_RealTimeVprintf (spi_ulog_handle,
                                   data_count,
                                   format_str,
                                   args);
#endif
    }
    va_end(args);
}
