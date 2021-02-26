/***********************************************************************
 * rfs_assert.h
 *
 * RFS Asserts module.
 *
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.

 * Provide Asserts support to the RFS module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/inc/rfs_assert.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-20   rp    Create

===========================================================================*/

#ifndef __RFS_ASSERT_H__
#define __RFS_ASSERT_H__

#include "assert.h"
#include "msg.h"

/*---------------------------------------------------------------------------
 * RFS_ASSERT :
 *    Enabled on CRM-builds and can be disabled on commercial.

 * RFS_DEBUG_ASSERT :
 *    Disabled on both CRM and commercial builds. Enabled only on demand
 *    when we debug RFS
---------------------------------------------------------------------------*/

#define RFS_ASSERT(cond)                                        \
  do {                                                          \
    if (!(cond)) {                                              \
      do {                                                      \
        MSG_SPRINTF_1(MSG_SSID_RFS, MSG_LEGACY_ERROR,           \
                      "RFS_ASSERT : %s", #cond);                \
      } while (0);                                              \
      ASSERT (cond);                                            \
    }                                                           \
  } while (0)


#if defined (RFS_ENABLE_DEBUG_MODE)
  #define RFS_DEBUG_ASSERT(cond) RFS_ASSERT(cond)
#else
  #define RFS_DEBUG_ASSERT(cond) ((void)0)
#endif

#endif /* not __RFS_ASSERT_H__ */
