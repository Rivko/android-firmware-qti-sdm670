/*-----------------------------------------------------------------------
Copyright (c) 2008-2010, 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/
#ifndef __VOICE_RESAMP_TYPEDEF__
#define __VOICE_RESAMP_TYPEDEF__

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


#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;        /* Boolean value type.           */
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;         /* Unsigned 32 bit value         */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;         /* Unsigned 16 bit value         */
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;          /* Unsigned 8  bit value         */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;          /* Signed 32 bit value           */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;          /* Signed 16 bit value           */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;           /* Signed 8  bit value           */
#define _INT8_DEFINED
#endif

#if defined (__qdsp6__)

#ifndef _UINT64_DEFINED
typedef  unsigned long long uint64;         /* Unsigned 64 bit value         */
#define _UINT64_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  signed long long   int64;          /* Signed 64 bit value           */
#define _INT64_DEFINED
#endif

#else

#ifndef _UINT64_DEFINED
typedef  unsigned __int64 uint64;         /* Unsigned 64 bit value         */
#define _UINT64_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  signed __int64   int64;          /* Signed 64 bit value           */
#define _INT64_DEFINED
#endif

#endif

#endif//#ifndef __VOICE_RESAMP_TYPEDEF__
