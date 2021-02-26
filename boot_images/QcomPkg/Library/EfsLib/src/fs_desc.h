/***********************************************************************
 * fs_desc.h
 *
 * File Descriptor structure declaration and operations.
 * Copyright (C) 2002-2007, 2009-2010, 2013 QUALCOMM Technologies, Inc.
 *
 * This file includes functions declarations for file descriptor operations
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-05-24    wek     Make file descriptor ops constant.
2010-03-19    yog     Added support for files having max filesize upto 4GB.
2009-11-23    wek     Change funciton signature to fit its purpose.
2009-03-27    ebb     Removed #include "fs_buffer.h".
2007-08-18    rp      Removed the inclusion of fs_inode.h
2006-01-03    dlb     Make initialization explicit.
2005-12-20    dlb     Descriptor table initialization.
2005-08-09    dlb     Add nodev support.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2003-06-17    jkl     Clean up code.
2002-08-27    drh     Created by adm.  Added history header.

===========================================================================*/

#ifndef __FS_DESC_H__
#define __FS_DESC_H__

#include "comdef.h"
#include "fs_sys_types.h"
#include "fs_errno.h"
#include "fs_vnode.h"
#include "fs_mount.h"

/* A single descriptor contains the following information. */

#define FS_DESC_STATE_CLOSED    0       /* This descriptor is closed. */
#define FS_DESC_STATE_FILE      1       /* This descriptor referrs to a
                                           file. */
struct fs_descriptor;

struct desc_ops {
  int (*close) (struct fs_descriptor *file);
  fs_ssize_t (*write) (struct fs_descriptor *file, const void *buf,
      fs_size_t count);
  fs_ssize_t (*read) (struct fs_descriptor *file, void *buf,
      fs_size_t count);
};

struct fs_descriptor {
  int fd;
  int state;
  uint8 desc_type;              /* To indicate if it is a file
                                   or socket (to be implemented later ) */
  int mode;                     /* What mode is this opened in. */

  /* XXX: Make this a union for socket support. */
  fs_size_t file_pos;            /* Current file position. */
  struct fs_vnode *vp;

  const struct desc_ops *dops;
};

/* Lookup the information concerning a descriptor.  Returns a null pointer
 * if this descriptor is not currently open. */
struct fs_descriptor *fs_desc_lookup (int fd);

/* Allocate a new descriptor in preparation for opening.  Returns NULL if
 * there are no descriptors available. */
struct fs_descriptor *fs_desc_alloc (void);

/* Mark a descriptor as free. */
void fs_desc_free (struct fs_descriptor *file);

/* Scan through all open descriptors, close (cleanup) and mark as "nodev"
 * the referenced vnodes.  This will cause all future operations (other
 * than close) to fail with ENODEV.  This is used when forcing an umount
 * of a filesystem. */
void fs_desc_close_and_nodev (struct fs_mount *mp);

/* Initialize the descriptor table. */
void fs_desc_init (void);

#endif /* End of __FS_DESC_H__ */
