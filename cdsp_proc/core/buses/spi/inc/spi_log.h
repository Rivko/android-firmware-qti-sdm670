/** 
    @file  spi_log.h
    @brief SPI logging
 */
/*=============================================================================
            Copyright (c) 2016-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_LOG_H__
#define __SPI_LOG_H__

#include "comdef.h"

// limit arguments to 10
#define SPI_LOG_NUM_ARGS(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) (N - 1)
#define SPI_LOG(level, msg, args...) SPI_LOG_##level(msg, ##args)

#ifdef SPI_LOG_ENABLE_DATA
#define SPI_LOG_LEVEL_DATA(...) spi_log(SPI_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define SPI_LOG_LEVEL_DATA(...)
#endif

#ifdef SPI_LOG_ENABLE_ERROR
#define SPI_LOG_LEVEL_ERROR(...) spi_log(SPI_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define SPI_LOG_LEVEL_ERROR(...)
#endif

#ifdef SPI_LOG_ENABLE_INFO
#define SPI_LOG_LEVEL_INFO(...) spi_log(SPI_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define SPI_LOG_LEVEL_INFO(...)
#endif

#ifdef SPI_LOG_ENABLE_REGS
#define SPI_LOG_LEVEL_REGS(...) spi_log(SPI_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define SPI_LOG_LEVEL_REGS(...)
#endif

#ifdef SPI_LOG_ENABLE_PERF
#define SPI_LOG_LEVEL_PERF(...)
#else
#define SPI_LOG_LEVEL_PERF(...)
#endif

#ifdef SPI_LOG_ENABLE_VERBOSE
#define SPI_LOG_LEVEL_VERBOSE(...) spi_log(SPI_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define SPI_LOG_LEVEL_VERBOSE(...)
#endif

boolean spi_log_init(void);
void spi_log (uint32 data_count, const char *format_str, ...);

#endif /*__SPI_LOG_H__*/
