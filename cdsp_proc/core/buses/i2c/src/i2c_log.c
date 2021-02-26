/** 
    @file  i2c_log.c
    @brief logging implementation
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_types.h"
#include "i2c_log.h"

// micro ulog
#ifdef I2C_ENABLE_MICRO_ULOG
#include "micro_ULog.h"
#define I2C_UULOG_BUFFER_SIZE 2048
static uint8 i2c_uulog_buffer [I2C_UULOG_BUFFER_SIZE];
micro_ULogHandle i2c_uulog_handle;
static boolean i2c_uulog_init (void)
{
    uint32              actual_buf_size;
    micro_ULogResult    res1;

    res1 = micro_ULog_CreateLockableLog (&i2c_uulog_handle,
                                         "uI2C",
                                         (char*) i2c_uulog_buffer,
                                         &actual_buf_size,
                                         I2C_UULOG_BUFFER_SIZE);

    if ((res1 != MICRO_ULOG_SUCCESS) || (actual_buf_size == 0))
    {
        return FALSE;
    }

    return TRUE;
}
#endif

// ulog
#ifdef I2C_ENABLE_ULOG
#include "ULogFront.h"
#include "qurt_island.h"
static uint32 i2c_ulog_buffer_size = 16384;
ULogHandle i2c_ulog_handle;
static boolean i2c_ulog_init (void)
{
    ULogResult          res2;

    res2 = ULogFront_RealTimeInit(&i2c_ulog_handle,
                                  "I2C",
                                  i2c_ulog_buffer_size,
                                  ULOG_MEMORY_LOCAL,
                                  ULOG_LOCK_OS);
    if (!res2)
    {
        return FALSE;
    }

    return TRUE;
}
#endif

boolean i2c_log_init (void)
{
    boolean ulog_res    = TRUE;
    boolean uulog_res   = TRUE;

#ifdef I2C_ENABLE_ULOG
    ulog_res  = i2c_ulog_init();
#endif
#ifdef I2C_ENABLE_MICRO_ULOG
    uulog_res = i2c_uulog_init();
#endif

    return ulog_res && uulog_res;
}

void i2c_log (uint32 data_count, const char *format_str, ...)
{
    va_list args;
    va_start(args, format_str);
    if (qurt_island_get_status ())
    {
#ifdef I2C_ENABLE_MICRO_ULOG
        micro_ULog_RealTimeVprintf(i2c_uulog_handle,
                                   data_count,
                                   format_str,
                                   args);
#endif
    }
    else
    {
#ifdef I2C_ENABLE_ULOG
        ULogFront_RealTimeVprintf (i2c_ulog_handle,
                                   data_count,
                                   format_str,
                                   args);
#endif
    }
    va_end(args);
}
