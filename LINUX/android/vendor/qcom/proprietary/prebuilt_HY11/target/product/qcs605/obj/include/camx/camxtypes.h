////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxtypes.h
/// @brief CamX abstraction of common types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE NC004c: Converting CSL header to C, we need typedef
// NOWHINE FILE GR017: Typedefing prohibited types in this file

#ifndef CAMXTYPES_H
#define CAMXTYPES_H

#include <stddef.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Linux Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LINUX)
#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef int             BOOL;
typedef char            CHAR;
typedef uint8_t         BYTE;
typedef unsigned char   UCHAR;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef int             INT;
typedef unsigned int    UINT;
typedef float           FLOAT;

typedef uint8_t         UINT8;
typedef int8_t          INT8;
typedef uint16_t        UINT16;
typedef int16_t         INT16;
typedef uint32_t        UINT32;
typedef int32_t         INT32;
typedef uint64_t        UINT64;
typedef int64_t         INT64;

typedef max_align_t     MAXALIGN_T;
typedef size_t          SIZE_T;
typedef void            VOID;

#if !defined (TRUE)
#define TRUE                1
#endif // !defined (TRUE)

#if !defined (FALSE)
#define FALSE               0
#endif // !defined (FALSE)

#ifdef __cplusplus
}
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Windows Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined (_WIN32)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN 1
#endif // !defined(WIN32_LEAN_AND_MEAN)

#include <Windows.h>

#ifdef __cplusplus
#include <cstddef>
typedef std::max_align_t    MAXALIGN_T;
#endif // __cplusplus

#else

// #error Unsupported target

#endif // defined (_LINUX)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS Agnostic Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned int    BIT;
typedef double          DOUBLE;

typedef intptr_t        INTPTR_T;
typedef uintptr_t       UINTPTR_T;
typedef ptrdiff_t       PTRDIFF_T;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Result Codes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The result code type to be returned from functions. Must match ordering and size CamxResultStrings.
typedef INT32               CamxResult;
static const CamxResult CamxResultSuccess           = 0;    ///< Operation was successful
static const CamxResult CamxResultEFailed           = 1;    ///< Operation encountered unspecified error
static const CamxResult CamxResultEUnsupported      = 2;    ///< Operation is not supported
static const CamxResult CamxResultEInvalidState     = 3;    ///< Invalid state
static const CamxResult CamxResultEInvalidArg       = 4;    ///< Invalid argument
static const CamxResult CamxResultEInvalidPointer   = 5;    ///< Invalid memory pointer
static const CamxResult CamxResultENoSuch           = 6;    ///< No such item exists or is valid
static const CamxResult CamxResultEOutOfBounds      = 7;    ///< Out of bounds
static const CamxResult CamxResultENoMemory         = 8;    ///< Out of memory
static const CamxResult CamxResultETimeout          = 9;    ///< Operation timed out
static const CamxResult CamxResultENoMore           = 10;   ///< No more items available
static const CamxResult CamxResultENeedMore         = 11;   ///< Operation requires more
static const CamxResult CamxResultEExists           = 12;   ///< Item exists
static const CamxResult CamxResultEPrivLevel        = 13;   ///< Privileges are insufficient for requested operation
static const CamxResult CamxResultEResource         = 14;   ///< Resources are insufficient for requested operation
static const CamxResult CamxResultEUnableToLoad     = 15;   ///< Unable to load library/object
static const CamxResult CamxResultEInProgress       = 16;   ///< Operation is already in progress
static const CamxResult CamxResultETryAgain         = 17;   ///< Could not complete request; try again
static const CamxResult CamxResultEBusy             = 18;   ///< Device or resource busy
static const CamxResult CamxResultEReentered        = 19;   ///< Non re-entrant API re-entered
static const CamxResult CamxResultEReadOnly         = 20;   ///< Cannot change read-only object or parameter
static const CamxResult CamxResultEOverflow         = 21;   ///< Value too large for defined data type
static const CamxResult CamxResultEOutOfDomain      = 22;   ///< Math argument or result out of domain
static const CamxResult CamxResultEInterrupted      = 23;   ///< Waitable call is interrupted
static const CamxResult CamxResultEWouldBlock       = 24;   ///< Operation would block
static const CamxResult CamxResultETooManyUsers     = 25;   ///< Too many users
static const CamxResult CamxResultENotImplemented   = 26;   ///< Function or method is not implemented
static const CamxResult CamxResultEDisabled         = 27;   ///< Feature disabled (by an override for example)
// Increment the count below if adding additional result codes.
static const CamxResult CamxResultCount             = 28;   ///< The number of result codes. Not to be used.

