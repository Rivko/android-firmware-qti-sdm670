/**********************************************************************
 * fs_romfs.c
 *
 * EFS2
 * Copyright (C) 2002-2014,2016 QUALCOMM Technologies, Inc.
 *
 * ROM filesystem mountpoint handler.
 *
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_romfs.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2016-04-14    rp      Add EFS golden copy creation from files list.
2014-01-29    vm      Changes for 64-bit compilation in boot.
2013-12-09    wek     Add existing feature flag for romfs.
2013-10-09    nr      Replace banned memory functions with safer versions.
2013-07-22    rp      Add new EFS API to store data in SuperBlock.
2013-06-19    dks     Added NAND Flash scrub support.
2013-04-26    wek     Move all buffers to DDR when compiling for boot.
2012-12-14    wek     Implement modified efs_utime for item files.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2012-01-17    wek     Remove unused utils header file.
2011-11-02    wek     Use library supplied strl cpy and cat instead of fs_strl*
2011-10-11    nr      Added borrow_ram support to borrow ram from lower layers.
2011-03-08    rp      Add support to lock EFS.
2011-01-12    vr      Support EFS removal based on FEATURE_EFS2.
2010-11-18    rp      Support TAR image Golden-Copy for RMTS.
2010-05-13    wek     Make sync_no_wait non blocking.
2010-05-06    wek     Implement golden copy for remote storage page manager.
2010-03-19    yog     Added support for files having max filesize upto 4GB.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-12-18    vr      Added set_volume_name and get_volume_name APIs.
2009-11-04    dks     Added vops for get_device_attr,get_fs_data,image_prepare.
2009-08-11    wek     On mount calls use void pointer instead of char.
2009-04-03    wek     Added utime ops & set access time on getstat.
2009-01-09    ebb     Removed dependency on FEATURE_EFS_QEFS2.
2008-11-03    rp      Introduce device abstraction.
2008-10-06    np      Added shred functionality to delete obsolete user data.
2008-01-24    sch     Added getsize to vnode ops
2007-12-07    rp      Enhancements to the statvfs struct.
2007-10-18    rp      Featurized QEFS2.
2007-08-18    rp      struct fs_vnode cleanup.
2007-05-25    umr     Added support for EFS memory protection feature.
2007-01-12    dlb     Use safer string functions.
2006-12-05    dlb     Max transaction size configurable.
2006-11-10    sh      Add chmod stub.
2006-07-06    dlb     Remove 'size' field from vnode.
2006-06-27    dlb     Pass stat through readdir.
2006-05-07    sh      Fixed hardalloc vs -1 compiler warning
2006-03-24    sh      Moved MAX mount count into common header.
2006-02-16    dlb     Set dev for root of romfs.
2006-01-11    nrs     Fixed Copyright header
2005-12-22    dlb     Extensions to statvfs results.
2005-10-18    nrs     Fixed create_inode
2005-08-25    nrs     Implent buffer accounting for quotas and reservations
2005-08-23    sh      Added mknod.
2005-08-03    dlb     Fix vnode leak on lookup.
2005-08-01    nrs     Adjusted vops_structure for reservations and quotas
2005-07-15    nrs     Update copyright header.
2005-07-15    nrs     Adjusted vops structure for efs_chown
2005-07-13    dlb     Mountpoint cleanup.
2005-07-12    dlb     Move transactions out of mount.
2004-10-15    dlb     Update copyright header.
2004-07-19    dlb     Restructure vnode interface.
2003-06-17    jkl     Clean up code.
2003-02-13    dlb     Added string.h.
2002-10-14    dlb     Created

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS2

#ifdef FEATURE_EFS_ROMFS

#include "comdef.h"
#include "fs_mount.h"
#include "fs_romfs.h"
#include "fs_errno.h"
#include "fs_util.h"

#include <string.h>
#include "stringl/stringl.h"
//#include "fs_protected_memory.h"

#define ROOT_INODE      2
#define FIRST_FILE_INODE        10

static struct fs_mount_romfs romfs_mounts[FS_MAX_ROMFS_MOUNTS];

static int r_start (struct fs_mount *mpp, const void *args);
static int r_stop (struct fs_mount *mpp);
static int r_creat_root (struct fs_mount *mpp);
static int r_get_root (struct fs_mount *mpp, struct fs_vnode **rp);
static struct fs_vnode *r_create_inode (struct fs_mount *mpp, fs_mode_t mode,
    uint32 gid, uint32 uid);
static int r_write_inode (struct fs_mount *mpp, struct fs_vnode *vp);
static int r_read_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_inode_t inum);

static const struct fs_mount_ops romfs_ops = {
  r_start,
  r_stop,
  r_get_root,
  r_create_inode,
  r_write_inode,
  r_read_inode,
  NULL, /* read_named_inode */
  NULL, /* cleanup. */
};

