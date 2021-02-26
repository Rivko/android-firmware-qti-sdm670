/**********************************************************************
 * fs_extfs.c
 *
 * EFS2
 * Copyright (C) 2004-2013,2016 QUALCOMM Technologies, Inc.
 *
 * External filesystem mountpoint handler.
 *
 ***********************************************************************/

/*======================================================================
 *
 * EDIT HISTORY
 *
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_extfs.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $
 *
 * when        who  what, where, why
 * ----------  ---  ----------------------------------------------------
 * 2016-04-14  rp   Add EFS golden copy creation from files list.
 * 2013-10-09  nr   Replace banned memory functions with safer versions.
 * 2013-07-22  rp   Add new EFS API to store data in SuperBlock.
 * 2013-06-19  dks  Added NAND Flash scrub support.
 * 2012-12-14  wek  Implement modified efs_utime for item files.
 * 2012-08-14  wek  Implement chmod for item files.
 * 2012-01-17  wek  Remove unused utils header file.
 * 2011-11-02  wek  Use library supplied strl cpy and cat instead of fs_strl*.
 * 2011-10-11  nr   Added borrow_ram support to borrow ram from lower layers.
 * 2011-10-04  dks  Make EFS VU independent of Hotplug and Hfat VUs.
 * 2011-03-08  rp   Add support to lock EFS.
 * 2010-11-18  rp   Support TAR image Golden-Copy for RMTS.
 * 2010-11-09  dks  Fix coding style issues in allocate and free_drive_numbers
 * 2010-11-08  dks  Take Global lock before accessing fs_drive_in_use.
 * 2010-10-06  rp   Fix return value in ext_start.
 * 2010-05-13  wek  Make sync_no_wait non blocking.
 * 2010-05-06  wek  Implement golden copy for remote storage page manager.
 * 2010-03-19  yog  Added support for files having max filesize upto 4GB.
 * 2010-02-19  rp   Maintain same API files between 1.2 and 2.0 codebases.
 * 2010-01-19  vr   Added efs_sync_no_wait and efs_sync_get_status APIs.
 * 2009-12-18  vr   Added set_volume_name and get_volume_name APIs.
 * 2009-11-04  dks  Added vops for get_device_attr, get_fs_data, image_prepare
 * 2009-08-11  wek  Remove hfat drive number coupled to hotplug dev index.
 * 2009-05-18  rp   Removed obsolete feature FS_FEATURE_RMTEFS_SERVER
 * 2009-05-08  ebb  Removed SFAT support.
 * 2009-04-03  wek  Added utime ops to set access/mod time.
 * 2008-10-06  np   Added shred functionality to delete obsolete user data.
 * 2008-01-24  sch  Added ext_getsize
 * 2007-08-18  rp   struct fs_vnode cleanup.
 * 2007-05-25  umr  Added support for EFS memory protection feature.
 * 2007-01-12  dlb  Use safer string functions.
 * 2006-11-10  sh   Add chmod support for FAT READONLY attribute
 * 2006-08-27  sh   Added HFAT option and FS_FAT_xxx macros
 * 2006-07-07  dlb  Implement truncate.
 * 2006-07-06  dlb  Remove 'size' field from vnode.
 * 2006-06-27  dlb  Pass stat through readdir.
 * 2006-05-12  dlb  Fix construction of inodes on create.
 * 2006-03-24  sh   Moved MAX mount count into common header.
 * 2006-02-16  dlb  Null terminate vnode name.
 * 2006-01-11  nrs  Fixed Copyright header
 * 2005-11-10  nrs  Merge fix
 * 2005-11-09  sh   Renamed FS_FEATURE_EXTFS to FEATURE_EFS_EXTFS
 * 2005-11-01  nrs  Merge into Q&R
 * 2005-10-24  dlb  Cleanup for SFAT release.
 * 2005-10-21  dlb  Eliminate some compiler warnings.
 * 2005-10-11  dlb  Pass name through to stop.
 * 2005-08-11  dlb  Add umount support.
 * 2005-07-13  dlb  Mountpoint cleanup.
 * 2005-07-12  dlb  Move transactions out of mount.
 * 2005-06-06  dlb  Extensions for SFAT.
 * 2004-10-07  dlb  Whitespace cleanup.
 * 2004-06-25  dlb  Created
 *=====================================================================*/

