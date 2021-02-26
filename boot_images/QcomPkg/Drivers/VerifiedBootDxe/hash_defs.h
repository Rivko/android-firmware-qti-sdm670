/** @file  
                        hash_defs.h

  Header file containing the hash data types

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 07/28/16   SA     Initial version

=============================================================================*/

#ifndef HASH_DEFS_H
#define HASH_DEFS_H

typedef enum {
  VB_UNDEFINED_HASH = 0,
  VB_SHA1,
  VB_SHA256,
  VB_UNSUPPORTED_HASH,
  VB_RESERVED_HASH = 0x7fffffff /* force to 32 bits */
} VB_HASH;

#define VB_SHA256_SIZE  32

#endif
