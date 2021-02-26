/*===========================================================================

  fs_mount.h - Mountpoint generic code.

  Copyright (C) 2003, 2004, 2005, 2006, 2009 by Qualcomm Technologies Incorporated.

  All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.




when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-08-11    wek     On mount calls use void pointer instead of char.
2006-05-10    dlb     Track mountpoint generations.
2006-01-11    nrs     Fixed Copyright header
2005-10-18    nrs     Added UID to create inode
2005-08-25    nrs     Implent buffer accounting for quotas and reservations
2005-08-03    dlb     Add umount support.
2005-07-13    dlb     Mountpoint cleanup.
2005-07-12    dlb     Move transactions out of mount.
2005-03-23    dlb     Extern fs_root_dev.
2004-10-15    dlb     Update copyright line.
2003-02-28    dlb     Created.
===========================================================================*/

#ifndef __FS_MOUNT_H__
#define __FS_MOUNT_H__

#include "comdef.h"
#include "fs_vnode.h"

struct fs_mount;

struct fs_mount_ops {
  /* Start the mountpoint.  The parameter parsing depends on the
   * filesystem.  Returns 0 if the FS count be started. */
  int (*start) (struct fs_mount *mp, const void *dev);

  /* Stop the mountpoint. */
  int (*stop) (struct fs_mount *mp);

  /* Get the root of the filesystem. */
  int (*get_root) (struct fs_mount *mp, struct fs_vnode **rp);

  /* Create a new inode on this mountpoint. */
  struct fs_vnode *(*create_inode) (struct fs_mount *mp,
      fs_mode_t mode, uint32 gid, uint32 uid);

  /* Write an inode out.  Used by vnode code for flushing. */
  int (*write_inode) (struct fs_mount *mp, struct fs_vnode *vp);

  /* Read an inode in.  Used by the vnode code to cache entries. */
  int (*read_inode) (struct fs_mount *mp, struct fs_vnode *vp,
      fs_inode_t inum);

  /* "Read" in a named inode.  Used for symlink on some types, can be null. */
  int (*read_named_inode) (struct fs_mount *mp, struct fs_vnode *vp,
      fs_mode_t mode, const char *key, int key_len);

  /* Perform any necessary cleanup.  Called just before freeing a vnode.
   * Only called if not NULL. */
  void (*cleanup) (struct fs_mount *mp, struct fs_vnode *vp);
};

/* Generic mountpoints just have the ops table at the beginning.  Other
 * mountpoint types will have specific data for the specified type. */
struct fs_mount {
  const struct fs_mount_ops *ops;
  fs_dev_t                   dev;
  uint32                     mount_generation;
};
typedef struct fs_mount *fs_mount_t;

/* Entries in the mountpoint table.  Each mounted filesstem gets an entry
 * in this table. */
struct fs_mpoint {
  /* Device number in the underlying filesystem where this mountpoint is
   * mounted. */
  fs_dev_t              mount_at_dev;

  /* Inode of the directory for the mount. */
  fs_inode_t            mount_at_inode;

  /* The vnode for the root of this mount. */
  struct fs_vnode      *root_node;

  /* The mountpoint structure this refers to. */
  fs_mount_t            mp;

  /* The name of this mountpoint type. */
  const char           *fstype;
};

/* The global mountpoint table.  This table does not have an entry for the
 * root filesystem.  The root cannot be unmounted (except at shutdown), and
 * is accessed differently.  Points that are not used are indicated by a
 * 'dev' field of FS_INVALID_DEV. */
extern struct fs_mpoint fs_mpoints[FS_MAX_MOUNTS];

/* The root filesystem is available through this value.  This should only
 * be written upon startup, but can be read as needed. */
extern struct fs_mount *fs_root_mpoint;
extern fs_dev_t         fs_root_dev;
extern struct fs_vnode *fs_root_vnode;

/* Initialize the mountpoint mechanism. */
void fs_mount_init (void);

/* Register a mountpoint handler.  The name should be a static string,
 * since it won't be copied. */
void fs_mount_register (const char *fstype, fs_mount_t mp);

/* Mount the root filesystem. */
int fs_mount_root (const char *fstype, const void *arg);

/* Perform a submount. */
int fs_mount_mount (struct fs_vnode *mpoint,
    const char *fstype, const void *arg);

/* Perform a sub-unmount. */
int fs_mount_umount (struct fs_vnode *mpoint);

/* Perform a sub remount. */
int fs_mount_remount (struct fs_vnode *oldvp, struct fs_vnode *newvp);

#endif /* not __FS_MOUNT_H__ */
