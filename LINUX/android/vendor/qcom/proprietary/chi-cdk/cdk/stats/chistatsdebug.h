////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chistatsdebug.h
/// @brief Statistics debug information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef CHISTATSDEBUG_H
#define CHISTATSDEBUG_H

#include "camxcdktypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// The verbosity tag for a given debug print message
typedef UINT32 StatsLog;
static const StatsLog StatsLogDebug             = (1 << 0);     ///< Debug messages
static const StatsLog StatsLogError             = (1 << 1);     ///< Error messages
static const StatsLog StatsLogWarning           = (1 << 2);     ///< Warning messages
static const StatsLog StatsLogEntryExit         = (1 << 3);     ///< Entry/Exit messages
static const StatsLog StatsLogInfo              = (1 << 4);     ///< Informational messages
static const StatsLog StatsLogVerbose           = (1 << 5);     ///< Verbose messages
static const StatsLog StatsLogMax               = (1 << 6);     ///< Max verbosity levels

// The group tag for a given debug print message
typedef UINT32 StatsLogGroup;
static const StatsLogGroup StatsLogGroupNone    = (1 << 0);     ///< Generic group
static const StatsLogGroup StatsLogGroupAEC     = (1 << 1);     ///< AEC
static const StatsLogGroup StatsLogGroupAF      = (1 << 2);     ///< AF
static const StatsLogGroup StatsLogGroupAWB     = (1 << 3);     ///< AWB
static const StatsLogGroup StatsLogGroupAFD     = (1 << 4);     ///< AFD
static const StatsLogGroup StatsLogGroupASD     = (1 << 5);     ///< ASD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Define the logging function
///
/// @param  pFileName       Name of the logging file from which logging is invoked
/// @param  lineNumber      Line number of the logging file from which logging is invoked
/// @param  pFunctionName   Function name from which logging is invoked
/// @param  group           Logging group
/// @param  level           Log level
/// @param  format          Format string
/// @param  ...             Parameters required by format
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*StatsLoggingFunction)(
    const CHAR* pFileName,
    const INT lineNumber,
    const CHAR* pFunctionName,
    StatsLogGroup group,
    StatsLog level,
    const CHAR* pFormat,
    ...);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHISTATSDEBUG_H
