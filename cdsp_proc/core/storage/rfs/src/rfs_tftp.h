/***********************************************************************
 * rfs_tftp.h
 *
 * RFS wrapper on top of TFTP.
 * Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * APIs to hook up TFTP APIs under RFS API layer.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/rfs/src/rfs_tftp.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-12-03   dks   Move rfs init to a task
2013-11-12   dks   Create

===========================================================================*/

#ifndef __RFS_TFTP_H__
#define __RFS_TFTP_H__

#include "rfs_ops.h"

#ifdef FEATURE_RFS_USE_TFTP

int rfs_tftp_init (struct rfs_ops_type *base_ops);

#endif

#endif /* not __RFS_TFTP_H__ */
