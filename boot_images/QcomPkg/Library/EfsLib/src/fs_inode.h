
/***********************************************************************
 * fs_inode.h
 *
 * Inode structure declaration and operations.
 * Copyright (C) 2002-2009, 2012 QUALCOMM Technologies, Inc.
 *
 * This file includes funcitons declarations for inode operations
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2009-04-03    wek     Added access time (atime) to fs_inode.
2009-03-27    ebb     Changed #include fs_device.h => fs_device_types.h
2008-12-19    ebb     Renamed file and featurized as FEATURE_EFS_API.
2008-11-03    rp      Introduce device abstraction.
2007-10-18    rp      Featurized QEFS2.
2007-08-18    rp      struct fs_vnode cleanup.
2006-01-11    nrs     Fixed Copyright header
2005-10-18    nrs     Added GID and UID to inode constructor
2005-06-09    sh      Merged nlink and pad1 for 16-bit nlink
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2003-12-04    dlb     Add attributes.
2002-08-20    adm     Created.

===========================================================================*/


#ifndef _FS_INODE_H
#define _FS_INODE_H

#include "fs_config_i.h"

#ifdef FEATURE_EFS_API

#include "fs_sys_types.h"

/* Needed for fs_cluster_id. */
#include "fs_device_types.h"

#define FS_DIRECT_CLUSTER_COUNT         13
#define FS_DIRECTION_LEVELS             4

/* This is the inode as written to flash.  There shouldn't be any fields in
 * here that are volatile.  In fact, the inode_num shouldn't be here, since
 * it is obvious from where the inode was read or written to or from. */
typedef struct fs_inode {
  fs_mode_t     mode;
  uint16        nlink;          /* Number of hard links. */
  uint32        attr;           /* Extended attributes. */
  uint32        size;           /* Size of file, in bytes. */
  uint16        uid;
  uint16        gid;
  uint32        generation;     /* Inode generation.  Incremented on file
                                   creation. */
  uint32        blocks;         /* Blocks (clusters) used by file. */
  uint32        mtime;          /* Last mod time of file. */
  uint32        ctime;          /* Posix ctime. */
  uint32        atime;          /* Last access time of the file */
  uint32        reserved[7];    /* Future use. */

  /* Reference to the data for this file.  The first
   * FS_DIRECT_CLUSTER_COUNT entries are direct references to clusters.
   * Each value beyond that is a deeper level of data. */
  fs_cluster_id data[FS_DIRECT_CLUSTER_COUNT + FS_DIRECTION_LEVELS - 1];
} fs_inode_type;

/* Initialize an empty inode, appropriately. */
void fs_inode_construct (struct fs_inode *ip, fs_mode_t mode,
      uint32 gid, uint32 uid);

#endif /* FEATURE_EFS_API */

#endif /* End of _FS_INODE_H */
