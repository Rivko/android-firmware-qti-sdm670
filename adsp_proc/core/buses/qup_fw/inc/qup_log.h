/** 
    @file  qup_log.h
    @brief QUP logging interface
 */
/*=============================================================================
            Copyright (c) 2016 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __QUP_LOG_H__
#define __QUP_LOG_H__

#define LEVEL_DATA    0x00000001
#define LEVEL_INFO    0x00000004
#define LEVEL_ERROR   0x00000002
#define LEVEL_REGS    0x00000008

// to enable logs, set QUP_LOG_FLAGS to an OR of the above bits in scons
#ifdef QUP_LOG_LEVEL
#include "qurt_printf.h"
#define QUP_LOG(level,args...) \
    if (level & QUP_LOG_LEVEL) \
    {                          \
        qurt_printf (args);    \
    }
#else
#define QUP_LOG(level,args...)
#endif

#endif
