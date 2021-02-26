/***********************************************************************
 * fs_desc.c
 *
 * Descriptor management for EFS2
 * Copyright (C) 2002-2007, 2009-2010 QUALCOMM Technologies, Inc.
 *
 * This file includes the functions related to file descriptor
 *
 ***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_desc.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2010-02-08    rp      CMI compliance, remove #ifdef from API files.
2009-11-23    wek     Cleanup the file descriptors before making them nodev.
2009-04-22    ebb     Added memory protection.
2007-08-19     rp     Use FS_MAX_DESCRIPTORS from fs_config.h
2006-01-03    dlb     Make initialization explicit.
2005-10-26     sh     Lint cleanup.
2005-02-07    dlb     Increase number of file descriptors.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2003-06-17    jkl     Clean up code.
2002-08-20    adm     Created the file

===========================================================================*/

#include "fs_config_i.h"
#include "fs_desc.h"

static struct fs_descriptor fd_table[FS_MAX_DESCRIPTORS];
static int fds_used = 0;

/***********************************************************************
FUNCTION      fs_desc_lookup

DESCRIPTION   This function looks up the descriptor table and returns
              the fs_descriptor structure corresponding the file
              descriptor id if it is valid.

RETURN VALUE  fs_descriptor pointer if successful else  NULL
**********************************************************************/
struct fs_descriptor *
fs_desc_lookup (int fd)
{
  if (fd < 0 || fd >= FS_MAX_DESCRIPTORS ||
      fd_table[fd].state == FS_DESC_STATE_CLOSED)
    return NULL;
  else
    return &fd_table[fd];
}

/***********************************************************************
FUNCTION      fs_desc_alloc

DESCRIPTION   This function allocates a new file descriptor

RETURN VALUE
**********************************************************************/
struct fs_descriptor *
fs_desc_alloc (void)
{
  int i;

  for (i = 0; i < fds_used; i++)
  {
    if (fd_table[i].state == FS_DESC_STATE_CLOSED)
    {
      /* XXX: Hardcoded, need to figure out how to configure this. */
      /* Why not let whoever you return this to set the value??? */
      fd_table[i].state = FS_DESC_STATE_FILE;
      return &fd_table[i];
    }
  }

  /* If none of the available descriptors can be used, see if there are any
   * remaining descriptors. */
  if (fds_used >= FS_MAX_DESCRIPTORS)
  {
    return NULL;
  }

  i = fds_used;
  fds_used++;

  fd_table[i].state = FS_DESC_STATE_FILE;
  fd_table[i].fd = i;
  fd_table[i].dops = NULL;

  return &fd_table[i];
}

/***********************************************************************
FUNCTION      fs_desc_free

DESCRIPTION

RETURN VALUE
**********************************************************************/
void
fs_desc_free (struct fs_descriptor *file)
{
  file->state = FS_DESC_STATE_CLOSED;
}

/* Cleanup and "Nodev" the vnodes of any descriptors that are still opened.
 * Although it is possible to have multiple descriptors pointing to a
 * given vnode, only the first will modify it.  The rest will not see
 * themselves as pointing to this mpoint any more. */
void
fs_desc_close_and_nodev (struct fs_mount *mp)
{
  int i;
  struct fs_descriptor *file;

  for (i = 0; i < FS_MAX_DESCRIPTORS; i++) {
    file = &fd_table[i];
    if (file->state == FS_DESC_STATE_FILE &&
        file->vp != NULL &&
        file->vp->mp == mp)
    {
      if (file->vp->mp->ops->cleanup != NULL)
        file->vp->mp->ops->cleanup (file->vp->mp, file->vp);

      fs_vnode_make_nodev (file->vp);
    }
  }
}

/* Initialization. */
void
fs_desc_init (void)
{
  fds_used = 0;
}