#include "fs_extfs.h"
#include "fs_errno.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_fcntl.h"
#include "assert.h"
#include "stringl/stringl.h"

#ifdef FEATURE_FS_COLDPLUG
  #include "coldplug_api.h"
  #include "coldplug_priv_api.h"
#endif

#ifdef FEATURE_EFS_EXTFS

#include "fs_ext_hfat.h"
//#include "fs_protected_memory.h"
//#include "fs_hfat_api.h"
#include "fs_global_lock.h"


#if defined(FEATURE_EFS_EXTFS_HFAT) && !defined(FEATURE_FS_COLDPLUG)
  #error "HFAT needs COLDPLUG"
#endif

static struct fs_mount_extfs extfs_mounts[FS_MAX_EXTFS_MOUNTS];

static int ext_start (struct fs_mount *mpp, const void *path);
static int ext_stop (struct fs_mount *mpp);
static int ext_get_root (struct fs_mount *mpp, struct fs_vnode **rp);
static struct fs_vnode *ext_create_inode (struct fs_mount *mpp,
    fs_mode_t mode, uint32 gid, uint32 uid);
static int ext_write_inode (struct fs_mount *mp, struct fs_vnode *vp);
static int ext_read_inode (struct fs_mount *mp, struct fs_vnode *vp,
    fs_inode_t inum);
static int ext_read_named_inode (struct fs_mount *mp, struct fs_vnode *vp,
    fs_mode_t mode, const char *key, int key_len);
static void ext_vnode_cleanup (struct fs_mount *mpp, struct fs_vnode *vp);

const struct fs_mount_ops extfs_ops = {
  ext_start,
  ext_stop,
  ext_get_root,
  ext_create_inode,
  ext_write_inode,
  ext_read_inode,
  ext_read_named_inode,
  ext_vnode_cleanup,
};

/* External FS specific operations. */
static int ext_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf);
static int ext_lookup (struct nameidata *ndp);
static int ext_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int ext_getsize (struct fs_vnode *vp, uint32 *numbytes);
static void *ext_opendir (struct fs_vnode *vp);
static int ext_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent);
static int ext_closedir (struct fs_vnode *vp, void *dir);
static fs_ssize_t ext_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count);
static fs_ssize_t ext_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode);
static int ext_create (struct nameidata *ndp, fs_mode_t mode);
static int ext_mkdir (struct nameidata *ndp, fs_mode_t mode);
static int ext_rmdir (struct nameidata *ndp);
static int ext_unlink (struct nameidata *ndp);
static int ext_rename (struct nameidata *ndp, struct nameidata *ndp2);
static int ext_truncate (struct fs_vnode *vp, fs_size_t pos);
static int ext_chmod (struct nameidata *ndp, fs_mode_t mode);
static int ext_utime (struct fs_vnode *vp, const struct fs_utimbuf *times);
static int ext_get_volume_name (struct fs_vnode *vp, char *label, int len);
static int ext_set_volume_name (struct fs_vnode *vp, char *label);

static const struct vfs_ops extfs_rootdir_vops = {
  ext_lookup,
  ext_create,
  fs_vnode_invalid_link,
  ext_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  ext_mkdir,
  ext_rmdir,
  ext_opendir,
  ext_readdir,
  ext_closedir,
  ext_getstat,
  fs_vnode_invalid_getsize,
  ext_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  ext_getstatvfs,
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
  fs_vnode_invalid_utime, /* For EXTFS utime_item is the same as utime. */
  fs_vnode_invalid_sync_start,
  fs_vnode_invalid_sync_wait,
  fs_vnode_invalid_sync_end,
  fs_vnode_invalid_sync_no_wait,
  fs_vnode_invalid_sync_get_status,
  fs_vnode_invalid_get_device_attr,
  fs_vnode_invalid_get_fs_data,
  fs_vnode_invalid_image_prepare,
  ext_get_volume_name,
  ext_set_volume_name,
  fs_vnode_invalid_make_golden_copy,
  fs_vnode_invalid_fs_init_complete,
  fs_vnode_invalid_lock_efs,
  fs_vnode_invalid_borrow_ram,
  fs_vnode_invalid_store_data_in_sb,
  fs_vnode_invalid_get_buf_for_fsg,
  fs_vnode_invalid_write_to_fsg
};

