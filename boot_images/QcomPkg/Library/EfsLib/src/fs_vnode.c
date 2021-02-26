/***********************************************************************
 * fs_vnode.c
 *
 * Vnode operations for EFS2
 * Copyright (C) 2002-2013,2015-2016 QUALCOMM Technologies, Inc.
 *
 * This file defines the functions related to vnode operations
 ***********************************************************************/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_vnode.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2016-05-23    rp      Support OEMs to create their own EFS-golden-copy.
2016-04-14    rp      Add EFS golden copy creation from files list.
2015-04-23    vm      Force the EFS Sync during shutdown.
2013-10-09    nr      Replace banned memory functions with safer versions.
2013-07-22    rp      Add new EFS API to store data in SuperBlock.
2013-06-19    dks     Added NAND Flash scrub support.
2013-05-10    wek     Update the error code returned by link/unlink entries.
2013-05-07    wek     Add EFS2 Read Only support.
2013-01-11    dks     Move writing to Golden partition outside of efs_sync.
2012-12-14    wek     Implement modified efs_utime for item files.
2012-12-18    dks     Update golden copy strategy for new rmts layout.
2012-10-22    wek     Convert item files to regular files.
2012-08-14    wek     Implement chmod for item files.
2011-10-11    nr      Added borrow_ram support to borrow ram from lower layers.
2011-03-08    rp      Add support to lock EFS.
2010-11-18    rp      Support TAR image Golden-Copy for RMTS.
2010-05-13    wek     Make sync_no_wait non blocking.
2010-05-06    wek     Implement golden copy for remote storage page manager.
2010-03-19    yog     Added support for files having max filesize upto 4GB.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-12-18    vr      Added set_volume_name and get_volume_name APIs.
2009-10-14    dks     Added vops for get_fs_data,get_device_attr,image_prepare
2009-05-18    rp      Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
2009-04-03    wek     Added utime ops to nodev.
2008-12-17    ebb     Updated fs_vnode_init to zero mp and ref fields.
2008-10-06    np      Added shred functionality to delete obsolete user data.
2008-04-28    np      Updated vnode pool size definition.
2008-01-24    sch     Added invalid stub for getsize vnode op
2007-08-19    rp      Use FS_MAX_DESCRIPTORS from fs_config.h
2007-08-18    rp      struct fs_vnode cleanup.
2007-05-25    umr     Added support for EFS memory protection feature.
2006-11-10    sh      Add chmod support.
2006-09-20    dlb     Lint cleanup.
2006-07-12    dlb     Fix bug in named vnode lookup.
2006-06-27    dlb     Pass stat through readdir.
2006-06-26     yg     Memory reduction effort
2006-05-10    dlb     Add generation count to nodev mpoint.
2006-05-09    dlb     Export nodev.
2006-01-11    nrs     Fixed Copyright header
2005-11-12     sh     Use rex tcb err_num instead of global efs_errno
2005-10-18    nrs     Fixed prototypes for quotas & reservations
2005-08-30    dlb     Fix mode type on mknod.
2005-08-23    sh      Added mknod
2005-08-10    dlb     Count vnodes correctly in utest.
2005-08-10    dlb     Closedir must work on ENODEV vnodes.
2005-08-04    dlb     Support NODEV vnode.
2005-08-01    nrs     Added calls to set and get for quotas and reservations
2005-07-15    nrs     Added implementation for efs_chown
2005-05-26    sd      Compilation fixes for L4.
2005-03-25    dlb     Allow invalidate of multiple ref'd vnodes.
2005-02-07    dlb     Increase size of vnode pool.
2004-10-17    dlb     Add vnode obliteration.
2004-10-15    dlb     Update copyright header.
2004-07-19    dlb     Restructure vnode interface.
2003-10-17     gr     Moved fs_time to fs_util.c. Out of place in this file.
2003-06-17    jkl     Clean up code.
2003-06-13     gr     Added a function to get the ref count of a vnode.
2002-08-27    drh     Changed comparisons of cluster_id types to remove
                      compiler warnings and added TO DO with them to check
                      later on.
2002-08-08    drh     Created by adm.  Added history header.

===========================================================================*/

#include <string.h>

#include "assert.h"
#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_vnode.h"
#include "fs_mount.h"
//#include "fs_protected_memory.h"

