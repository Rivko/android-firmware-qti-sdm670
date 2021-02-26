/**********************************************************************
 * fs_extfs.h
 *
 * EFS2
 * Copyright (C) 2004-2006,2008-2010,2013 QUALCOMM Technologies, Inc.
 *
 * External filesystem mountpoint handler.
 *
 ***********************************************************************/

/*======================================================================
 *
 * EDIT HISTORY
 *
 *
 * when        who  what, where, why
 * ----------  ---  ----------------------------------------------------
 * 2013-05-24  wek  Make mountpoint ops as constant.
 * 2010-03-19  yog  Added support for files having max filesize upto 4GB.
 * 2009-12-18  vr   Added set_volume_name and get_volume_name APIs.
 * 2009-08-11  wek  Remove hfat drive number coupled to hotplug dev index.
 * 2009-04-03  wek  Added utime ops to set access/mod time.
 * 2008-01-24  sch  Add getsize vnode operation
 * 2006-11-10  sh   Add chmod support for FAT READONLY attribute
 * 2006-09-12  sh   Add helpful feature flag checks here.
 * 2006-07-07  dlb  Implement truncate.
 * 2006-07-06  dlb  Remove 'size' field from vnode.
 * 2006-06-27  dlb  Pass stat through readdir.
 * 2005-10-11  dlb  Pass name through to stop.
 * 2005-08-11  dlb  Add umount support.
 * 2005-07-19  dlb  Mountpoint cleanup.
 * 2005-06-06  dlb  Extensions for SFAT.
 * 2004-06-25  dlb  Created
 *=====================================================================*/

#ifndef __FS_EXTFS_H__
#define __FS_EXTFS_H__

#include "comdef.h"
#include "fs_mount.h"
#include "fs_vnode.h"

/* Make sure the target has specified which FAT library to use */
#if defined FEATURE_EFS_EXTFS \
    && !defined FEATURE_EFS_EXTFS_SFAT \
    && !defined FEATURE_EFS_EXTFS_HFAT
#  error "One FAT library must be enabled: FEATURE_EFS_EXTFS_HFAT or _SFAT"
#endif

#if defined FEATURE_EFS_EXTFS_SFAT && defined FEATURE_EFS_EXTFS_HFAT
#  error "SFAT and HFAT are mutually exclusive.  Chose only one."
#endif

#if !defined FEATURE_EFS_EXTFS && \
    (defined FEATURE_EFS_EXTFS_SFAT || defined FEATURE_EFS_EXTFS_HFAT)
#  error "Both SFAT and HFAT require FEATURE_EFS_EXTFS to be enabled."
#endif

/* Filesystem specific operations. */
struct fs_extfs_ops {
  /* Initialize the filesystem. */
  int (*start) (const char *name, const void *hdev);

  /* Stop the filesystem.  The name passed in can be used to resolve the
   * FS to stop. */
  int (*stop) (const char *name);

  /* Determine basic file type/size info.  Used when first looking up
   * entries to determine which operations are to be permitted. */
  int (*base_stat) (const char *name, fs_mode_t *mode);

  /* Get full stat info.  st_dev field should not be filled in, and st_inum
   * can be left alone. */
  int (*getstat) (const char *name, struct fs_stat *buf);

  /* Get the size info for the file in bytes. This function can fail if the
   * underlying file system does not support getsize */
  int (*getsize) (int fd, uint32 *numbytes);

  /* Normal file operations.  open can only be called with the following
   * modes O_RDONLY, O_RDWR, or O_CREAT|O_EXCL|O_RDWR.  The file should
   * always be created with reasonable permissions (0644 is good).
   *
   * Also, lseek returns a status (0 is normal) not the current position.
   * It always seeks to an absolute position. */

  int (*open) (const char *name, int mode);
  int (*lseek) (int fd, fs_size_t pos, int mode);
  int (*read) (int fd, void *buf, fs_size_t count);
  int (*write) (int fd, const void *buf, fs_size_t count);

  /* Change the file attributes */
  int (*chmod) (const char *name, fs_mode_t mode);

  /* Close the open handle. */
  int (*close) (int fd);

  /* Directory operations. */
  int (*mkdir) (const char *name);
  int (*rmdir) (const char *name);
  int (*unlink) (const char *name);
  int (*rename) (const char *oldname, const char *newname);

  /* Filesystem information. */
  int (*statvfs) (const char *name, struct fs_statvfs *buf);

  /* Directory operations.  Special case, opendir doesn't return integer,
   * but a pointer.  Also, readdir should return -EEOF to indicate it is
   * done. */
  void * (*opendir) (const char *name);
  int (*readdir) (void *dir, struct fs_dirent *dirent);
  int (*closedir) (void *dir);

  /* Truncate the given descriptor to the specified position. */
  int (*truncate) (int fd, fs_size_t pos);

  /* Change file access and modification time. */
  int (*utime) (const char *path, const struct fs_utimbuf *times);

  /* Gets the volume name */
  int (*get_volume_name) (char *name, char *label, int len);

  /* Sets the volume name */
  int (*set_volume_name) (const char *name, char *label);
};

struct fs_mount_extfs {
  struct fs_mount  parent;
  struct fs_vnode *root;
  const struct fs_extfs_ops *ops;
  char             prefix[FS_PATH_MAX+1];
};

extern const struct fs_mount_ops extfs_ops;

void fs_extfs_init (void);

#endif /* not __FS_EXTFS_H__ */
