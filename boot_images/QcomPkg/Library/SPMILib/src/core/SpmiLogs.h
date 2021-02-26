/**
 * @file: SpmiLogs.h
 * 
 * @brief: This module provides common logging functionality for SPMI driver
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/SpmiLogs.h#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#ifndef __SPMI_LOGS_H_
#define __SPMI_LOGS_H_

#include "SpmiOsLogs.h"

#define SPMI_LOG_LEVEL_MAX     SPMI_LOG_LEVEL_TRACE
#define SPMI_LOG_LEVEL_TRACE   6
#define SPMI_LOG_LEVEL_VERBOSE 5
#define SPMI_LOG_LEVEL_INFO    4
#define SPMI_LOG_LEVEL_WARNING 3
#define SPMI_LOG_LEVEL_ERROR   2
#define SPMI_LOG_LEVEL_FATAL   1
#define SPMI_LOG_LEVEL_NONE    0

// This comes from SConscript or Builds file
#ifndef SPMI_LOG_COMPILE_LEVEL
    #define SPMI_LOG_COMPILE_LEVEL  SPMI_LOG_LEVEL_WARNING
    #warning "Defaulting SPMI log level to WARNING"
#endif

// Not all platforms need initialization
#ifdef SPMI_OS_LOG_INIT
    #define SPMI_LOG_INIT() SPMI_OS_LOG_INIT()
#else
    #define SPMI_LOG_INIT()
#endif

#if (SPMI_LOG_COMPILE_LEVEL >= SPMI_LOG_LEVEL_FATAL)
    #define SPMI_LOG_FATAL(format_str, args...) SPMI_OS_LOG_FATAL(format_str, ##args)
#else
    #define SPMI_LOG_FATAL(format_str, args...)
#endif


#if (SPMI_LOG_COMPILE_LEVEL >= SPMI_LOG_LEVEL_ERROR)
    #define SPMI_LOG_ERROR(format_str, args...) SPMI_OS_LOG_ERROR(format_str, ##args)
#else
    #define SPMI_LOG_ERROR(format_str, args...)
#endif


#if (SPMI_LOG_COMPILE_LEVEL >= SPMI_LOG_LEVEL_WARNING)
    #define SPMI_LOG_WARNING(format_str, args...) SPMI_OS_LOG_WARNING(format_str, ##args)
#else
    #define SPMI_LOG_WARNING(format_str, args...)
#endif


#if (SPMI_LOG_COMPILE_LEVEL >= SPMI_LOG_LEVEL_INFO)
    #define SPMI_LOG_INFO(format_str, args...) SPMI_OS_LOG_INFO(format_str, ##args)
#else
    #define SPMI_LOG_INFO(format_str, args...)
#endif

#if (SPMI_LOG_COMPILE_LEVEL >= SPMI_LOG_LEVEL_VERBOSE)
    #define SPMI_LOG_VERBOSE(format_str, args...) SPMI_OS_LOG_VERBOSE(format_str, ##args)
#else
    #define SPMI_LOG_VERBOSE(format_str, args...)
#endif

#if (SPMI_LOG_COMPILE_LEVEL >= SPMI_LOG_LEVEL_TRACE)
    #define SPMI_LOG_TRACE(format_str, args...) SPMI_OS_LOG_TRACE(format_str, ##args)
    // Suppresses newline if possible
    #define SPMI_LOG_TRACE_RAW(format_str, args...) SPMI_OS_LOG_TRACE_RAW(format_str, ##args)
#else
    #define SPMI_LOG_TRACE(format_str, args...)
    #define SPMI_LOG_TRACE_RAW(format_str, args...)
#endif

#endif
