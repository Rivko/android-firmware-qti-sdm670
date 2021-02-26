/*==============================================================================
  @file CoreString.h

  String related Function/Structure Declarations for portability.

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/CoreString.h#1 $ 
============================================================================*/ 
#ifndef CORE_STRING_H
#define CORE_STRING_H

#if defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION)
#include "stdio.h"
#define core_snprintf sprintf_s 
#define core_strlcpy(a, b, c) strcpy_s(a, c, b)
#define core_strlcat(a, b, c) strcat_s(a, c, b)
#define core_memcpy(dest, dest_len, src, src_len) memcpy_s(dest, dest_len, src, src_len)

#elif defined (__GNUC__) || defined (FEATURE_CORE_LOW_MEM) \
      || defined( __arm__ ) || defined( __ARMCC_VERSION )
/* std_strlprintf may not be provided by the kernel so use library snprintf */
#include "stdio.h"
#include <stringl/stringl.h>
#define core_snprintf snprintf 
#define core_strlcpy strlcpy 
#define core_strlcat strlcat
#define core_memcpy memscpy

#else
/* std_strlprintf is provided by the kernel */
#include <stringl/stringl.h>
#define core_snprintf std_strlprintf 
#define core_strlcpy strlcpy
#define core_strlcat strlcat
#define core_memcpy(dest, dest_len, src, src_len) memcpy(dest, src, src_len)

#endif

#endif /* CORE_STRING_H */
