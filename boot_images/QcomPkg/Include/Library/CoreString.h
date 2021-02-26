/*==============================================================================
  @file CoreString.h

  String related Function/Structure Declarations for portability.

  Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

============================================================================*/ 
#ifndef CORE_STRING_H
#define CORE_STRING_H

#if defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION)
    #include "stdio.h"
    #define core_snprintf sprintf_s 
    #define core_strlcpy(a, b, c) strcpy_s(a, c, b)
    #define core_strlcat(a, b, c) strcat_s(a, c, b)
    #define core_memcpy(pDest, DestLength, pSrc, Count) memcpy_s(pDest, DestLength, pSrc, Count)
 
#elif defined( __ARMCC_VERSION ) || defined(TARGET_UEFI)
    #include <Library/BaseLib.h>
    #include "string.h"
	
    #define core_memcpy(pDest, DestLength, pSrc, Count) memcpy_s(pDest, DestLength, pSrc, Count)

    char *core_strlcpy(char *dst, const char *src, unsigned int size);
    int core_snprintf(char *str, unsigned int  count, const char *fmt, ...);
    char *core_strchr ( char * str, int character );
    size_t memcpy_s(void * pDest, size_t DestLength, const void * pSrc, size_t Count);

#elif defined (__GNUC__) || defined (FEATURE_CORE_LOW_MEM) || defined( __arm__ )
/* std_strlprintf may not be provided by the kernel so use library snprintf */
    #include "stdio.h"
    #include <stringl/stringl.h>
    #define core_strlcpy strlcpy 
    #define core_snprintf snprintf 
    #define core_strlcat strlcat
    #define core_memcpy memcpy_s 
#else
    /* std_strlprintf is provided by the kernel */
    #include <stringl/stringl.h>
    #include "string.h"
    #define core_snprintf std_strlprintf 
    #define core_strlcpy strlcpy
    #define core_strlcat strlcat
    #define core_memcpy memcpy_s 


#endif

#endif /* CORE_STRING_H */