/**********************************************************************
 * Vnode pool management.
 *
 * The vnodes can be stored one of two ways.  Most entities will be looked
 * up through the dev and inum fields of the inode.  However, if inum is
 * set to FS_INVALID_INODE_NUMBER, then the vnode is name indexed rather
 * than numerically indexed.  The key and key_length fields of the "named"
 * field of the 'p' structure will be used for this key.
 *
 * Notes for implementing filesystems.  If some kind of unique number can
 * be determined for a particular vnode, it should be used.  Possibilites
 * include offsets into index blocks and other such things.  These unique
 * numbers are used to keep track of the vnode pool, and don't need to have
 * any kind of meaning outside of EFS.  The named index should only be used
 * for entities that are truly stored by name, such as entries in a
 * database.
 */

/* Vnode pool.  Size is statically defined for now.  This should be larger
 * than the maximum number of open file descriptors by the following:
 * - 1 for each mountpoint.
 * - 1 for each open iterator.
 * - 3 needed for temporary use to create directories.
 * - 1 needed for mounting the root.
 */
#ifndef FS_VNODE_POOL_SIZE
  #define FS_VNODE_POOL_SIZE      (FS_MAX_DESCRIPTORS+3+1\
                                  +FS_MAX_ITERATORS+FS_MAX_EXTFS_MOUNTS)
#endif

static struct fs_vnode vnode_pool[FS_VNODE_POOL_SIZE];
static struct fs_vnode *vnode_free = NULL;

struct fs_vnode *
fs_vnode_alloc (void)
{
  struct fs_vnode *here = vnode_free;

  if (here == 0)
    return here;

  vnode_free = here->next;

  memset (here, 0, sizeof(*here));
  here->ref = 1;

  return here;
}

void
fs_vnode_ref (struct fs_vnode *vn)
{
  vn->ref++;
}

void
fs_vnode_unref (struct fs_vnode *vn)
{
  vn->ref--;

  if (vn->ref < 0)
    FS_ERR_FATAL ("Vnode reference dropped below zero", 0, 0, 0);

  if (vn->ref == 0)
  {
    if (vn->dirty)
      FS_ERR_FATAL ("Unreferenced dirty vnode", 0, 0, 0);

    if (vn->mp->ops->cleanup != NULL)
      vn->mp->ops->cleanup (vn->mp, vn);

    vn->mode = 0;
    vn->next = vnode_free;
    vnode_free = vn;
  }
}

/* Upon transaction rollback, cause the cached vnode information to be
 * invalidated.  This also does an implicit unref (which, in the common
 * case allows the vnode to just be discarded). */
void
fs_vnode_invalidate (struct fs_vnode *vn)
{
  int result;

  if (vn->ref == 1) {
    /* If this is the only reference to the vnode, just discard it. */
    vn->dirty = 0;

    if (vn->mp->ops->cleanup != NULL)
      FS_ERR_FATAL ("Cannot have cleanup operation and invalidate", 0, 0, 0);

    vn->dev = FS_INVALID_DEV;
    vn->inum = FS_INVALID_INODE;

    fs_vnode_unref (vn);
  } else {
    /* Otherwise, since something else still references this vnode, re-read
     * the vnode information from the device. */

    if (vn->inum == FS_INVALID_INODE) {
      /* Named lookup. */
      ASSERT (vn->mp->ops->read_named_inode != NULL);

      result = vn->mp->ops->read_named_inode (
          vn->mp, vn, vn->mode, vn->p.named.key, vn->p.named.key_length);
      if (result != 0)
        FS_ERR_FATAL ("Unable to re-read named inode information", 0, 0, 0);
      /* Everything else should still be set correctly. */
    } else {
      /* Numbered lookup. */
      result = vn->mp->ops->read_inode (vn->mp, vn, vn->inum);
      if (result != 0)
        FS_ERR_FATAL ("Unable to re-read inode information", 0, 0, 0);
    }

    /* Now unreference our ref. */
    vn->dirty = 0;
    fs_vnode_unref (vn);
  }
}

/* Replace the destination vnode with source and invalidate source vnode.
 * This function should be called only when the source vnode was recently
 * created (inside an EFS transaction). There should be only one reference
 * to the source vnode. */
void
fs_vnode_supplant (struct fs_vnode *dest, struct fs_vnode *src)
{
  int ref;

  if (src->ref != 1)
  {
    /* If the reference count of source is not 1, then there is a bug with
     * the code calling this function. The source vnode should be referenced
     * once. If the reference count is greater than 1 then this vnode can't
     * be invalidated because there is somebody else using it. if the
     * reference count is zero, then it should not supplant the destination
     * vnode. */
    FS_ERR_FATAL ("[%d] Invalid source vnode on supplant", src->ref, 0, 0);
  }

  /* Keep the reference count of destination, update the rest of the fields. */
  ref = dest->ref;
  fs_memscpy (dest, sizeof (struct fs_vnode), src, sizeof (struct fs_vnode));
  dest->ref = ref;
  fs_vnode_invalidate (src);
}

