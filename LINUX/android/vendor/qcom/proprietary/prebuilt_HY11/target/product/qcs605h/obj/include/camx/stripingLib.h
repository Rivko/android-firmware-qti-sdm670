#ifndef __STRIPING_LIB_H__
#define __STRIPING_LIB_H__

//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  stripingLib.h
// @brief Common data structures of striping library
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
//
// 04/07/2017        etsairi       Initial version
//------------------------------------------------------------------------

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */

#include "fwdefs.h"

#include <stddef.h>     /* size_t */
#include <stdint.h>     /* int32_t */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(ALLOW_STRIPING_IN_FW)
#define STRIPING_LIB_API __declspec (dllexport)
#else
#define STRIPING_LIB_API
#endif

#define STRIPING_LIB_SUCCESS                    HLSTATUS_SUCCESS
#define STRIPING_LIB_INVALID_INPUT              1
#define STRIPING_LIB_OUT_OF_MEM                 2

#define STRIPING_LIB_GENERAL_ERROR              -1
#define STRIPING_LIB_IPE_INVALID_STRIPE_UBWC    -2

/*------------------------------------------------------------------------
*       Type Declarations
* ----------------------------------------------------------------------- */

/** Striping library handle, this is an opaque do not change! */
typedef struct _StripingLibContext *SL_HANDLE;

/** Function pointer for striping library memory allocation function. Return 0 in case of success, otherwise fails. */
typedef int32_t (*StripingLibAlloc)(void** ptr, size_t size);

/** Function pointer for striping library memory free function. Return 0 in case of success, otherwise fails. */
typedef int32_t (*StripingLibFree)(void** ptr);

#pragma pack(push)
#pragma pack(4)

/**< memory allocation function pointer struct */
typedef struct _StripingLibMemOp
{
    StripingLibAlloc    alloc;  /**< memory allocation function */
    StripingLibFree     free;   /**< memory free function */
}StripingLibMemOp;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* __STRIPING_LIB_H__ */
