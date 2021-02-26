/*============================================================================
  aop_ULogTargetCustom.h

  Target-specific type definitions.

  Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
  QUALCOMM Proprietary.  Export of this technology or software is regulated 
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/
/* $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/power/aop_ulogtargetcustom.h#1 $ */
#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

#if defined(__GNUC__)
#define __int64 long long
#endif

#ifndef _UINT64_DEFINED
typedef  unsigned __int64   uint64;      /* Unsigned 64 bit value */
#define _UINT64_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  __int64            int64;       /* Signed 64 bit value */
#define _INT64_DEFINED
#endif

#ifndef _BYTE_DEFINED
typedef  unsigned char      byte;        /* byte type */
#define  _BYTE_DEFINED
#endif

#if defined(__cplusplus)
#define RPM_ULOG_INLINE inline
#else
#define RPM_ULOG_INLINE __inline
#endif

/*----------------------------------------------------------------------------
  Each system may have a unique way of getting the 64 bit time value.  
  Replace (0) with an appropriate uint64 time function.
----------------------------------------------------------------------------*/
#define RPM_ULOG_GETTIME64 (0)

/* Fixme after QDSS support is brought in */
#if QDSS_SUPPORT
/*----------------------------------------------------------------------------
  Each system may have a unique QDSS send packet API.  
  ULog requires the 2 QDSS APIs to include and not include 
  a QDSS timestamp.  Lack of a QDSS timestamp is used as 
  an indicator of secondary packets for multi packet message formats.
  Customize these values and functions for your system. 
 ----------------------------------------------------------------------------*/
//defines for how the QDSS functions return success of failure
#define RPM_ULOG_QDSS_SUCCESS  0
#define RPM_ULOG_QDSS_FAIL     -1

//include the correct header file for the QDSS API
#include "qdss_lite.h"

//Allocate the QDSS port.  Return RPM_ULOG_QDSS_SUCCESS or RPM_ULOG_QDSS_FAIL
#define RPM_ULOG_QDSS_ALLOC(x)      QDSSAllocPort(x)

//free the QDSS port if necessary
#define RPM_ULOG_QDSS_FREE(x)       QDSSFreePort(*(x))

//QDSS write functions  (Using 1 for the protocol, This is differnt from normal ULog where the person setting the transport can choose the protocol.  It saves a little ram to just set it to 1. )
#define RPM_ULOG_QDSS_TS_WRITE(log, firstContent, firstCount)       QDSSWriteOSTPacket(log->stmPort, QDSSOSTHeader(QDSS_OST_ENTITY_ULOG, 1), (byte*)(firstContent), firstCount);
#define RPM_ULOG_QDSS_NO_TS_WRITE(log, secondContent, secondCount)  QDSSWriteOSTPacket(log->stmPort, QDSSOSTHeader(QDSS_OST_ENTITY_ULOG, 1), (byte*)(secondContent), secondCount);
#endif

//strlcpy
#if defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION)
#include "stdio.h"
#define RPM_ULOG_STRLCPY(a, b, c) strcpy_s(a, c, b)

#elif defined (__clang__)
#include <stdio.h>
#include <string.h>
#define RPM_ULOG_STRLCPY strlcpy 

#elif defined (__GNUC__)

#include "stdio.h"
#include "stringl.h"
#define RPM_ULOG_STRLCPY strlcpy 

#else

#include <string.h>
#define RPM_ULOG_STRLCPY strlcpy

#endif