/* Vops for extfs entities. */
static const struct vfs_ops extfs_dir_vops = {
  fs_vnode_invalid_lookup,
  ext_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  ext_mkdir,
  ext_rmdir,
  ext_opendir,
  ext_readdir,
  ext_closedir,
  ext_getstat,
  fs_vnode_invalid_getsize,
  ext_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  ext_getstatvfs,
  fs_vnode_invalid_put_item,
  fs_vnode_invalid_get_item,
  fs_vnode_invalid_mknod,
  ext_chmod,
  fs_vnode_invalid_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  fs_vnode_invalid_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_scrub,
  ext_utime,
  ext_utime, /* For EXTFS utime_item is the same as utime. */
  fs_vnode_invalid_sync_start,
  fs_vnode_invalid_sync_wait,
  fs_vnode_invalid_sync_end,
  fs_vnode_invalid_sync_no_wait,
  fs_vnode_invalid_sync_get_status,
  fs_vnode_invalid_get_device_attr,
  fs_vnode_invalid_get_fs_data,
  fs_vnode_invalid_image_prepare,
  ext_get_volume_name,
  ext_set_volume_name,
  fs_vnode_invalid_make_golden_copy,
  fs_vnode_invalid_fs_init_complete,
  fs_vnode_invalid_lock_efs,
  fs_vnode_invalid_borrow_ram,
  fs_vnode_invalid_store_data_in_sb,
  fs_vnode_invalid_get_buf_for_fsg,
  fs_vnode_invalid_write_to_fsg
};

static const struct vfs_ops extfs_file_vops = {
  fs_vnode_invalid_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  ext_write,
  ext_read,
  ext_truncate,
  fs_vnode_invalid_mkdir,
  fs_vnode_invalid_rmdir,
  fs_vnode_invalid_opendir,
  fs_vnode_invalid_readdir,
  fs_vnode_invalid_closedir,
  ext_getstat,
  ext_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  ext_getstatvfs,
  fs_vnode_invalid_put_item,
  fs_vnode_invalid_get_item,
  fs_vnode_invalid_mknod,
  ext_chmod,
  fs_vnode_invalid_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  fs_vnode_invalid_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_scrub,
  ext_utime,
  ext_utime, /* For EXTFS utime_item is the same as utime. */
  fs_vnode_invalid_sync_start,
  fs_vnode_invalid_sync_wait,
  fs_vnode_invalid_sync_end,
  fs_vnode_invalid_sync_no_wait,
  fs_vnode_invalid_sync_get_status,
  fs_vnode_invalid_get_device_attr,
  fs_vnode_invalid_get_fs_data,
  fs_vnode_invalid_image_prepare,
  ext_get_volume_name,
  ext_set_volume_name,
  fs_vnode_invalid_make_golden_copy,
  fs_vnode_invalid_fs_init_complete,
  fs_vnode_invalid_lock_efs,
  fs_vnode_invalid_borrow_ram,
  fs_vnode_invalid_store_data_in_sb,
  fs_vnode_invalid_get_buf_for_fsg,
  fs_vnode_invalid_write_to_fsg
};