int
fs_vnode_getref (struct fs_vnode *vn)
{
  return vn->ref;
}

/* XXX: If the vnode pool gets too large, it might help to hash this
 * information. */
struct fs_vnode *
fs_vnode_lookup (struct fs_mount *mp, fs_dev_t dev, fs_inode_t inode)
{
  int i, result;
  struct fs_vnode *vp;

  /* Not a terribly efficient way to do this. */
  for (i = 0; i < FS_VNODE_POOL_SIZE; i++)
  {
    if (vnode_pool[i].mode != 0 &&
        vnode_pool[i].dev == dev &&
        vnode_pool[i].inum == inode)
    {
      fs_vnode_ref (&vnode_pool[i]);
      return &vnode_pool[i];
    }
  }

  /* Allocate the vnode, and attempt to read in its contents. */
  if (mp->ops->read_inode == NULL)
    return NULL;

  vp = fs_vnode_alloc ();
  if (vp == NULL)
    return NULL;

  vp->dev = dev;
  vp->inum = inode;
  vp->mp = mp;
  vp->dirty = 0;
  result = mp->ops->read_inode (mp, vp, inode);

  if (result != 0) {
    fs_vnode_unref (vp);
    return NULL;
  }

  return vp;
}

/* Note that passing in a mode of 0 indicates that any mode will match. */
struct fs_vnode *
fs_vnode_named_lookup (struct fs_mount *mp, fs_dev_t dev,
    fs_mode_t mode,
    const char *key, int key_len)
{
  int i, result;
  struct fs_vnode *vp;

  for (i = 0; i < FS_VNODE_POOL_SIZE; i++) {
    if (vnode_pool[i].mode != 0 &&
        (mode == 0 ||
          (vnode_pool[i].mode & S_IFMT) == (mode & S_IFMT)) &&
        vnode_pool[i].dev == dev &&
        vnode_pool[i].inum == FS_INVALID_INODE &&
        vnode_pool[i].p.named.key_length == (unsigned) key_len &&
        memcmp (key, vnode_pool[i].p.named.key, key_len) == 0)
    {
      fs_vnode_ref (&vnode_pool[i]);
      return &vnode_pool[i];
    }
  }

  /* Allocate the vnode, and attempt to read in its contents. */
  if (mp->ops->read_named_inode == NULL)
    return NULL;

  vp = fs_vnode_alloc ();
  if (vp == NULL)
    return NULL;

  vp->dev = dev;
  vp->inum = FS_INVALID_INODE;
  vp->mp = mp;
  vp->dirty = 0;
  result = mp->ops->read_named_inode (mp, vp, mode, key, key_len);

  if (result != 0) {
    fs_vnode_unref (vp);
    return NULL;
  }

  return vp;
}

void
fs_vnode_init (void)
{
  int i;

  vnode_free = 0;

  for (i = 0; i < FS_VNODE_POOL_SIZE; i++)
  {
    vnode_pool[i].ref = 0;
    vnode_pool[i].mp = 0;
    vnode_pool[i].mode = 0;
    vnode_pool[i].dirty = 0;
    vnode_pool[i].next = vnode_free;
    vnode_free = &vnode_pool[i];
  }
}

void
fs_vnode_flush (struct fs_vnode *vn)
{
  if (vn->dirty) {
    (void) vn->mp->ops->write_inode (vn->mp, vn);

    vn->dirty = 0;
  }
}

/* These are all of the invalid operations, so that various mountpoints
 * don't have to provide private error returning implementations. */
int
fs_vnode_invalid_lookup (struct nameidata *ndp)
{
  (void) ndp;

  return -EPERM;
}

int
fs_vnode_invalid_create (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;

  return -EPERM;
}

int
fs_vnode_invalid_link (struct nameidata *ndp)
{
  (void) ndp;

  return -EPERM;
}

int
fs_vnode_invalid_unlink (struct nameidata *ndp)
{
  (void) ndp;

  return -EPERM;
}

fs_ssize_t
fs_vnode_invalid_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;
  (void) mode;

  return -EINVAL;
}

fs_ssize_t
fs_vnode_invalid_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;

  return -EINVAL;
}

int
fs_vnode_invalid_truncate (struct fs_vnode *vp, fs_size_t pos)
{
  (void) vp;
  (void) pos;

  return -EINVAL;
}

int
fs_vnode_invalid_mkdir (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;

  return -EINVAL;
}

int
fs_vnode_invalid_rmdir (struct nameidata *ndp)
{
  (void) ndp;

  return -EINVAL;
}

