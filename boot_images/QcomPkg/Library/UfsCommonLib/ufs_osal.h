#ifndef _UFS_OSAL_H_
#define _UFS_OSAL_H_

/**
  @file ufs_osal.h
  @brief
  This file contains implementation of interfaces that provide a thin 
  abstraction to various OS-specific APIs used by the UFS driver.

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
*/
/*
===============================================================================

                             Edit History


when        who     what, where, why
---------   ---     ------------------------------------------------------------
2014-06-19   rh     Initial creation
===============================================================================
*/

#include "stdint.h"
#include "stdio.h"
#include <string.h>


#undef  PACKED
#define PACKED                  __packed  
#define PACKED_POST             __attribute__ ((packed))  

//#define osal_memcpy(dst,src,size)   {memcpy (dst, src, size);  }
#define osal_memcpy(dst,src,size)   {int i; char *dst_l=(char *)dst; char *src_l=(char *)src; for(i=0;i<size;i++){*dst_l++=*src_l++;}}
#define osal_memset(dst,value,size) {memset (dst, value, size); }

#endif // #define _UFS_OSAL_H_