/* For other filetypes.  Just make everything invalid. */
static const struct vfs_ops extfs_other_vops = {
  fs_vnode_invalid_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  fs_vnode_invalid_mkdir,
  fs_vnode_invalid_rmdir,
  fs_vnode_invalid_opendir,
  fs_vnode_invalid_readdir,
  fs_vnode_invalid_closedir,
  fs_vnode_invalid_getstat,
  fs_vnode_invalid_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  ext_getstatvfs,
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
  fs_vnode_invalid_utime, /* For EXTFS utime_item is the same as utime. */
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

#define ROOT_INODE      2
#define INVALID_INODE_NUMBER  0xffffffff

/* FD modes.  Determins how the underlying file was opened. */
#define FD_MODE_NONE    0
#define FD_MODE_READ    1
#define FD_MODE_READWRITE 2

/* In order to initialize an FAT volume we need to assing a letter to it.
 * We need to add the letter prefix to file/directory access to FAT.
 * This array holds wich drive letter is in use and by wich hotplug device,
 * offset 0 is 'a', 1 is 'b'...
 */
static void *fs_drive_in_use[FS_HFAT_MAXVOLUME_COUNT];

/* Marks one of the drive as in use and returns the assigned drive
 * letter. Returns -1 on error.
 */
static int
allocate_drive_number (void *dev)
{
  int i;

  ASSERT (dev != NULL);

  fs_global_lock ();

  for (i = 0; i < FS_HFAT_MAXVOLUME_COUNT; i++)
    if (fs_drive_in_use[i] == NULL)
    {
      fs_drive_in_use[i] = dev;
      fs_global_unlock();
      return i;
    }

  fs_global_unlock ();

  return -1;
}

#if 0
/* Gets the dirive number letter (a=0, b=1....) for the given pointer.
 * the pointer was stored eariler by allocate in the list. */
static int
get_drive_number (void *dev)
{
  int i;

  ASSERT (dev != NULL);
  for (i = 0; i < FS_HFAT_MAXVOLUME_COUNT; i++)
    if (fs_drive_in_use[i] == dev)
      return i;

  return -1;
}
#endif

/* Given the driveno it frees that drive letter. driveno = drive_letter - 'a'
 */
static void*
free_drive_number (int driveno)
{
  void *hdev = NULL;

  fs_global_lock ();

  ASSERT (fs_drive_in_use[driveno] != NULL);
  hdev = fs_drive_in_use[driveno];
  fs_drive_in_use[driveno] = NULL;

  fs_global_unlock ();

  return hdev;
}

static int
ext_start (struct fs_mount *mpp, const void *path)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) mpp;
  struct fs_vnode *root;
  int result, driveno;
  struct coldplug_handle *chdl;

  chdl = coldplug_open_partition (path);
  if (chdl == NULL)
    return -1;

  driveno = allocate_drive_number ((void*)chdl);

  /* Check if we allocated all the available slots */
  if (driveno < 0)
    return -ENOMEM;

  mp->prefix[0] = 'a' + driveno;
  mp->prefix[1] = ':';
  mp->prefix[2] = '\0';

  root = fs_vnode_alloc ();
  if (root == NULL)
    return -ENOSPC;

  /* No, these don't have inodes. */
  /* fs_inode_construct (&root->p.inode, S_IFDIR | 0755); */

  root->mp = mpp;
  root->dev = mpp->dev;
  root->inum = ROOT_INODE;
  root->dirty = 0;
  root->vops = &extfs_rootdir_vops;
  root->mode = S_IFDIR | 0755;

  /* Be sure to fill in the prefix as the name, so that stat of the root
   * directory works.  Note that some filesystems will want a slash and
   * some won't.  For now we put the trailing slash, and assume that future
   * filesystems that don't want it will change this to be configurable. */
  strlcpy (root->p.named.key, mp->prefix, sizeof (root->p.named.key));
  strlcat (root->p.named.key, "/", sizeof (root->p.named.key));
  root->p.named.key_length = strlen (root->p.named.key);
  root->p.named.p.extfs.fd = -1;
  root->p.named.p.extfs.fd_mode = 0;

  mp->root = root;

  result = mp->ops->start (mp->prefix, chdl);
  if (result != 0)
  {
    chdl = (struct coldplug_handle * )free_drive_number (driveno);
    (void) coldplug_close_partition (chdl);
    fs_vnode_unref (root);
    mp->root = NULL;
  }
  return result;
}

static int
ext_stop (struct fs_mount *mpp)
{
  int result, letter;
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) mpp;
  struct coldplug_handle *chdl;

  letter = mp->prefix[0] - 'a';

  result = mp->ops->stop (mp->prefix);

  chdl = (struct coldplug_handle * )free_drive_number (letter);
  (void) coldplug_close_partition (chdl);

  return result;
}

static int
ext_get_root (struct fs_mount *mpp, struct fs_vnode **rp)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) mpp;

  if (mp->root == NULL)
    return -EINVAL;

  *rp = mp->root;
  return 0;
}

static struct
fs_vnode *ext_create_inode (struct fs_mount *mpp,
    fs_mode_t mode, uint32 uid, uint32 gid)
{
  (void) mpp;
  (void) mode;
  (void) uid;
  (void) gid;

  FS_ERR_FATAL ("ext_create_inode", 0, 0, 0);

  /* Eliminate compiler warning. */
  return NULL;
}

static int
ext_write_inode (struct fs_mount *mp, struct fs_vnode *vp)
{
  (void) mp;
  (void) vp;

  FS_ERR_FATAL ("ext_write_inode", 0, 0, 0);

  /* Eliminate compiler warning. */
  return -EINVAL;
}

