/***********************************************************************
 * rfs_desc.c
 *
 * File descriptor management for rfs
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.

 * File descriptor management for rfs
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/rfs/src/rfs_desc.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2014-10-15   dks   Create

===========================================================================*/

#include "comdef.h"
#include "rfs_config_i.h"
#include "rfs_desc.h"
#include "fs_os.h"
#include "rfs_errno.h"
#include "rfs_assert.h"
#include "fs_os_malloc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>

#define RFS_PATH_BUFF_SIZE (RFS_PATH_MAX + 1)

struct rfs_desc
{
  int used;
  char *filename;
  uint32 offset;
  int oflag;
};

static struct rfs_desc rfs_fd [RFS_MAX_FILE_DESCRIPTORS];

/* Critical section for accessing the FD table */
static fs_os_mutex_t rfs_desc_cs;

static void
rfs_desc_lock_init (void)
{
  memset (&rfs_desc_cs, 0, sizeof (rfs_desc_cs));
  fs_os_mutex_init (&rfs_desc_cs);
}

static void
rfs_desc_lock (void)
{
  fs_os_mutex_lock (&rfs_desc_cs);
}

static void
rfs_desc_unlock (void)
{
  fs_os_mutex_unlock (&rfs_desc_cs);
}

int
rfs_desc_allocate_fd (const char *path, int oflag)
{
  int i = RFS_ESYSTEM;

  rfs_desc_lock ();

  if (strlen(path) > RFS_PATH_MAX)
  {
    i = RFS_ENAMETOOLONG;
    goto End;
  }

  for (i = 0; i < RFS_MAX_FILE_DESCRIPTORS; i++)
  {
    if (rfs_fd[i].used == 0)
    {
      break;
    }
  }

  if (i < RFS_MAX_FILE_DESCRIPTORS)
  {
    rfs_fd[i].used = 1;
    rfs_fd[i].offset  = 0;
    rfs_fd[i].filename = (char *) fs_os_malloc (RFS_PATH_BUFF_SIZE);
    if (rfs_fd[i].filename == NULL)
    {
      i = RFS_ENOMEM;
      goto End;
    }
    strlcpy (rfs_fd[i].filename, path, RFS_PATH_BUFF_SIZE);
    rfs_fd[i].oflag = oflag;
  }
  else
  {
    i = RFS_EMFILE;
  }

End:
  rfs_desc_unlock ();

  return i;
}

int
rfs_desc_free_fd (int fd)
{
  int result = RFS_ESYSTEM;

  rfs_desc_lock ();

  if (fd < 0 || fd >= RFS_MAX_FILE_DESCRIPTORS ||
      rfs_fd[fd].used != 1)
  {
    result = RFS_EBADF;
    goto End;
  }

  fs_os_free (rfs_fd[fd].filename);
  memset (&rfs_fd[fd], 0x0, sizeof (rfs_fd[fd]));
  result = RFS_ENOERROR;

End:

  rfs_desc_unlock ();
  return result;
}

int
rfs_desc_lookup_fd (int fd, char *fname, uint32 fname_len, int *oflag,
                    uint32 *offset)
{
  int result = RFS_ESYSTEM;

  rfs_desc_lock ();

  if (fname == NULL || fname_len < 2 || oflag == NULL || offset == NULL)
  {
    goto End;
  }

  memset (fname, 0x0, fname_len);

  if (fd < 0 || fd >= RFS_MAX_FILE_DESCRIPTORS ||
      rfs_fd[fd].used != 1)
  {
    result = RFS_EBADF;
    goto End;
  }

  if ((strlen(rfs_fd[fd].filename) + 1) > fname_len)
  {
    result = RFS_ENAMETOOLONG;
    goto End;
  }

  strlcpy (fname, rfs_fd[fd].filename, fname_len);
  *offset = rfs_fd[fd].offset;
  *oflag = rfs_fd[fd].oflag;

  result = RFS_ENOERROR;

End:
  rfs_desc_unlock ();
  return result;
}

int
rfs_desc_update_offset (int fd, uint32 offset)
{
  int result = RFS_ESYSTEM;

  rfs_desc_lock ();


  if (fd < 0 || fd >= RFS_MAX_FILE_DESCRIPTORS ||
      rfs_fd[fd].used != 1)
  {
    result = RFS_EBADF;
    goto End;
  }

  rfs_fd[fd].offset = offset;
  result = RFS_ENOERROR;

End:
  rfs_desc_unlock ();
  return result;
}


void
rfs_desc_init (void)
{
  rfs_desc_lock_init ();
}
