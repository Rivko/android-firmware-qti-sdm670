/***********************************************************************
 * fs_namei.h
 *
 * Nameidata structure declaration and operations.
 * Copyright (C) 2002-2004, 2007 QUALCOMM Technologies, Inc.
 *
 * This file declares the functions related to EFS2's path lookup operations
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2007-10-01    rp      Removed the inclusion of fs_db.h
2004-10-15    dlb     Update copyright line.
    ...
2002-08-20    adm     Created.

===========================================================================*/

#ifndef _FS_NAMEI_H
#define _FS_NAMEI_H

#include "comdef.h"
#include "fs_errno.h"
#include "fs_sys_types.h"
#include "fs_vnode.h"

/* The namei structure contains information about pathnames as they are
 * traversed.  The 'path' field points to the original base of the path.
 * The 'next' field points to the beginning of the particular component we
 * are interested in.  'len' is the number of characters in the component
 * name (up to the '/' or end of the string).
 *
 * dvp, and vp are a vnode pair used to walk down the directory.  dvp is
 * the vnode of the directory we are interested in looking in, can be
 * thought of as the parent.  The vp is the result node from the lookup.
 *
 * All of the necessary fields are filled in by fs_namei when the lookup
 * happens.
 */

struct nameidata {
  const char *path;             /* Pathname pointer. */
  const char *next;             /* Next character in path. */

  int length;                   /* Length of this component of the name. */

  struct fs_vnode *dvp;            /* Intermediate directory. */
  struct fs_vnode *vp;             /* Vnode of result. */

  char namebuf[FS_PATH_MAX+1];    /* Copy of the filename. */
};

/* Lookup a single component.  The results will be returned in 'ndp'.
 * 'ndp->vp' will be set to the vnode for the result, or NULL if the last
 * component of the path does not exist.  'ndp->dvp' will be set to the
 * parent directory of this component.  Each will have an increased
 * reference count (on successful return).
 *
 * 'flag' can be used to determine if the last component of the path need
 * exist.  It should be a combination of O_EXCL and O_CREAT or'd together,
 * having the following meanings.
 *
 * 0 - the file must exist.
 * O_EXCL - not sensical, errors whether the file exists or not.
 * O_CREAT - the file need not exist.
 * O_EXCL | O_CREAT - The file must not exist.
 *
 * Returns 0 or a negative errno indicating the error condition.
 */
int fs_namei (struct nameidata *ndp, const char *path, int flag);

#endif /* End of _FS_NAMEI_H */
