/** 
    @file  SPI_log.h
    @brief SPI logging interface
 */
/*=============================================================================
            Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_LOG_H__
#define __SPI_LOG_H__

//#define LEVEL_DATA    0x00000001
#define LEVEL_INFO    0x00000040
#define LEVEL_ERROR   0x80000000
//#define LEVEL_REGS    0x00000008

#define SPI_LOG_LEVEL

// to enable logs, set SPI_LOG_FLAGS to an OR of the above bits in scons
#ifdef SPI_LOG_LEVEL
#include<Library/DebugLib.h>
#define SPI_LOG(level, format_str, args...) \
        DEBUG((level, format_str"\n", ##args))
#else
#define SPI_LOG(level,args...)
#endif

#endif