static int
ext_read_inode (struct fs_mount *mp, struct fs_vnode *vp,
    fs_inode_t inum)
{
  (void) mp;
  (void) vp;
  (void) inum;

  FS_ERR_FATAL ("ext_read_inode", 0, 0, 0);

  /* Compiler warning. */
  return -EINVAL;
}

/* Construct an inode for a given named entity.  This needs to determine
 * the type of the file, and fill in the appropriate methods. */
static int
ext_read_named_inode (struct fs_mount *mpp, struct fs_vnode *vp,
    fs_mode_t mode, const char *key, int key_len)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) mpp;
  int result;

  (void) mode;   /* Not used. */
  (void) mpp;

  vp->p.named.key_length = key_len;
  fs_memscpy (vp->p.named.key, sizeof (vp->p.named.key), key, key_len);
  vp->p.named.key[key_len] = '\0';

  result = mp->ops->base_stat (vp->p.named.key, &vp->mode);

  if (result < 0)
    return result;

  vp->p.named.p.extfs.fd = -1;
  vp->p.named.p.extfs.fd_mode = 0;

  if (S_ISREG (vp->mode)) {
    vp->vops = &extfs_file_vops;
  } else if (S_ISDIR (vp->mode)) {
    vp->vops = &extfs_dir_vops;
  } else {
    vp->vops = &extfs_other_vops;
  }

  return 0;
}

static void
ext_vnode_cleanup (struct fs_mount *mpp, struct fs_vnode *vp)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) mpp;

  if (vp->p.named.p.extfs.fd_mode != FD_MODE_NONE) {
    (void) mp->ops->close (vp->p.named.p.extfs.fd);
    vp->p.named.p.extfs.fd_mode = FD_MODE_NONE;
    vp->p.named.p.extfs.fd = -1;
  }
}

/* Implementation specific to a given external filesystem. */

/* Local buffer for external filesystems.  This is used to build up
 * pathnames. */
static char ext_tmp_path[FS_PATH_MAX+1];

/* Lookup is different than internal filesystems.  We grab the entire rest
 * of the name, and just look that up locally. */
static int
ext_lookup (struct nameidata *ndp)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) ndp->dvp->mp;
  struct fs_vnode *vp;
  const int length = strlen (ndp->next);
  const char *name;
  int tlength;

  /* printf ("ext_lookup: \"%s\"\n", ndp->next); */

  /* Adjust the length field to indicate to namei that we've parsed the
   * entire name. */
  ndp->length = length;

  if (strlen (mp->prefix) + length + 2 >= FS_PATH_MAX)
    return -ENAMETOOLONG;

  strlcpy (ext_tmp_path, mp->prefix, sizeof (ext_tmp_path));
  strlcat (ext_tmp_path, "/", sizeof (ext_tmp_path));

  name = ndp->next;

  /* FAT doesn't have a "." or ".." entry in the root directory.  Since it
   * is usually the root of the filesystem that is the root of the
   * mountpoint, "." and ".." won't work there.  Go through the beginning
   * of the path given, and make a simple attempt to remove any "." or ".."
   * as well as extra slashes that appear in the name given. */
  while (1) {
    if (name[0] == '.' && name[1] == '/')
      name += 2;
    else if (name[0] == '.' && name[1] == 0)
      name++;
    else if (name[0] == '.' && name[1] == '.' && name[2] == '/')
      name += 3;
    else if (name[0] == '.' && name[1] == '.' && name[2] == 0)
      name += 2;
    else
      break;
  }

  strlcat (ext_tmp_path, name, sizeof (ext_tmp_path));

  tlength = strlen (ext_tmp_path);

  /* Look up this name in the vnode cache.  If the name is found, use the
   * same vnode to refer to this file (meaning that multiple opens of the
   * same file will use the same SFAT file handle). */
  vp = fs_vnode_named_lookup (&mp->parent, mp->parent.dev,
      0, ext_tmp_path, tlength);

  if (vp == NULL)
    return -ENOENT;

  ndp->vp = vp;

  return 0;
}

static int
ext_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->statvfs (mp->prefix, buf);
}

static void *
ext_opendir (struct fs_vnode *vp)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->opendir (vp->p.named.key);
}

static int
ext_readdir (struct fs_vnode *vp, void *dir, struct fs_dirent *dirent)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->readdir (dir, dirent);
}

static int
ext_closedir (struct fs_vnode *vp, void *dir)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->closedir (dir);
}