void *
fs_vnode_invalid_opendir (struct fs_vnode *vp)
{
  (void) vp;

  return NULL;
}

int
fs_vnode_invalid_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent)
{
  (void) vp;
  (void) dir;
  (void) dirent;

  return -EINVAL;
}

int
fs_vnode_invalid_closedir (struct fs_vnode *vp, void *dir)
{
  (void) vp;
  (void) dir;

  return -EINVAL;
}

int
fs_vnode_invalid_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  (void) vp;
  (void) buf;

  return -EINVAL;
}

int
fs_vnode_invalid_getsize (struct fs_vnode *vp, uint32 *numbytes)
{
  (void) vp;
  (void) numbytes;

  return -EINVAL;
}

int
fs_vnode_invalid_rename (struct nameidata *ndp, struct nameidata *ndp2)
{
  (void) ndp;
  (void) ndp2;

  return -EINVAL;
}

int
fs_vnode_invalid_symlink (struct nameidata *ndp, const char *dest)
{
  (void) ndp;
  (void) dest;

  return -EINVAL;
}

int
fs_vnode_invalid_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz)
{
  (void) vp;
  (void) buf;
  (void) bufsiz;

  return -EINVAL;
}

int
fs_vnode_invalid_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  (void) vp;
  (void) buf;

  return -EINVAL;
}

int
fs_vnode_invalid_put_item (struct nameidata *ndp,
    void *data, int length, int mode)
{
  (void) ndp;
  (void) data;
  (void) length;
  (void) mode;

  return -EINVAL;
}

int
fs_vnode_invalid_mknod (struct nameidata *ndp, fs_mode_t mode,
                        fs_devspecial_t dev)
{
  (void) ndp;
  (void) mode;
  (void) dev;

  return -EINVAL;
}

int
fs_vnode_invalid_get_item (struct fs_vnode *dvp, void *data, int length)
{
  (void) dvp;
  (void) data;
  (void) length;

  return -EINVAL;
}

/* The nodev vops. */
static int vnodev_lookup (struct nameidata *ndp);
static int vnodev_create (struct nameidata *ndp, fs_mode_t mode);
static int vnodev_link (struct nameidata *ndp);
static int vnodev_unlink (struct nameidata *ndp);
static fs_ssize_t vnodev_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode);
static fs_ssize_t vnodev_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count);
static int vnodev_truncate (struct fs_vnode *vp, fs_size_t pos);
static int vnodev_mkdir (struct nameidata *ndp, fs_mode_t mode);
static int vnodev_rmdir (struct nameidata *ndp);
static void *vnodev_opendir (struct fs_vnode *vp);
static int vnodev_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent);
static int vnodev_closedir (struct fs_vnode *vp, void *dir);
static int vnodev_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int vnodev_getsize (struct fs_vnode *vp, uint32 *numbytes);
static int vnodev_rename (struct nameidata *ndp, struct nameidata *ndp2);
static int vnodev_symlink (struct nameidata *ndp, const char *dest);
static int vnodev_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz);
static int vnodev_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf);
static int vnodev_put_item (struct nameidata *ndp, void *data,
    int length, int mode);
static int vnodev_get_item (struct fs_vnode *vp, void *data, int length);
static int vnodev_mknod (struct nameidata *ndp, fs_mode_t mode,
    fs_devspecial_t dev);
static int vnodev_chmod (struct nameidata *ndp, fs_mode_t mode);
static int vnodev_chown (struct fs_vnode *vp, int uid_val,
    int gid_val);
static int vnodev_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size);
static int vnodev_set_quota (struct fs_vnode *vp, uint32 gid,
    uint32 size);
static int vnodev_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo);
static int vnodev_scrub (struct fs_vnode *vp);
static int vnodev_utime (struct fs_vnode *vp, const struct fs_utimbuf *times);
static int vnodev_sync_start (struct fs_vnode *vp, int do_force_sync,
                              int is_shutdown_sync);
static int vnodev_sync_wait (struct fs_vnode *vp);
static int vnodev_sync_end (struct fs_vnode *vp);
static int vnodev_sync_no_wait (struct fs_vnode *vp,
                                fs_sync_token_t *sync_token);
static int vnodev_sync_get_status (struct fs_vnode *vp,
                 fs_sync_token_t sync_token, enum fs_sync_result *sync_result);
static int vnodev_get_device_attr (struct fs_vnode *vp,
    struct fs_device_attr * dev_attr);
static int vnodev_get_fs_data (struct fs_vnode *vp,
    struct fs_factimage_read_info * image_info_ptr,
    byte* page_buffer);
