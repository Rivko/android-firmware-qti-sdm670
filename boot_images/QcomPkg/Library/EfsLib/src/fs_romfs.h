/**********************************************************************
 * fs_romfs.h
 *
 * EFS2
 * Copyright (C) 2002-2005, 2007, 2009, 2012 QUALCOMM Technologies, Inc.
 *
 * ROM filesystem mountpoint handler.
 *
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2009-05-01    ebb     Moved struct efs_romfs_node to fs_sys_types.h.
2009-01-09    ebb     Removed dependency on FEATURE_EFS_QEFS2.
2007-10-18    rp      Featurized QEFS2.
2005-09-02    dlb     Remove spurious declaration.
2005-07-16    dlb     Mountpoint cleanup.
2004-10-15    dlb     Update copyright header.
2002-10-14    dlb     Created

===========================================================================*/

#ifndef __FS_ROMFS_H__
#define __FS_ROMFS_H__

/* Public definition of the struct efs_romfs_node */
#include "fs_sys_types.h"
#include "fs_config_i.h"

#include "comdef.h"
#include "fs_mount.h"
#include "fs_vnode.h"

struct fs_mount_romfs {
  /* Fields to set before starting the mountpoint. */
  struct fs_mount       parent;
  struct efs_romfs_node *nodes;
  int                   node_count;

  /* The remainder are for internal use by the romfs. */
  struct fs_vnode      *root;
};

void fs_romfs_init (void);

#endif /* not __FS_ROMFS_H__ */