static int
ext_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  buf->st_dev = mp->parent.dev;
  buf->st_ino = INVALID_INODE_NUMBER;

  return mp->ops->getstat (vp->p.named.key, buf);
}

/* This function gets the size of the given file in bytes. It succeeds
 * only if the file is already open in both EFS and underlying file system.
 * getstat() should be used to get the size of the file in case this function
 * fails.
 */
static int
ext_getsize (struct fs_vnode *vp, uint32 *numbytes)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->getsize (vp->p.named.p.extfs.fd, numbytes);
}

/* Ensure that the descriptor for the given vnode is opened for at least
 * the required mode.  Should always be called with either FD_MODE_READ or
 * FD_MODE_READWRITE.  Returns 0 on success or a negative errno. */
static int
ext_set_open_mode (struct fs_vnode *vp, int required_mode)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;
  int fd;
  int mode;

  /* Sanity check on the argument. */
  ASSERT (required_mode == FD_MODE_READ ||
      required_mode == FD_MODE_READWRITE);

  /* If we're requesting write and it was only opened in read mode, close
   * it first. */
  if (required_mode == FD_MODE_READWRITE &&
      vp->p.named.p.extfs.fd_mode == FD_MODE_READ)
  {
    (void) mp->ops->close (vp->p.named.p.extfs.fd);
    vp->p.named.p.extfs.fd_mode = FD_MODE_NONE;
  }

  /* Open the descriptor, if it isn't already open. */
  if (vp->p.named.p.extfs.fd_mode == FD_MODE_NONE) {
    mode = O_RDONLY;
    if (required_mode == FD_MODE_READWRITE)
      mode = O_RDWR;

    fd = mp->ops->open (vp->p.named.key, mode);

    if (fd < 0)
      return fd;

    vp->p.named.p.extfs.fd_mode = required_mode;
    vp->p.named.p.extfs.fd = fd;
  }

  return 0;
}

static fs_ssize_t
ext_read (struct fs_vnode *vp, fs_size_t pos,
    void *buf, fs_size_t count)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;
  int fd;
  int result;
  int rcount;

  /* Make sure the FD is opened. */
  result = ext_set_open_mode (vp, FD_MODE_READ);
  if (result < 0) {
    return result;
  }

  fd = vp->p.named.p.extfs.fd;

  /* set the mode as O_RDONLY. */
  result = mp->ops->lseek (fd, pos, O_RDONLY);
  if (result < 0) {
    return result;
  }

  rcount = mp->ops->read (fd, buf, count);

  return rcount;
}

static fs_ssize_t
ext_write (struct fs_vnode *vp, fs_size_t pos,
    const void *buf, fs_size_t count, int mode)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;
  int fd;
  int result;
  int rcount;

  /* Make sure the FD is opened right. */
  result = ext_set_open_mode (vp, FD_MODE_READWRITE);
  if (result < 0) {
    return result;
  }

  fd = vp->p.named.p.extfs.fd;

  result = mp->ops->lseek (fd, pos, mode);
  if (result < 0) {
    return result;
  }

  rcount = mp->ops->write (fd, buf, count);

  return rcount;
}

static int
ext_create (struct nameidata *ndp, fs_mode_t mode)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) ndp->dvp->mp;
  int length = strlen (ndp->next);
  struct fs_vnode *vp;
  int result;
  int tlength;

  if (strlen (mp->prefix) + length + 2 >= FS_PATH_MAX)
    return -ENAMETOOLONG;

  strlcpy (ext_tmp_path, mp->prefix, sizeof (ext_tmp_path));
  strlcat (ext_tmp_path, "/", sizeof (ext_tmp_path));
  strlcat (ext_tmp_path, ndp->next, sizeof (ext_tmp_path));

  tlength = strlen (ext_tmp_path);

  /* Ignore mode. */
  (void) mode;

  vp = fs_vnode_alloc ();
  if (vp == NULL)
    return -ENOSPC;

  /* Construct the vnode correctly. */
  vp->dev = mp->parent.dev;
  vp->inum = FS_INVALID_INODE;
  vp->mp = &mp->parent;
  vp->dirty = 0;

  /* Initialize the vnode members first, in case open fails we have to unref.*/
  /* From ext_read_named_inode */
  vp->p.named.key_length = tlength;
  strlcpy (vp->p.named.key, ext_tmp_path, sizeof (vp->p.named.key));
  vp->mode = S_IFREG | 0644;
  vp->vops = &extfs_file_vops;
  vp->p.named.p.extfs.fd = FD_MODE_NONE;

  result = mp->ops->open (ext_tmp_path, O_CREAT | O_EXCL | O_RDWR);
  if (result < 0) {
    fs_vnode_unref (vp);
    return result;
  }

  vp->p.named.p.extfs.fd_mode = FD_MODE_READWRITE;
  vp->p.named.p.extfs.fd = result;

  ndp->vp = vp;

  return 0;
}

