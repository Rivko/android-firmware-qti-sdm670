////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxcdktypes.h
/// @brief CDK common types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR017: Prohibited intrinsic type

#ifndef CAMXSCDKTYPES_H
#define CAMXSCDKTYPES_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Linux Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LINUX)
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef int                 BOOL;
typedef char                CHAR;
typedef uint8_t             SBYTE;
typedef unsigned char       UCHAR;
typedef int                 INT;
typedef unsigned int        UINT;
typedef float               FLOAT;

typedef uint8_t             UINT8;
typedef int8_t              INT8;
typedef uint16_t            UINT16;
typedef int16_t             INT16;
typedef uint32_t            UINT32;
typedef int32_t             INT32;
typedef uint64_t            UINT64;
typedef int64_t             INT64;

typedef max_align_t         MAXALIGN_T;
typedef size_t              SIZE_T;
typedef void                VOID;

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
#include <stddef.h>

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
// Result Codes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The result code type to be returned from functions. Must match ordering and size CamxResultStrings.
typedef INT32               CDKResult;
static const CDKResult CDKResultSuccess           = 0;    ///< Operation was successful
static const CDKResult CDKResultEFailed           = 1;    ///< Operation encountered unspecified error
static const CDKResult CDKResultEUnsupported      = 2;    ///< Operation is not supported
static const CDKResult CDKResultEInvalidState     = 3;    ///< Invalid state
static const CDKResult CDKResultEInvalidArg       = 4;    ///< Invalid argument
static const CDKResult CDKResultEInvalidPointer   = 5;    ///< Invalid memory pointer
static const CDKResult CDKResultENoSuch           = 6;    ///< No such item exists or is valid
static const CDKResult CDKResultEOutOfBounds      = 7;    ///< Out of bounds
static const CDKResult CDKResultENoMemory         = 8;    ///< Out of memory
static const CDKResult CDKResultETimeout          = 9;    ///< Operation timed out
static const CDKResult CDKResultENoMore           = 10;   ///< No more items available
static const CDKResult CDKResultENeedMore         = 11;   ///< Operation requires more
static const CDKResult CDKResultEExists           = 12;   ///< Item exists
static const CDKResult CDKResultEPrivLevel        = 13;   ///< Privileges are insufficient for requested operation
static const CDKResult CDKResultEResource         = 14;   ///< Resources are insufficient for requested operation
static const CDKResult CDKResultEUnableToLoad     = 15;   ///< Unable to load library/object
static const CDKResult CDKResultEInProgress       = 16;   ///< Operation is already in progress
static const CDKResult CDKResultETryAgain         = 17;   ///< Could not complete request; try again
static const CDKResult CDKResultEBusy             = 18;   ///< Device or resource busy
static const CDKResult CDKResultEReentered        = 19;   ///< Non re-entrant API re-entered
static const CDKResult CDKResultEReadOnly         = 20;   ///< Cannot change read-only object or parameter
static const CDKResult CDKResultEOverflow         = 21;   ///< Value too large for defined data type
static const CDKResult CDKResultEOutOfDomain      = 22;   ///< Math argument or result out of domain
static const CDKResult CDKResultEInterrupted      = 23;   ///< Waitable call is interrupted
static const CDKResult CDKResultEWouldBlock       = 24;   ///< Operation would block
static const CDKResult CDKResultETooManyUsers     = 25;   ///< Too many users
static const CDKResult CDKResultENotImplemented   = 26;   ///< Function or method is not implemented
// Increment the count below if adding additional result codes.
static const CDKResult CDKResultCount             = 27;   ///< The number of result codes. Not to be used.

static const UINT CDKInvalidId                    = 0xFFFFFFFF;   ///< Defining an Invalid Handle


/// @brief Type of framework - chi override ID remapping to use
enum CameraIdRemapMode
{
    IdRemapCamera = 0,    ///< Remap logic for camera functionality used
    IdRemapTorch = 1      ///< Remap logic for torch functionality used
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS Agnostic Primitive Data Type Abstractions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned int    BIT;
typedef double          DOUBLE;

typedef intptr_t        INTPTR_T;
typedef uintptr_t       UINTPTR_T;
typedef ptrdiff_t       PTRDIFF_T;

#ifdef __cplusplus
}
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDK interface data structure packing information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CDK_PACK_SIZE 8

#define CDK_UNUSED_PARAM(x) (VOID)x

#if defined(__GNUC__)
#define CDK_VISIBILITY_PUBLIC __attribute__ ((visibility ("default")))
#define CDK_VISIBILITY_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define CDK_VISIBILITY_PUBLIC __declspec(dllexport)
#define CDK_VISIBILITY_LOCAL
#endif // defined(__GNUC__)

#endif // CAMXSCDKTYPES_H