static int vnodev_image_prepare (struct fs_vnode *vp);
static int vnodev_get_volume_name (struct fs_vnode *vp, char *label, int len);
static int vnodev_set_volume_name (struct fs_vnode *vp, char *label);
static int vnodev_make_golden_copy (struct fs_vnode *vp);
static int vnodev_fs_init_complete (struct fs_vnode *vp);
static int vnodev_lock_efs (struct fs_vnode *vp);
static int vnodev_borrow_ram (struct fs_vnode *vp, void **ram_addr,
                               uint32 *ram_size);
static int vnodev_store_data_in_sb (struct fs_vnode *vp, const void *data,
                               uint32 data_size);
static int vnodev_get_buf_for_fsg (struct fs_vnode *vp, uint8 **buf,
                                   uint32 *buf_size, uint32 *hdr_offset);
static int vnodev_write_to_fsg (struct fs_vnode *vp, uint8 *buf,
                                uint32 buf_size, uint32 partition);

const struct vfs_ops fs_vnode_nodev_ops = {
  vnodev_lookup,
  vnodev_create,
  vnodev_link,
  vnodev_unlink,
  vnodev_write,
  vnodev_read,
  vnodev_truncate,
  vnodev_mkdir,
  vnodev_rmdir,
  vnodev_opendir,
  vnodev_readdir,
  vnodev_closedir,
  vnodev_getstat,
  vnodev_getsize,
  vnodev_rename,
  vnodev_symlink,
  vnodev_readlink,
  vnodev_getstatvfs,
  vnodev_put_item,
  vnodev_get_item,
  vnodev_mknod,
  vnodev_chmod,
  vnodev_chown,
  vnodev_set_reservation,
  vnodev_set_quota,
  vnodev_get_group_info,
  fs_vnode_invalid_shred,
  vnodev_scrub,
  vnodev_utime,
  vnodev_utime, /* For NODEV utime_item is the same as utime. */
  vnodev_sync_start,
  vnodev_sync_wait,
  vnodev_sync_end,
  vnodev_sync_no_wait,
  vnodev_sync_get_status,
  vnodev_get_device_attr,
  vnodev_get_fs_data,
  vnodev_image_prepare,
  vnodev_get_volume_name,
  vnodev_set_volume_name,
  vnodev_make_golden_copy,
  vnodev_fs_init_complete,
  vnodev_lock_efs,
  vnodev_borrow_ram,
  vnodev_store_data_in_sb,
  vnodev_get_buf_for_fsg,
  vnodev_write_to_fsg
};

static int
vnodev_lookup (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static int
vnodev_create (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;
  return -ENODEV;
}

static int
vnodev_link (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static int
vnodev_unlink (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static fs_ssize_t
vnodev_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;
  (void) mode;

  return -ENODEV;
}

static fs_ssize_t
vnodev_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;
  return -ENODEV;
}

static int
vnodev_truncate (struct fs_vnode *vp, fs_size_t pos)
{
  (void) vp;
  (void) pos;
  return -ENODEV;
}

static int
vnodev_mkdir (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;
  return -ENODEV;
}

static int
vnodev_rmdir (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static void *
vnodev_opendir (struct fs_vnode *vp)
{
  (void) vp;
  return NULL;
  /* return -ENODEV; */
}

static int
vnodev_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent)
{
  (void) vp;
  (void) dir;
  (void) dirent;
  return -ENODEV;
}

/* Closedir is a special case, since we allow it to pass.  This method is
 * only invoked through the efs_closedir call, which should have already
 * checked for its existence. */
static int
vnodev_closedir (struct fs_vnode *vp, void *dir)
{
  (void) vp;
  (void) dir;
  return 0;
}

static int
vnodev_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  (void) vp;
  (void) buf;
  return -ENODEV;
}

static int
vnodev_getsize (struct fs_vnode *vp, uint32 *size)
{
  (void) vp;
  (void) size;
  return -ENODEV;
}

static int
vnodev_rename (struct nameidata *ndp, struct nameidata *ndp2)
{
  (void) ndp;
  (void) ndp2;
  return -ENODEV;
}

static int
vnodev_symlink (struct nameidata *ndp, const char *dest)
{
  (void) ndp;
  (void) dest;
  return -ENODEV;
}

static int
vnodev_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz)
{
  (void) vp;
  (void) buf;
  (void) bufsiz;
  return -ENODEV;
}

static int
vnodev_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  (void) vp;
  (void) buf;
  return -ENODEV;
}

static int
vnodev_put_item (struct nameidata *ndp, void *data,
    int length, int mode)
{
  (void) ndp;
  (void) data;
  (void) length;
  (void) mode;
  return -ENODEV;
}