static int v_romfs_lookup (struct nameidata *ndp);
fs_ssize_t v_romfs_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count);
void *v_romfs_opendir (struct fs_vnode *vp);
int v_romfs_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent);
int v_romfs_closedir (struct fs_vnode *vp, void *dir);
static int v_romfs_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int v_romfs_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf);

static const struct vfs_ops romfs_dir_vops = {
  v_romfs_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  fs_vnode_invalid_mkdir,
  fs_vnode_invalid_rmdir,
  v_romfs_opendir,
  v_romfs_readdir,
  v_romfs_closedir,
  v_romfs_getstat,
  fs_vnode_invalid_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  v_romfs_getstatvfs,
  fs_vnode_invalid_put_item,
  fs_vnode_invalid_get_item,
  fs_vnode_invalid_mknod,
  fs_vnode_invalid_chmod,
  fs_vnode_invalid_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  fs_vnode_invalid_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_scrub,
  fs_vnode_invalid_utime,
  fs_vnode_invalid_utime, /* For ROMFS utime_item is the same as utime. */
  fs_vnode_invalid_sync_start,
  fs_vnode_invalid_sync_wait,
  fs_vnode_invalid_sync_end,
  fs_vnode_invalid_sync_no_wait,
  fs_vnode_invalid_sync_get_status,
  fs_vnode_invalid_get_device_attr,
  fs_vnode_invalid_get_fs_data,
  fs_vnode_invalid_image_prepare,
  fs_vnode_invalid_get_volume_name,
  fs_vnode_invalid_set_volume_name,
  fs_vnode_invalid_make_golden_copy,
  fs_vnode_invalid_fs_init_complete,
  fs_vnode_invalid_lock_efs,
  fs_vnode_invalid_borrow_ram,
  fs_vnode_invalid_store_data_in_sb,
  fs_vnode_invalid_get_buf_for_fsg,
  fs_vnode_invalid_write_to_fsg
};

static const struct vfs_ops romfs_file_vops = {
  fs_vnode_invalid_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  fs_vnode_invalid_write,
  v_romfs_read,
  fs_vnode_invalid_truncate,
  fs_vnode_invalid_mkdir,
  fs_vnode_invalid_rmdir,
  fs_vnode_invalid_opendir,
  fs_vnode_invalid_readdir,
  fs_vnode_invalid_closedir,
  v_romfs_getstat,
  fs_vnode_invalid_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  v_romfs_getstatvfs,
  fs_vnode_invalid_put_item,
  fs_vnode_invalid_get_item,
  fs_vnode_invalid_mknod,
  fs_vnode_invalid_chmod,
  fs_vnode_invalid_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  fs_vnode_invalid_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_scrub,
  fs_vnode_invalid_utime,
  fs_vnode_invalid_utime, /* For ROMFS utime_item is the same as utime. */
  fs_vnode_invalid_sync_start,
  fs_vnode_invalid_sync_wait,
  fs_vnode_invalid_sync_end,
  fs_vnode_invalid_sync_no_wait,
  fs_vnode_invalid_sync_get_status,
  fs_vnode_invalid_get_device_attr,
  fs_vnode_invalid_get_fs_data,
  fs_vnode_invalid_image_prepare,
  fs_vnode_invalid_get_volume_name,
  fs_vnode_invalid_set_volume_name,
  fs_vnode_invalid_make_golden_copy,
  fs_vnode_invalid_fs_init_complete,
  fs_vnode_invalid_lock_efs,
  fs_vnode_invalid_borrow_ram,
  fs_vnode_invalid_store_data_in_sb,
  fs_vnode_invalid_get_buf_for_fsg,
  fs_vnode_invalid_write_to_fsg
};

static int
r_start (struct fs_mount *mpp, const void *args)
{
  struct efs_romfs_node *nodes;
  struct fs_mount_romfs *mp = (struct fs_mount_romfs *) mpp;
  int count;

  nodes = (struct efs_romfs_node *) args;
  for (count = 0; nodes[count].name != NULL; count++)
    ;

  mp->nodes = nodes;
  mp->node_count = count;

  return r_creat_root (mpp);
}

