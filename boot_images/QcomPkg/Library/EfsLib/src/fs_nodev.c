/***********************************************************************
 * fs_nodev.c
 *
 * Error-returning filesystem.
 * Copyright (C) 2006-2007,2009,2013 QUALCOMM Technologies, Inc.
 *
 * Allows a filesystem to be mounted that always returns ENODEV.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_nodev.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-26   wek   Move all buffers to DDR when compiling for boot.
2009-08-11   wek   On mount calls use void pointer instead of char.
2007-08-18   rp    struct fs_vnode cleanup.
2006-05-08   dlb   Create

===========================================================================*/

#include "fs_mount.h"
#include "fs_vnode.h"
#include "fs_nodev.h"
//#include "fs_protected_memory.h"

/* The nodev filesystem is fairly simple.  This is a bare minimum structure
 * for any filesystem, with methods that return ENODEV. */

static struct fs_mount_nodevfs nodevfs_mounts[FS_MAX_NODEV_MOUNTS];

static int nd_start (struct fs_mount *mpp, const void *args);
static int nd_stop (struct fs_mount *mpp);
static int nd_get_root (struct fs_mount *mpp, struct fs_vnode **rp);
static struct fs_vnode *nd_create_inode (struct fs_mount *mpp,
    fs_mode_t mode, uint32 gid, uint32 uid);
static int nd_write_inode (struct fs_mount *mpp, struct fs_vnode *vp);
static int nd_read_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_inode_t inum);

static const struct fs_mount_ops nodevfs_ops = {
  nd_start,
  nd_stop,
  nd_get_root,
  nd_create_inode,
  nd_write_inode,
  nd_read_inode,
  NULL, /* read_named_inode */
  NULL, /* cleanup */
};

/* The start method is called at mount time.  It needs to construct the
 * root vnode. */
static int
nd_start (struct fs_mount *mpp, const void *args)
{
  struct fs_mount_nodevfs *mp = (struct fs_mount_nodevfs *) mpp;
  struct fs_vnode *root;

  (void) args;
  root = fs_vnode_alloc ();
  if (root == NULL)
    return -ENOSPC;

  root->mp = mpp;
  root->vops = &fs_vnode_nodev_ops;
  root->mode = S_IFDIR | 0755;
  root->dev = mpp->dev;

  mp->root = root;

  return 0;
}

/* The stop method is called by umount.  The root vnode will be unrefed by
 * the umount code, so in this case, this code doesn't need to do anything.
 */
static int
nd_stop (struct fs_mount *mpp)
{
  (void) mpp;

  return 0;
}

static int
nd_get_root (struct fs_mount *mpp, struct fs_vnode **rp)
{
  struct fs_mount_nodevfs *mp = (struct fs_mount_nodevfs *) mpp;

  *rp = mp->root;

  return 0;
}

static struct fs_vnode *
nd_create_inode (struct fs_mount *mpp,
    fs_mode_t mode, uint32 gid, uint32 uid)
{
  (void) mpp;
  (void) mode;
  (void) gid;
  (void) uid;
  return NULL;
}

static int
nd_write_inode (struct fs_mount *mpp, struct fs_vnode *vp)
{
  (void) mpp;
  (void) vp;
  return -EINVAL;
}

static int
nd_read_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_inode_t inum)
{
  (void) mpp;
  (void) vp;
  (void) inum;
  return -EINVAL;
}

/* Add the nodev entries to the mountpoint table. */
void
fs_nodevfs_init (void)
{
  unsigned i;

  for (i = 0; i < FS_MAX_NODEV_MOUNTS; i++) {
    nodevfs_mounts[i].parent.ops = &nodevfs_ops;
    fs_mount_register ("nodev", &nodevfs_mounts[i].parent);
  }
}