static int
vnodev_get_item (struct fs_vnode *vp, void *data, int length)
{
  (void) vp;
  (void) data;
  (void) length;
  return -ENODEV;
}

static int
vnodev_mknod (struct nameidata *ndp, fs_mode_t mode,
    fs_devspecial_t dev)
{
  (void) ndp;
  (void) mode;
  (void) dev;
  return -ENODEV;
}

static int
vnodev_chmod (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;

  return -ENODEV;
}

static int
vnodev_chown (struct fs_vnode *vp, int uid_val, int gid_val)
{
  (void) vp;
  (void) uid_val;
  (void) gid_val;

  return -ENODEV;
}

static int
vnodev_scrub (struct fs_vnode *vp)
{
  (void) vp;
  return -ENODEV;
}


static int
vnodev_utime (struct fs_vnode *vp, const struct fs_utimbuf *times)
{
  (void) vp;
  (void) times;

  return -ENODEV;
}

static int
vnodev_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -ENODEV;
}

static int
vnodev_set_quota (struct fs_vnode *vp, uint32 gid, uint32 size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -ENODEV;
}

static int
vnodev_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo)
{
  (void) vp;
  (void) gid;
  (void) ginfo;

  return -ENODEV;
}

static int
vnodev_sync_start (struct fs_vnode *vp, int do_force_sync,
                   int is_shutdown_sync)
{
  (void) vp;
  (void) do_force_sync;
  (void) is_shutdown_sync;

  return -ENODEV;
}

static int
vnodev_sync_wait (struct fs_vnode *vp)
{
  (void) vp;

  return -ENODEV;
}

static int
vnodev_sync_end (struct fs_vnode *vp)
{
  (void) vp;

  return -ENODEV;
}

static int
vnodev_sync_no_wait (struct fs_vnode *vp, fs_sync_token_t *sync_token)
{
  (void) vp;
  (void) sync_token;

  return -ENODEV;
}

static int
vnodev_sync_get_status (struct fs_vnode *vp, fs_sync_token_t sync_token,
                        enum fs_sync_result *sync_result)
{
  (void) vp;
  (void) sync_token;
  (void) sync_result;

  return -ENODEV;
}

static int
vnodev_get_device_attr (struct fs_vnode *vp, struct fs_device_attr * dev_attr)
{
  (void) vp;
  (void) dev_attr;

  return -ENODEV;
}

static int
vnodev_get_fs_data (struct fs_vnode *vp,
    struct fs_factimage_read_info * image_info_ptr, byte* page_buffer)
{
  (void) vp;
  (void) image_info_ptr;
  (void) page_buffer;

  return -ENODEV;
}

static int
vnodev_image_prepare (struct fs_vnode *vp)
{
  (void) vp;

  return -ENODEV;
}

static int
vnodev_get_volume_name (struct fs_vnode *vp, char *label, int len)
{
  (void) vp;
  (void) label;
  (void) len;

  return -ENODEV;
}

static int
vnodev_set_volume_name (struct fs_vnode *vp, char *label)
{
  (void) vp;
  (void) label;

  return -ENODEV;
}

static int
vnodev_make_golden_copy (struct fs_vnode *vp)
{
  (void) vp;

  return -ENODEV;
}

static int
vnodev_fs_init_complete (struct fs_vnode *vp)
{
  (void) vp;

  return -ENODEV;
}

static int
vnodev_lock_efs (struct fs_vnode *vp)
{
  (void) vp;

  return -ENODEV;
}

static int
vnodev_borrow_ram (struct fs_vnode *vp, void **ram_addr, uint32 *ram_size)
{
  (void) vp;
  (void) ram_addr;
  (void) ram_size;

  return -ENODEV;
}

static int
vnodev_store_data_in_sb (struct fs_vnode *vp, const void *data,
                         uint32 data_size)
{
  (void) vp;
  (void) data;
  (void) data_size;

  return -ENODEV;
}

static int
vnodev_get_buf_for_fsg (struct fs_vnode *vp, uint8 **buf, uint32 *buf_size,
                        uint32 *hdr_offset)
{
  (void) vp;
  (void) buf;
  (void) buf_size;
  (void) hdr_offset;

  return -ENODEV;
}

static int
vnodev_write_to_fsg (struct fs_vnode *vp, uint8 *buf, uint32 buf_size,
                     uint32 partition)
{
  (void) vp;
  (void) buf;
  (void) buf_size;
  (void) partition;

  return -ENODEV;
}

static const struct fs_mount_ops mnodev_ops = {
  0, 0, 0, 0, 0, 0, 0, 0
};

static const struct fs_mount mnodev_mpoint = {
  &mnodev_ops,
  FS_INVALID_DEV,
  0,
};