static int
r_stop (struct fs_mount *mpp)
{
  (void) mpp;

  return 0;
}

static int
r_creat_root (struct fs_mount *mpp)
{
  struct fs_mount_romfs *mp = (struct fs_mount_romfs *) mpp;
  struct fs_vnode *root;

  root = fs_vnode_alloc ();
  if (root == NULL)
    return -ENOSPC;

  fs_inode_construct (&root->p.inode, S_IFDIR | 0755, FS_GROUP_ZERO,
                      FS_CURR_UID);

  root->mp = mpp;
  root->inum = ROOT_INODE;
  root->dirty = 0;
  root->vops = &romfs_dir_vops;
  root->mode = root->p.inode.mode;
  root->dev = mpp->dev;

  mp->root = root;

  return 0;
}

static int
r_get_root (struct fs_mount *mpp, struct fs_vnode **rp)
{
  struct fs_mount_romfs *mp = (struct fs_mount_romfs *) mpp;

  *rp = mp->root;

  return 0;
}

static struct
fs_vnode *r_create_inode (struct fs_mount *mpp, fs_mode_t mode, uint32 gid,
    uint32 uid)
{
  (void) mpp;
  (void) mode;
  (void) gid;
  (void) uid;
  return NULL;
}

static int
r_write_inode (struct fs_mount *mpp, struct fs_vnode *vp)
{
  (void) mpp;
  (void) vp;
  return -EINVAL;
}

static int r_read_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_inode_t inum)
{
  struct fs_mount_romfs *mp = (struct fs_mount_romfs *) mpp;
  size_t *temp;

  /* XXX: Check that inum is valid. */

  /* There are no subdirectories. */
  vp->vops = &romfs_file_vops;

  /* Construct a meaningful inode. */
  fs_inode_construct (&vp->p.inode, S_IFREG | 0444, FS_GROUP_ZERO,
      FS_CURR_UID);

  vp->p.inode.size = mp->nodes[inum - FIRST_FILE_INODE].length;
  vp->p.inode.nlink = 1;
  vp->p.inode.blocks = 0;
  vp->p.inode.mtime = 0;
  vp->p.inode.ctime = 0;

  vp->mode = vp->p.inode.mode;

  /* Storing a 64-bit address into 2 consecutive 32-bit array elements holding
   * cluster-id.
   */
  temp = (size_t *) vp->p.inode.data;
  *temp = (size_t)  mp->nodes[inum - FIRST_FILE_INODE].data;

  return 0;
}

static int
v_romfs_lookup (struct nameidata *ndp)
{
  struct fs_mount_romfs *mp = (struct fs_mount_romfs *) ndp->dvp->mp;
  struct fs_vnode *vp;
  int i;

  /* Special cases for "." and "..". */
  if (ndp->length == 0 ||
      (ndp->length == 1 && ndp->next[0] == '.') ||
      (ndp->length == 2 && ndp->next[0] == '.' && ndp->next[1] == '.')) {
    fs_vnode_ref (ndp->dvp);
    ndp->vp = ndp->dvp;

    return 0;
  }

  for (i = 0; i < mp->node_count; i++) {
    if (strlen (mp->nodes[i].name) == (unsigned) ndp->length &&
        memcmp (mp->nodes[i].name, ndp->next, ndp->length) == 0)
    {
      break;
    }
  }

  if (i == mp->node_count)
    return -ENOENT;

  /* Lookup the inode, and construct it if it isn't yet available. */
  vp = fs_vnode_lookup (&mp->parent, mp->parent.dev, i + FIRST_FILE_INODE);
  if (vp == NULL)
    return -EINVAL;
  ndp->vp = vp;
  return 0;
}

fs_ssize_t
v_romfs_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count)
{
  size_t temp = 0;
  uint8 *file_data;

  /* To make it compatible for both 32 bit and 64 bit addresses
   * Forming the 64-bit address from 2 consecutive elements each having
   * 32-bit address of the data cluster.
   */
  fs_memscpy (&temp, sizeof (temp), vp->p.inode.data, sizeof (size_t *));
  file_data = (uint8 *) temp;

  /* Check the constraints, and copy the data if necessary. */

  if (pos >= vp->p.inode.size)
    return 0;

  if (pos + count > vp->p.inode.size)
    count = vp->p.inode.size - pos;

  fs_memscpy (buf, count, file_data + pos, count);

  return count;
}

