/***********************************************************************
 * rfs_msg.h
 *
 * Message Macros to printf RFS module messages.
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Message Macros to printf RFS module messages.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/rfs/src/rfs_msg.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-05   dks   Target compilation and audio compilation fixes.
2014-08-04   rp    Create

===========================================================================*/

#ifndef __RFS_MSG_H__
#define __RFS_MSG_H__


#include "rfs_config_i.h"
#include "msg.h"

#define RFS_MSG_STRING_0(a) \
        MSG(MSG_SSID_RFS,MSG_LEGACY_HIGH,a)

#define RFS_MSG_STRING_1(a,b) \
        MSG_SPRINTF_1(MSG_SSID_RFS,MSG_LEGACY_HIGH,a,b)

#define RFS_MSG_STRING_2(a,b,c) \
        MSG_SPRINTF_2(MSG_SSID_RFS,MSG_LEGACY_HIGH,a,b,c)

#define RFS_MSG_STRING_3(a,b,c,d) \
        MSG_SPRINTF_3(MSG_SSID_RFS,MSG_LEGACY_HIGH,a,b,c,d)

#endif /* not __RFS_MSG_H__ */
