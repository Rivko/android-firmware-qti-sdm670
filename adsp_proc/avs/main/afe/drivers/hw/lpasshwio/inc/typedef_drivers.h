/**
    Defines basic data types for HAL implementations.

    Copyright (c) 2008 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

when       who     what, where, why
------     ---    -------------------------------------------------------
10/30/08    RA     Initial creation

========================================================================== */

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__
#if defined(COMDEF_H) /* guards against a known re-definer */
#define _BOOLEAN_DEFINED
#define _UINT32_DEFINED
#define _UINT16_DEFINED
#define _UINT8_DEFINED
#define _INT32_DEFINED
#define _INT16_DEFINED
#define _INT8_DEFINED
#define _UINT64_DEFINED
#define _INT64_DEFINED
#define _BYTE_DEFINED
#endif /* #if !defined(COMDEF_H) */

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef _INT8_DEFINED
typedef signed char int8;
#define _INT8_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef unsigned char uint8;
#define _UINT8_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef signed short int int16;
#define _INT16_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef unsigned short int uint16;
#define _UINT16_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef signed int long int32;
#define _INT32_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef unsigned long int uint32;
#define _UINT32_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef signed long long int int64;
#define _INT64_DEFINED
#endif

#ifndef _UINT64_DEFINED
typedef unsigned long long int uint64;
#define _UINT64_DEFINED
#endif

#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;
#define _BOOLEAN_DEFINED
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL  0
#endif

#if defined(__cplusplus)
}
#endif

#endif // __TYPEDEF_H__

