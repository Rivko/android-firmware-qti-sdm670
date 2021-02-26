/** 
    @file  i2c_log.h
    @brief I2C logging interface
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __I2C_LOG_H__
#define __I2C_LOG_H__

#define LEVEL_DATA    0x00000001
#define LEVEL_INFO    0x00000004
#define LEVEL_ERROR   0x00000002
#define LEVEL_REGS    0x00000008

#define I2C_LOG_LEVEL

// to enable logs, set I2C_LOG_FLAGS to an OR of the above bits in scons
#ifdef I2C_LOG_LEVEL
#include<Library/DebugLib.h>
#define I2C_LOG(level, format_str, args...) \
        DEBUG((level, format_str"\n", ##args))
#else
#define I2C_LOG(level,args...)
#endif

#endif