/* The iterators just need to point to a simple offset.  We use 0 to
 * indicate that they are free (makes initialization easy), 1 and 2 are the
 * dir nodes, and 3 and on are the actual entries. */
#define FS_MAX_ROMFS_ITERS   3
static int dir_offsets[FS_MAX_ROMFS_ITERS] = { 0 };

void *
v_romfs_opendir (struct fs_vnode *vp)
{
  int i;
  (void) vp;

  for (i = 0; i < FS_MAX_ROMFS_ITERS; i++)
    if (dir_offsets[i] == 0)
      break;

  if (i == FS_MAX_ROMFS_ITERS)
    return NULL;

  dir_offsets[i] = 1;

  return &dir_offsets[i];
}

int
v_romfs_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent)
{
  struct fs_mount_romfs *mp = (struct fs_mount_romfs *) vp->mp;
  int *doff = (int *) dir;
  const char *target;
  int length;

  if (*doff <= 0)
    return -EINVAL;

  switch (*doff) {
    case 1:
      target = ".";
      break;
    case 2:
      target = "..";
      break;
    default:
      /* This isn't what posix says... */
      if (*doff - 3 >= mp->node_count)
        return -EEOF;

      target = mp->nodes[*doff - 3].name;
  }

  length = strlen (target);
  if (length > FS_NAME_MAX)
    return -ENAMETOOLONG;

  strlcpy (dirent->d_name, target, sizeof (dirent->d_name));
  dirent->d_stats_present = 0;

  (*doff)++;
  return 0;
}

int
v_romfs_closedir (struct fs_vnode *vp, void *dir)
{
  (void) vp;
  *((int *) dir) = 0;

  return 0;
}

static int
v_romfs_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode = vp->p.inode.mode;
  buf->st_nlink = vp->p.inode.nlink;
  buf->st_size = vp->p.inode.size;

  buf->st_blksize = 0;

  buf->st_blocks = vp->p.inode.blocks;
  buf->st_atime = vp->p.inode.atime;
  buf->st_mtime = vp->p.inode.mtime;
  buf->st_ctime = vp->p.inode.ctime;

  return 0;
}

static int
v_romfs_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  struct fs_mount_romfs *mp = (struct fs_mount_romfs *) vp->mp;
  int i;
  int romfs_size = 0;

  for (i = 0; i < mp->node_count; i++)
  {
    romfs_size += mp->nodes[i].length;
  }
  buf->f_bsize   = 1;
  buf->f_blocks  = romfs_size;
  buf->f_bfree   = 0;
  buf->f_bavail  = 0;
  buf->f_ffree   = 0;
  buf->f_favail  = 0;
  buf->f_files   = mp->node_count;
  buf->f_fsid    = 0;
  buf->f_flag    = 0;
  buf->f_namemax = FS_NAME_MAX;
  buf->f_balloc  = romfs_size;
  buf->f_hardalloc = FS_INVALID_HARDALLOC;
  buf->f_maxwrite = FS_INVALID_MAXWRITE;

  buf->f_pathmax = FS_PATH_MAX;
  buf->f_is_case_sensitive = 1; /* Path is case-sensitive. */
  buf->f_is_case_preserving = 1; /* Path is case-preserved. */
  buf->f_max_file_size = FS_MAX_FILE_SIZE;
  buf->f_max_file_size_unit = 1;
  buf->f_max_open_files = FS_MAX_DESCRIPTORS;
  buf->f_name_rule = FS_FILENAME_RULE_8BIT_RELAXED;
  buf->f_name_encoding = FS_FILENAME_ENCODING_8BIT;

  return 0;
}

void
fs_romfs_init (void)
{
  int i;
  for (i = 0; i < FS_MAX_ROMFS_MOUNTS; i++) {
    romfs_mounts[i].root = NULL;
    romfs_mounts[i].parent.ops = &romfs_ops;
    fs_mount_register ("romfs", &romfs_mounts[i].parent);
  }
}
#else /* FEATURE_EFS_ROMFS */
void
fs_romfs_init (void)
{
  /* ROMFS Disabled, do nothing.*/
}

#endif /* FEATURE_EFS_ROMFS */

#endif /* FEATURE_EFS2 */