void
fs_vnode_make_nodev (struct fs_vnode *vp)
{
  vp->mp = (struct fs_mount *) &mnodev_mpoint;
  vp->dev = FS_INVALID_DEV;
  vp->inum = FS_INVALID_INODE;

  vp->dirty = 0;
  vp->vops = &fs_vnode_nodev_ops;
}

int
fs_vnode_invalid_chmod (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;

  return -EINVAL;
}

int
fs_vnode_invalid_chown (struct fs_vnode *vp, int uid_val, int gid_val)
{
  (void) vp;
  (void) uid_val;
  (void) gid_val;

  return -EINVAL;
}

int fs_vnode_invalid_utime (struct fs_vnode *vp,
                            const struct fs_utimbuf *times)
{
  (void) vp;
  (void) times;

  return -EINVAL;
}

int
fs_vnode_invalid_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -EINVAL;
}

int
fs_vnode_invalid_set_quota (struct fs_vnode *vp, uint32 gid, uint32 size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -EINVAL;
}

int
fs_vnode_invalid_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo)
{
  (void) vp;
  (void) gid;
  (void) ginfo;

  return -EINVAL;
}


int
fs_vnode_invalid_shred (struct fs_vnode *vp)
{
  (void) vp;
  return -EPERM;
}

int
fs_vnode_invalid_scrub (struct fs_vnode *vp)
{
  (void) vp;
  return -EPERM;
}

int
fs_vnode_invalid_sync_start (struct fs_vnode *vp, int do_force_sync,
                             int is_shutdown_sync)
{
  (void) vp;
  (void) do_force_sync;
  (void) is_shutdown_sync;

  return -EINVAL;
}

int
fs_vnode_invalid_sync_wait (struct fs_vnode *vp)
{
  (void) vp;
  return -EINVAL;
}

int
fs_vnode_invalid_sync_end (struct fs_vnode *vp)
{
  (void) vp;
  return -EINVAL;
}

int
fs_vnode_invalid_sync_no_wait (struct fs_vnode *vp,
                               fs_sync_token_t *sync_token)
{
  (void) vp;
  (void) sync_token;
  return -EINVAL;
}

int
fs_vnode_invalid_sync_get_status (struct fs_vnode *vp,
            fs_sync_token_t sync_token, enum fs_sync_result *sync_result)
{
  (void) vp;
  (void) sync_token;
  (void) sync_result;
  return -EINVAL;
}

int
fs_vnode_invalid_get_device_attr (struct fs_vnode *vp,
    struct fs_device_attr * dev_attr)
{
  (void) vp;
  (void) dev_attr;

  return -EINVAL;
}

int
fs_vnode_invalid_get_fs_data (struct fs_vnode *vp,
    struct fs_factimage_read_info * image_info_ptr, byte* page_buffer)
{
  (void) vp;
  (void) image_info_ptr;
  (void) page_buffer;

  return -EINVAL;
}

int
fs_vnode_invalid_image_prepare (struct fs_vnode *vp)
{
  (void) vp;

  return -EINVAL;
}

int
fs_vnode_invalid_get_volume_name (struct fs_vnode *vp, char *label, int len)
{
  (void) vp;
  (void) label;
  (void) len;

  return -EINVAL;
}

int
fs_vnode_invalid_set_volume_name (struct fs_vnode *vp, char *label)
{
  (void) vp;
  (void) label;

  return -EINVAL;
}

int
fs_vnode_invalid_make_golden_copy (struct fs_vnode *vp)
{
  (void) vp;

  return -EINVAL;
}

int
fs_vnode_invalid_fs_init_complete (struct fs_vnode *vp)
{
  (void) vp;

  return -EINVAL;
}

int
fs_vnode_invalid_lock_efs (struct fs_vnode *vp)
{
  (void) vp;

  return -EINVAL;
}

int
fs_vnode_invalid_borrow_ram (struct fs_vnode *vp, void **ram_addr,
                             uint32 *ram_size)
{
  (void) vp;
  (void) ram_addr;
  (void) ram_size;

  return -EINVAL;
}

int
fs_vnode_invalid_store_data_in_sb (struct fs_vnode *vp, const void *data,
                                   uint32 data_size)
{
  (void) vp;
  (void) data;
  (void) data_size;

  return -EINVAL;
}

int
fs_vnode_invalid_get_buf_for_fsg (struct fs_vnode *vp, uint8 **buf,
                                  uint32 *buf_size, uint32 *hdr_offset)
{
  (void) vp;
  (void) buf;
  (void) buf_size;
  (void) hdr_offset;

  return -EINVAL;
}

