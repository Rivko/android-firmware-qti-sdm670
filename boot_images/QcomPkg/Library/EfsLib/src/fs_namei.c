/***********************************************************************
 * fs_namei.c
 *
 * Posix interface for EFS2
 * Copyright (C) 2002-2014 QUALCOMM Technologies, Inc.
 *
 * This file includes the functions related to EFS2's path lookup operations
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_namei.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2014-01-28    nr      Fix klocwork issues.
2013-10-09    nr      Replace banned memory functions with safer versions.
2012-03-08    wek     Rename macros and remove header files to compile for boot
2012-01-17    wek     Remove unused utils header file.
2011-11-02    wek     Use library supplied strl cpy and cat instead of fs_strl*
2010-02-06    yog     Validate the input param for NULL check.
2009-10-09    wek     Added define for symlink resolve limit.
2008-04-21    ebb     Fixed bug introduced causing change of path_last.
2008-02-20    ebb     Stripped trailing / before parsing path.
2007-10-01    rp      While resolving symlinks don't discard the old path.
2007-08-18    rp      Removed the inclusion of fs_inode.h
2007-05-25    umr     Added support for EFS memory protection feature.
2007-01-12    dlb     Use safer string functions.
2006-05-10    dlb     Clarify some mountpoint names.
2005-07-13    dlb     Mountpoint cleanup.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2004-07-26    dlb     Allow mountpoints to grab entire name.
2003-12-04    dlb     Corrected symlink leak.
2003-06-17    jkl     Clean up code.
2002-08-20    adm     Created.

===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stringl/stringl.h"

#include "fs_util.h"
#include "fs_namei.h"
#include "fs_mount.h"
#include "fs_fcntl.h"
//#include "fs_protected_memory.h"

#include "assert.h"

static char link_buf[FS_PATH_MAX];

/* Name lookup. */
int
fs_namei (struct nameidata *ndp, const char *path, int flag)
{
  int i;
  int result = -ENOENT;
  const char *namestart, *nameend;
  size_t path_len;
  char *path_last;
  int symlink_count = 0;

  ASSERT (path != NULL);

  /* Because of symlinks, copy the entire path into our working buffer. */
  path_len = strlen (path);
  ASSERT (path_len > 0);
  if (path_len > FS_PATH_MAX)
    return -ENAMETOOLONG;

  strlcpy (ndp->namebuf, path, sizeof (ndp->namebuf));

  path_last = ndp->namebuf + path_len;

  ndp->path = ndp->namebuf;

  ndp->vp = 0;

  /* Remove all trailing slashes in namebuf unless we are at root.
   * Make sure we only change namebuf if we are altering string.  */
  if (path_len > 0)
  {
    for (i = path_len - 1; i > 0; i--)
    {
      if (ndp->namebuf[i] == '/')
      {
        ndp->namebuf[i] = 0;
      }
      else
      {
        break;
      }
    }
    path_last = &(ndp->namebuf[i + 1]);
  }

  /* Is this relative or absolute?  XXX: Change this when we have a
   * current directory that can be different. */
  if (*path == '/')
    ndp->dvp = fs_root_vnode;
  else
    ndp->dvp = fs_root_vnode;

  fs_vnode_ref (ndp->dvp);

  /* Now do the scan of the path itself. */
  namestart = ndp->path;

  while (*namestart == '/')
    namestart++;

  for (;;)
  {
    /* Coming into the loop, namestart points to the beginning of the
     * interesting part of the path. */

    nameend = namestart;

    /* Scan for the end of the name. */
    while (*nameend != 0 && *nameend != '/')
      nameend++;

    if (nameend - namestart > FS_NAME_MAX) {
      result = -ENAMETOOLONG;
      goto cleanup;
    }

    ndp->next = namestart;
    ndp->length = nameend - namestart;

    /* Lookup the next node */
    result = ndp->dvp->vops->lookup (ndp);

    /* The above call is permitted to adjust 'length' */
    nameend = namestart + ndp->length;

    /* The lookup is allowed to fail if this is the last component of the
     * name. */
    if (result != 0 && *nameend == 0)
      break;

    /* Otherwise, a missing entry in the middle is a problem. */
    if (result != 0) {
      /* cleanup will return the result. */
      goto cleanup;
    }

    /* If this entry is a symlink, and we should be following it, resolve
     * the symlink.  This resolution then backs up. */
    if (S_ISLNK (ndp->vp->mode) &&
        !(*nameend == 0 && ((flag & O_NOFOLLOW) != 0)))
    {
      symlink_count++;

      if (symlink_count > FS_SYMLINK_RESOLVE_LIMIT) {
        result = -ELOOP;
        goto cleanup;
      }

      result = ndp->vp->vops->readlink (ndp->vp, link_buf, FS_PATH_MAX);
      if (result < 0)
        goto cleanup;

      /* If the symlink starts with a '/', start over at the root dir. */
      if (link_buf[0] == '/') {
        fs_vnode_unref (ndp->dvp);
        ndp->dvp = fs_root_vnode;
        fs_vnode_ref (ndp->dvp);
        namestart = ndp->path;
      }

      /* compute the new path-len as below:
       *  <- (old-path till symlink) + (symlink) + (old-path after symlink)
       */
      path_len = (namestart - ndp->path) + result + (path_last - nameend);
      if (path_len > FS_PATH_MAX) {
        result = -ENAMETOOLONG;
        goto cleanup;
      }

      /* make space for the symlink, move the remaining chars in the path
       * to the correct place so that we can insert the symlink in between
       */
      fs_memsmove ((char *) namestart + result, path_last - nameend + 1,
                   nameend, path_last - nameend + 1);

      path_last = ndp->namebuf + path_len;

      /* Now insert the symlink at the correct place in the path. */
      fs_memscpy ((char *) namestart, result, link_buf, result);

      /* Skip any initial slashes. */
      while (namestart[0] == '/')
        namestart++;

      result = 0;

      /* Unref the vnode for the symlink. */
      fs_vnode_unref (ndp->vp);
      ndp->vp = 0;

      /* Start from the new name. */
    } else {
      /* Check if we have crossed a mountpoint. */
      for (i = 0; i < FS_MAX_MOUNTS; i++) {
        if (fs_mpoints[i].mount_at_dev == ndp->vp->dev &&
            fs_mpoints[i].mount_at_inode == ndp->vp->inum)
          break;
      }
      if (i < FS_MAX_MOUNTS) {
        /* Mountpoint was hit, so just cross into the new place. */
        fs_vnode_unref (ndp->vp);
        ndp->vp = fs_mpoints[i].root_node;
        fs_vnode_ref (ndp->vp);
      }

      /* If we are done, return now. */
      if (*nameend == 0)
        break;

      fs_vnode_unref (ndp->dvp);
      ndp->dvp = ndp->vp;

      ndp->vp = 0;

      /* Skip over any slashes at separating the components of the path.
       * Making this a while allows multiple slashes to be used without harm.
       */
      while (*nameend == '/')
        nameend++;

      namestart = nameend;
    }
  }

  /* Check for the proper existence operations.  First, is the file
   * forbidden to exist. */
  if ((flag & O_EXCL) != 0 &&
      ndp->vp != NULL)
  {
    result = -EEXIST;
    goto cleanup;
  }

  /* Also check if we are allowed to create the entry. */
  if ((flag & O_CREAT) == 0 &&
      ndp->vp == NULL)
  {
    result = -ENOENT;
    goto cleanup;
  }

  /* Finally, return the valid entry. */
  return 0;

cleanup:
  /* Dereference any nodes we've visited. */
  if (ndp->dvp != NULL) {
    fs_vnode_unref (ndp->dvp);
    ndp->dvp = NULL;
  }
  if (ndp->vp != NULL) {
    fs_vnode_unref (ndp->vp);
    ndp->vp = NULL;
  }

  return result;
}