/// @brief The string name of the type of the memory allocation. Must be in order of CamxResult codes.
#if __GNUC__
static const CHAR* CamxResultStrings[] __attribute__((unused)) =
#else // __GNUC__
static const CHAR* CamxResultStrings[] =
#endif // _GNUC__
{
    "CamxResultSuccess",
    "CamxResultEFailed",
    "CamxResultEUnsupported",
    "CamxResultEInvalidState",
    "CamxResultEInvalidArg",
    "CamxResultEInvalidPointer",
    "CamxResultENoSuch",
    "CamxResultEOutOfBounds",
    "CamxResultENoMemory",
    "CamxResultETimeout",
    "CamxResultENoMore",
    "CamxResultENeedMore",
    "CamxResultEExists",
    "CamxResultEPrivLevel",
    "CamxResultEResource",
    "CamxResultEUnableToLoad",
    "CamxResultEInProgress",
    "CamxResultETryAgain",
    "CamxResultEBusy",
    "CamxResultEReentered",
    "CamxResultEReadOnly",
    "CamxResultEOverflow",
    "CamxResultEOutOfDomain",
    "CamxResultEInterrupted",
    "CamxResultEWouldBlock",
    "CamxResultETooManyUsers",
    "CamxResultENotImplemented",
    "CamxResultEDisabled"
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief The type of the memory allocation. Must match ordering and size CamxMemTypeStrings.
typedef INT32               CamxMemType;
static const CamxMemType    CamxMemTypeAny      = 0;
/// @todo (CAMX-431): Add additional memory types here
// Increment the count below if adding additional memory types.
static const CamxMemType    CamxMemTypeCount    = 1;   ///< The number of memory types. Not to be used.

/// @brief The string name of the type of the memory allocation. Must match ordering and size of CamxMemType.
#if __GNUC__
static const CHAR* CamxMemTypeStrings[] __attribute__((unused)) =
#else // __GNUC__
static const CHAR* CamxMemTypeStrings[] =
#endif // _GNUC__
{
    "CamxMemTypeAny",   ///< Generic allocation with no associated type
    /// @todo (CAMX-431): Add additional memory types here
};

/// @brief Specifies the read/write protection for a buffer
struct CamxProtectionType
{
    BOOL read;  ///< Memory allocation is being read from
    BOOL write; ///< Memory allocation is being written to
};

static const CamxProtectionType CamxNoProtection    = { 0, 0 };
static const CamxProtectionType CamxReadOnly        = { 1, 0 };
static const CamxProtectionType CamxWriteOnly       = { 0, 1 };
static const CamxProtectionType CamxReadWrite       = { 1, 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log Codes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The verbosity tag for a given debug print message
typedef UINT32 CamxLog;
static const CamxLog CamxLogDebug                   = 0;            ///< Debug messages
static const CamxLog CamxLogError                   = 1;            ///< Error messages
static const CamxLog CamxLogWarning                 = 2;            ///< Warning messages
static const CamxLog CamxLogEntryExit               = 3;            ///< Entry/Exit messages
static const CamxLog CamxLogInfo                    = 4;            ///< Informational messages
static const CamxLog CamxLogVerbose                 = 5;            ///< Verbose messages
static const CamxLog CamxLogPerfInfo                = 6;            ///< Performance info message
static const CamxLog CamxLogPerfWarning             = 7;            ///< Performance warning message
static const CamxLog CamxLogDRQ                     = 8;            ///< DRQ logging
static const CamxLog CamxLogMeta                    = 9;            ///< Metadata logging
static const CamxLog CamxLogMax                     = 10;           ///< Max verbosity levels

// The group tag for a given debug print message
typedef UINT32 CamxLogGroup;
static const CamxLogGroup CamxLogGroupNone          = (1 << 0);     ///< Generic group
static const CamxLogGroup CamxLogGroupSensor        = (1 << 1);     ///< Sensor
static const CamxLogGroup CamxLogGroupIFace         = (1 << 2);     ///< IFace
static const CamxLogGroup CamxLogGroupISP           = (1 << 3);     ///< ISP
static const CamxLogGroup CamxLogGroupPProc         = (1 << 4);     ///< Post Processor
static const CamxLogGroup CamxLogGroupImgLib        = (1 << 5);     ///< Image Lib
static const CamxLogGroup CamxLogGroupCPP           = (1 << 6);     ///< CPP
static const CamxLogGroup CamxLogGroupHAL           = (1 << 7);     ///< HAL
static const CamxLogGroup CamxLogGroupJPEG          = (1 << 8);     ///< JPEG
static const CamxLogGroup CamxLogGroupStats         = (1 << 9);     ///< Stats
static const CamxLogGroup CamxLogGroupCSL           = (1 << 10);    ///< CSL
static const CamxLogGroup CamxLogGroupApp           = (1 << 11);    ///< Application
static const CamxLogGroup CamxLogGroupUtils         = (1 << 12);    ///< Utilities
static const CamxLogGroup CamxLogGroupSync          = (1 << 13);    ///< Sync
static const CamxLogGroup CamxLogGroupMemSpy        = (1 << 14);    ///< MemSpy
static const CamxLogGroup CamxLogGroupAssert        = (1 << 15);    ///< Asserts
static const CamxLogGroup CamxLogGroupCore          = (1 << 16);    ///< Core
static const CamxLogGroup CamxLogGroupHWL           = (1 << 17);    ///< HWL
static const CamxLogGroup CamxLogGroupChi           = (1 << 18);    ///< CHI
static const CamxLogGroup CamxLogGroupDRQ           = (1 << 19);    ///< DRQ
static const CamxLogGroup CamxLogGroupFD            = (1 << 20);    ///< FD
static const CamxLogGroup CamxLogGroupIQMod         = (1 << 21);    ///< IQ module
static const CamxLogGroup CamxLogGroupLRME          = (1 << 22);    ///< LRME
static const CamxLogGroup CamxLogGroupNCS           = (1 << 23);    ///< NCS
static const CamxLogGroup CamxLogGroupMeta          = (1 << 24);    ///< Metadata
static const CamxLogGroup CamxLogGroupAEC           = (1 << 25);    ///< AEC
static const CamxLogGroup CamxLogGroupAWB           = (1 << 26);    ///< AWB
static const CamxLogGroup CamxLogGroupAF            = (1 << 27);    ///< AF

/// @brief Trace enablement structure
struct CamxTraceInfo
{
    CamxLogGroup    groupsEnable;        ///< Tracing groups enable bits
    BOOL            traceErrorLogEnable; ///< Enable tracing for error logs
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Debug print settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct DebugLogInfo
{
    CamxLogGroup    groupsEnable[CamxLogMax];               ///< Logging groups enable bits per log level
    FILE*           pDebugLogFile;                          ///< Debug log filehandle or NULL when disabled
    BOOL            systemLogEnable;                        ///< Global logging enable flag
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timing Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Time structure
struct CamxTime
{
    UINT32  seconds;                    ///< Time in whole seconds
    UINT32  nanoSeconds;                ///< Fractional nanoseconds
};

/// @brief Date time structure
struct CamxDateTime
{
    UINT32  seconds;                    ///< Seconds after the minute 0-61*
    UINT32  microseconds;               ///< Microseconds 0-999999
    UINT32  minutes;                    ///< Time in minutes 0-59
    UINT32  hours;                      ///< Time in hours 0-23
    UINT32  dayOfMonth;                 ///< Day of month 1-31
    UINT32  month;                      ///< Months since January 0-11, where 0 is January
    UINT32  year;                       ///< Year since 1900
    UINT32  weekday;                    ///< Day of week 0-6
    UINT32  yearday;                    ///< Day of year 0-365
    UINT32  dayLightSavingTimeFlag;     ///< Day light saving time flag
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Miscellaneous common types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Rational number type
struct CamxRational
{
    INT32 numerator;    ///< Rational number numerator
    INT32 denominator;  ///< Rational number denominator
};

/// @brief dimension type
struct CamxDimension
{
    UINT32 width;    ///< width
    UINT32 height;   ///< height
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CAMXTYPES_H
