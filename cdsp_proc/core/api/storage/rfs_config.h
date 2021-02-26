/***********************************************************************
 * rfs_config.h
 *
 * Remote File System Public Configuration file.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Public configuration file for the Remote File System module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/api/storage/rfs_config.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-11-14   rp    Create

===========================================================================*/

#ifndef __RFS_CONFIG_H__
#define __RFS_CONFIG_H__

#include "customer.h"

/**
 Maximum length of a full pathname, not including a trailing '\\0' character.
*/
#ifndef RFS_PATH_MAX
  #define RFS_PATH_MAX 256
#endif


#endif /* not __RFS_CONFIG_H__ */
