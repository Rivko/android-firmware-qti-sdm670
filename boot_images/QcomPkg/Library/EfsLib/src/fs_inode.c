/***********************************************************************
 * fs_inode.c
 *
 * Posix interface for EFS2
 * Copyright (C) 2002-2009, 2012 QUALCOMM Technologies, Inc.
 *
 * This file includes the functions related to inode operations
 *
 ***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2009-04-03    wek     Populate atime field when constructing inode.
2008-12-19    ebb     Renamed file and featurized as FEATURE_EFS_API.
2008-11-03    rp      Introduce device abstraction.
2007-10-18    rp      Featurized QEFS2.
2007-08-18    rp      struct fs_vnode cleanup.
2006-01-11    nrs     Fixed Copyright header
2005-10-22     sh     Lint cleanup
2005-10-18    nrs     Added GID and UID to inode constructor
2005-05-15    dlb     Update generation count on inode creation.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2003-12-08     gr     Sets mtime on inode creation.
2003-10-17     gr     Now sets ctime on inode creation.
2002-08-27    drh     Changed comparisons of cluster_id types to remove
                      compiler warnings and added TO DO with them to check
                      later on.
2002-08-20    adm     Created.

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_API

#include <stdio.h>
#include <stdlib.h>

#include "fs_inode.h"
#include "fs_util.h"

/* Construct new inode. */
void
fs_inode_construct (struct fs_inode *ip, fs_mode_t mode,
      uint32 gid, uint32 uid)
{
  int i;
  uint32 curtime;

  curtime = fs_time ();

  ip->mode = mode;
  ip->size = 0;
  ip->nlink = 0;
  ip->mtime = curtime;
  ip->ctime = curtime;
  ip->atime = curtime;
  ip->blocks = 0;
  ip->gid = gid;
  ip->uid = uid;

  /* Yes, we aren't initializing this, but merely incrementing this.  The
   * inodes are allocated as all zeros, and this will increment the
   * generation count each time the inode is newly created. */
  ip->generation++;

  for (i = 0; i < FS_DIRECT_CLUSTER_COUNT + FS_DIRECTION_LEVELS - 1; i++)
    ip->data[i] = FS_INVALID_CLUSTER_ID;
}

#endif /* FEATURE_EFS_API */
