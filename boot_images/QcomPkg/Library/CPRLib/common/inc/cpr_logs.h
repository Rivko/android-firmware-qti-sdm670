/**
 * @file:  cpr_logs.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/inc/cpr_logs.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_LOGS_H
#define CPR_LOGS_H

#include "cpr_image_logs.h"

#define CPR_LOG_LEVEL_MAX     CPR_LOG_LEVEL_TRACE
#define CPR_LOG_LEVEL_TRACE   6
#define CPR_LOG_LEVEL_VERBOSE 5
#define CPR_LOG_LEVEL_INFO    4
#define CPR_LOG_LEVEL_WARNING 3
#define CPR_LOG_LEVEL_ERROR   2
#define CPR_LOG_LEVEL_FATAL   1
#define CPR_LOG_LEVEL_NONE    0

// This comes from SConscript or Build file
#ifndef CPR_LOG_COMPILE_LEVEL
    #define CPR_LOG_COMPILE_LEVEL  CPR_LOG_LEVEL_NONE
#endif

// Not all images need initialization
#ifdef CPR_IMAGE_LOG_INIT
    #define CPR_LOG_INIT() CPR_IMAGE_LOG_INIT()
#else
    #define CPR_LOG_INIT()
#endif

#if (CPR_LOG_COMPILE_LEVEL >= CPR_LOG_LEVEL_FATAL)
    #define CPR_LOG_FATAL(format_str, args...) CPR_IMAGE_LOG_FATAL(format_str, ##args)
#else
    #define CPR_LOG_FATAL(format_str, args...)
#endif


#if (CPR_LOG_COMPILE_LEVEL >= CPR_LOG_LEVEL_ERROR)
    #define CPR_LOG_ERROR(format_str, args...) CPR_IMAGE_LOG_ERROR(format_str, ##args)
#else
    #define CPR_LOG_ERROR(format_str, args...)
#endif


#if (CPR_LOG_COMPILE_LEVEL >= CPR_LOG_LEVEL_WARNING)
    #define CPR_LOG_WARNING(format_str, args...) CPR_IMAGE_LOG_WARNING(format_str, ##args)
#else
    #define CPR_LOG_WARNING(format_str, args...)
#endif


#if (CPR_LOG_COMPILE_LEVEL >= CPR_LOG_LEVEL_INFO)
    #define CPR_LOG_INFO(format_str, args...) CPR_IMAGE_LOG_INFO(format_str, ##args)
#else
    #define CPR_LOG_INFO(format_str, args...)
#endif

#if (CPR_LOG_COMPILE_LEVEL >= CPR_LOG_LEVEL_VERBOSE)
    #define CPR_LOG_VERBOSE(format_str, args...) CPR_IMAGE_LOG_VERBOSE(format_str, ##args)
#else
    #define CPR_LOG_VERBOSE(format_str, args...)
#endif

#if (CPR_LOG_COMPILE_LEVEL >= CPR_LOG_LEVEL_TRACE)
    #define CPR_LOG_TRACE(format_str, args...) CPR_IMAGE_LOG_TRACE(format_str, ##args)

    // Suppresses newline if possible
    #define CPR_LOG_TRACE_RAW(format_str, args...) CPR_IMAGE_LOG_TRACE_RAW(format_str, ##args)
#else
    #define CPR_LOG_TRACE(format_str, args...)
    #define CPR_LOG_TRACE_RAW(format_str, args...)
#endif


#ifdef CPR_IMAGE_STATIC_LOG_INIT
    #define CPR_STATIC_LOG_INIT() CPR_IMAGE_STATIC_LOG_INIT()
#else
    #define CPR_STATIC_LOG_INIT()
#endif

#ifdef CPR_IMAGE_STATIC_LOG_UPDATE_ALL
    #define CPR_STATIC_LOG_UPDATE_ALL() CPR_IMAGE_STATIC_LOG_UPDATE_ALL()
#else
    #define CPR_STATIC_LOG_UPDATE_ALL()
#endif


#if defined CPR_IMAGE_STATIC_LOG_INIT && defined CPR_IMAGE_STATIC_LOG_CPR_INFO
    #define CPR_STATIC_LOG_CPR_INFO(cpr_info_ptr)   CPR_IMAGE_STATIC_LOG_CPR_INFO(cpr_info_ptr)
#else
    #define CPR_STATIC_LOG_CPR_INFO(...)
#endif

#if defined CPR_IMAGE_STATIC_LOG_INIT && defined CPR_IMAGE_STATIC_LOG_RAIL_INFO
    #define CPR_STATIC_LOG_RAIL_INFO(rail_state_ptr) CPR_IMAGE_STATIC_LOG_RAIL_INFO(rail_state_ptr)
#else
    #define CPR_STATIC_LOG_RAIL_INFO(...)
#endif

#if defined CPR_IMAGE_STATIC_LOG_INIT && defined CPR_IMAGE_STATIC_LOG_MODE_INFO
    #define CPR_STATIC_LOG_MODE_INFO(rail_state_ptr, mode_ptr, submode_ptr) CPR_IMAGE_STATIC_LOG_MODE_INFO(rail_state_ptr, mode_ptr, submode_ptr)
#else
    #define CPR_STATIC_LOG_MODE_INFO(...)
#endif

#if defined CPR_IMAGE_STATIC_LOG_INIT && defined CPR_IMAGE_STATIC_LOG_ISR_INFO
    #define CPR_STATIC_LOG_ISR_INFO(rail_state_ptr, mode_ptr, submode_ptr) CPR_IMAGE_STATIC_LOG_ISR_INFO(rail_state_ptr, mode_ptr, submode_ptr)
#else
    #define CPR_STATIC_LOG_ISR_INFO(...)
#endif

#endif