int
fs_vnode_invalid_write_to_fsg (struct fs_vnode *vp, uint8 *buf,
                               uint32 buf_size, uint32 partition)
{
  (void) vp;
  (void) buf;
  (void) buf_size;
  (void) partition;

  return -EINVAL;
}

void
fs_vnode_init_invalid_vops (struct vfs_ops *vops)
{
  vops->lookup           = fs_vnode_invalid_lookup;
  vops->create           = fs_vnode_invalid_create;
  vops->link             = fs_vnode_invalid_link;
  vops->unlink           = fs_vnode_invalid_unlink;
  vops->write            = fs_vnode_invalid_write;
  vops->read             = fs_vnode_invalid_read;
  vops->truncate         = fs_vnode_invalid_truncate;
  vops->mkdir            = fs_vnode_invalid_mkdir;
  vops->rmdir            = fs_vnode_invalid_rmdir;
  vops->opendir          = fs_vnode_invalid_opendir;
  vops->readdir          = fs_vnode_invalid_readdir;
  vops->closedir         = fs_vnode_invalid_closedir;
  vops->getstat          = fs_vnode_invalid_getstat;
  vops->getsize          = fs_vnode_invalid_getsize;
  vops->rename           = fs_vnode_invalid_rename;
  vops->symlink          = fs_vnode_invalid_symlink;
  vops->readlink         = fs_vnode_invalid_readlink;
  vops->getstatvfs       = fs_vnode_invalid_getstatvfs;
  vops->put_item         = fs_vnode_invalid_put_item;
  vops->get_item         = fs_vnode_invalid_get_item;
  vops->mknod            = fs_vnode_invalid_mknod;
  vops->chmod            = fs_vnode_invalid_chmod;
  vops->chown            = fs_vnode_invalid_chown;
  vops->set_reservation  = fs_vnode_invalid_set_reservation;
  vops->set_quota        = fs_vnode_invalid_set_quota;
  vops->get_group_info   = fs_vnode_invalid_get_group_info;
  vops->shred            = fs_vnode_invalid_shred;
  vops->scrub            = fs_vnode_invalid_scrub;
  vops->utime            = fs_vnode_invalid_utime;
  vops->utime_item       = fs_vnode_invalid_utime;
  vops->sync_start       = fs_vnode_invalid_sync_start;
  vops->sync_wait        = fs_vnode_invalid_sync_wait;
  vops->sync_end         = fs_vnode_invalid_sync_end;
  vops->sync_no_wait     = fs_vnode_invalid_sync_no_wait;
  vops->sync_get_status  = fs_vnode_invalid_sync_get_status;
  vops->get_device_attr  = fs_vnode_invalid_get_device_attr;
  vops->get_fs_data      = fs_vnode_invalid_get_fs_data;
  vops->image_prepare    = fs_vnode_invalid_image_prepare;
  vops->get_volume_name  = fs_vnode_invalid_get_volume_name;
  vops->set_volume_name  = fs_vnode_invalid_set_volume_name;
  vops->make_golden_copy = fs_vnode_invalid_make_golden_copy;
  vops->fs_init_complete = fs_vnode_invalid_fs_init_complete;
  vops->lock_efs         = fs_vnode_invalid_lock_efs;
  vops->borrow_ram       = fs_vnode_invalid_borrow_ram;
  vops->store_data_in_sb = fs_vnode_invalid_store_data_in_sb;
  vops->get_buf_for_fsg  = fs_vnode_invalid_get_buf_for_fsg;
  vops->write_to_fsg     = fs_vnode_invalid_write_to_fsg;
}


#ifdef FS_UNIT_TEST
/* Return a count of the number of referenced vnodes.  Used in unit tests
 * to detect vnode leaks. */
int
fs_vnode_refcount (void)
{
  int i, count;

  count = 0;
  for (i = 0; i < FS_VNODE_POOL_SIZE; i++) {
    if (vnode_pool[i].ref > 0) {
      count += vnode_pool[i].ref;
      if (vnode_pool[i].dirty)
        printf ("Warning: dirty vnode: %d\n", i);
    }
  }

  return count;
}
#endif

#ifdef FS_UNIT_TEST
/* Scan all vnodes to ensure that there are no more remaining references to
 * this mountpoint. */
void
fs_vnode_validate_umount (struct fs_mount *mp)
{
  int i;

  for (i = 0; i < FS_VNODE_POOL_SIZE; i++) {
    if (vnode_pool[i].ref > 0 && vnode_pool[i].mp == mp) {
      FS_ERR_FATAL ("umount left vnode referring to old FS", 0, 0, 0);
    }
  }
}
#endif /* FS_UNIT_TEST */
