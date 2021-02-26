/***********************************************************************
 * rfs_rmts.h
 *
 * RFS wrapper for Remote Storage Server
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * APIs to hook up Remote Storage Server APIs under RFS API layer.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/rfs/src/rfs_rmts.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-12-03   dks   Move rfs init to a task
2014-01-24   dks   Create

===========================================================================*/

#ifndef __RFS_RMTS_H__
#define __RFS_RMTS_H__

#include "rfs_ops.h"

#ifdef FEATURE_RFS_USE_RMTS

int rfs_rmts_init (struct rfs_ops_type *base_ops);

#endif

#endif /* not __RFS_RMTS_H__ */
