/*==============================================================================
  @file LoaderUtils.h

  String related Function/Structure Declarations for portability.

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary


when       who          what, where, why
--------   ---          -------------------------------------------------- 
05/12/15   bhsriniv     Remove USE_LOADER_UTILS macro
01/01/15   bhsriniv     Removed duplicate stdlib protos and included stdlib headers
10/02/14   ck           Added setjump functionality
08/25/14   kedara       Added strncmp declaration
08/08/14   ck           Added snprintf declaration
07/20/14   kedara       Map qsnprintf to snprintf
04/29/14   kedara       Initial version  

============================================================================*/ 


#ifndef LOADER_STRING_H
#define LOADER_STRING_H

#include <Base.h>
#include <Library/BaseLib.h>
#include "com_dtypes.h"
#include <string.h>
#include <stdio.h>

size_t memscpy( void *dst, size_t dst_size, const void *src, size_t src_size);

size_t memsmove( void *dst, size_t dst_size, const void *src, size_t src_size);

#define qsnprintf  snprintf
#endif /* LOADER_STRING_H */