static int
ext_mkdir (struct nameidata *ndp, fs_mode_t mode)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) ndp->dvp->mp;
  int length = strlen (ndp->next);

  /* No modes here. */
  (void) mode;

  if (strlen (mp->prefix) + length + 2 >= FS_PATH_MAX)
    return -ENAMETOOLONG;

  strlcpy (ext_tmp_path, mp->prefix, sizeof (ext_tmp_path));
  strlcat (ext_tmp_path, "/", sizeof (ext_tmp_path));
  strlcat (ext_tmp_path, ndp->next, sizeof (ext_tmp_path));

  return mp->ops->mkdir (ext_tmp_path);
}

static int
ext_rmdir (struct nameidata *ndp)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) ndp->dvp->mp;
  int result;

  result = mp->ops->rmdir (ndp->vp->p.named.key);

  if (result == 0) {
    fs_vnode_unref (ndp->vp);
    ndp->vp = NULL;
  }

  return result;
}

static int
ext_unlink (struct nameidata *ndp)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) ndp->dvp->mp;
  int result;

  result = mp->ops->unlink (ndp->vp->p.named.key);

  fs_vnode_unref (ndp->vp);
  ndp->vp = NULL;

  return result;
}

static int
ext_rename (struct nameidata *ndp, struct nameidata *ndp2)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) ndp->dvp->mp;
  int result;

  /* If the destination exists, we already have a name for it. */
  if (ndp2->vp != NULL) {
    result = mp->ops->rename (ndp->vp->p.named.key,
        ndp2->vp->p.named.key);
  } else {
    /* Construct the new name. */
    if (strlen (mp->prefix) + strlen (ndp2->next) + 2 >= FS_PATH_MAX)
      return -ENAMETOOLONG;

    strlcpy (ext_tmp_path, mp->prefix, sizeof (ext_tmp_path));
    strlcat (ext_tmp_path, "/", sizeof (ext_tmp_path));
    strlcat (ext_tmp_path, ndp2->next, sizeof (ext_tmp_path));

    result = mp->ops->rename (ndp->vp->p.named.key, ext_tmp_path);
  }

  return result;
}

static int
ext_truncate (struct fs_vnode *vp, fs_size_t pos)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;
  int result;

  result = ext_set_open_mode (vp, FD_MODE_READWRITE);
  if (result < 0)
    return result;

  return mp->ops->truncate (vp->p.named.p.extfs.fd, pos);
}

static int
ext_chmod (struct nameidata *ndp, fs_mode_t mode)
{
  struct fs_vnode *vp = ndp->vp;
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->chmod (vp->p.named.key, mode);
}

static int
ext_utime (struct fs_vnode *vp, const struct fs_utimbuf *times)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->utime (vp->p.named.key, times);
}


static int
ext_get_volume_name (struct fs_vnode *vp, char *label, int len)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->get_volume_name (mp->prefix, label, len);
}

static int
ext_set_volume_name (struct fs_vnode *vp, char *label)
{
  struct fs_mount_extfs *mp = (struct fs_mount_extfs *) vp->mp;

  return mp->ops->set_volume_name (mp->prefix, label);
}

void
fs_extfs_init (void)
{
  int i;

  fs_hfat_init();

  /* Can access fs_drive_in_use without acquiring global lock or unprotecting
   * memory here because efs_initialize has already acquired the global lock */
  for (i = 0; i < FS_HFAT_MAXVOLUME_COUNT; i++)
    fs_drive_in_use[i] = NULL;

  for (i = 0; i < FS_MAX_EXTFS_MOUNTS; i++) {
    extfs_mounts[i].root = NULL;
    extfs_mounts[i].parent.ops = &extfs_ops;

    /* XXX: Make this dynamic. */
    extfs_mounts[i].ops = &fs_hfat_ops;

    fs_mount_register ("extfs", &extfs_mounts[i].parent);
  }
}

#endif /* FEATURE_EFS_EXTFS */
