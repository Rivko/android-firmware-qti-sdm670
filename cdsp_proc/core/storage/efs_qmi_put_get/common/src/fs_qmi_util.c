/***********************************************************************
 * fs_qmi_util.c
 *
 * Utility functions for efs_qmi service and client
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * This file implements utility functions that the efs_qmi service and client
 * can use.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/efs_qmi_put_get/common/src/fs_qmi_util.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-10-04   dks   Create

===========================================================================*/

#include "fs_qmi_std_interface.h"
#include "fs_qmi_util.h"
#include "fs_errno.h"
#include "fs_fcntl.h"

fs_qmi_errno_type
fs_qmi_get_qmi_errno (int efs_err)
{
  switch (efs_err)
  {
    case ENOERR:
      return FS_QMI_ENOERR;
    case EPERM:
      return FS_QMI_EPERM;
    case ENOENT:
      return FS_QMI_ENOENT;
    case EEXIST:
      return FS_QMI_EEXIST;
    case EBADF:
      return FS_QMI_EBADF;
    case ENOMEM:
      return FS_QMI_ENOMEM;
    case EACCES:
      return FS_QMI_EACCES;
    case EBUSY:
      return FS_QMI_EBUSY;
    case EXDEV:
      return FS_QMI_EXDEV;
    case ENODEV:
      return FS_QMI_ENODEV;
    case ENOTDIR:
      return FS_QMI_ENOTDIR;
    case EISDIR:
      return FS_QMI_EISDIR;
    case EINVAL:
      return FS_QMI_EINVAL;
    case EMFILE:
      return FS_QMI_EMFILE;
    case ETXTBSY:
      return FS_QMI_ETXTBSY;
    case ENOSPC:
      return FS_QMI_ENOSPC;
    case ESPIPE:
      return FS_QMI_ESPIPE;
    case ENAMETOOLONG:
      return FS_QMI_ENAMETOOLONG;
    case ENOTEMPTY:
      return FS_QMI_ENOTEMPTY;
    case ELOOP:
      return FS_QMI_ELOOP;
    case EILSEQ:
      return FS_QMI_EILSEQ;
    case ESTALE:
      return FS_QMI_ESTALE;
    case EDQUOT:
      return FS_QMI_EDQUOT;
    case ENOCARD:
      return FS_QMI_ENOCARD;
    case EBADFMT:
      return FS_QMI_EBADFMT;
    case ENOTITM:
      return FS_QMI_ENOTITM;
    case EROLLBACK:
      return FS_QMI_EROLLBACK;
    case FS_ERANGE:
      return FS_QMI_FS_ERANGE;
    case EEOF:
      return FS_QMI_EEOF;
    case EUNKNOWN_SFAT:
      return FS_QMI_EUNKNOWN_SFAT;
    case EUNKNOWN_HFAT:
      return FS_QMI_EUNKNOWN_HFAT;
    default:
      return FS_QMI_ERRNO_TYPE_MAX_ENUM_VAL;
  }
}


int
fs_qmi_get_efs_errno (fs_qmi_errno_type fs_qmi_err)
{
  switch (fs_qmi_err)
  {
    case FS_QMI_ENOERR:
      return ENOERR;
    case FS_QMI_EPERM:
      return EPERM;
    case FS_QMI_ENOENT:
      return ENOENT;
    case FS_QMI_EEXIST:
      return EEXIST;
    case FS_QMI_EBADF:
      return EBADF;
    case FS_QMI_ENOMEM:
      return ENOMEM;
    case FS_QMI_EACCES:
      return EACCES;
    case FS_QMI_EBUSY:
      return EBUSY;
    case FS_QMI_EXDEV:
      return EXDEV;
    case FS_QMI_ENODEV:
      return ENODEV;
    case FS_QMI_ENOTDIR:
      return ENOTDIR;
    case FS_QMI_EISDIR:
      return EISDIR;
    case FS_QMI_EINVAL:
      return EINVAL;
    case FS_QMI_EMFILE:
      return EMFILE;
    case FS_QMI_ETXTBSY:
      return ETXTBSY;
    case FS_QMI_ENOSPC:
      return ENOSPC;
    case FS_QMI_ESPIPE:
      return ESPIPE;
    case FS_QMI_ENAMETOOLONG:
      return ENAMETOOLONG;
    case FS_QMI_ENOTEMPTY:
      return ENOTEMPTY;
    case FS_QMI_ELOOP:
      return ELOOP;
    case FS_QMI_EILSEQ:
      return EILSEQ;
    case FS_QMI_ESTALE:
      return ESTALE;
    case FS_QMI_EDQUOT:
      return EDQUOT;
    case FS_QMI_ENOCARD:
      return ENOCARD;
    case FS_QMI_EBADFMT:
      return EBADFMT;
    case FS_QMI_ENOTITM:
      return ENOTITM;
    case FS_QMI_EROLLBACK:
      return EROLLBACK;
    case FS_QMI_FS_ERANGE:
      return FS_ERANGE;
    case FS_QMI_EEOF:
      return EEOF;
    case FS_QMI_EUNKNOWN_SFAT:
      return EUNKNOWN_SFAT;
    case FS_QMI_EUNKNOWN_HFAT:
      return EUNKNOWN_HFAT;
    default:
      return (int)0xffffffff; /* Unknown */
  }
}


int32
fs_qmi_get_efs_oflag (fs_qmi_oflag_type fs_qmi_oflag)
{
  int32 efs_oflag = 0;

  if ((fs_qmi_oflag & FS_QMI_O_ACCMODE) == FS_QMI_O_ACCMODE)
    efs_oflag |= O_ACCMODE;

  if ((fs_qmi_oflag & FS_QMI_O_RDONLY) == FS_QMI_O_RDONLY)
    efs_oflag |= O_RDONLY;

  if ((fs_qmi_oflag & FS_QMI_O_WRONLY) == FS_QMI_O_WRONLY)
    efs_oflag |= O_WRONLY;

  if ((fs_qmi_oflag & FS_QMI_O_RDWR) == FS_QMI_O_RDWR)
    efs_oflag |= O_RDWR;

  if ((fs_qmi_oflag & FS_QMI_O_CREAT) == FS_QMI_O_CREAT)
    efs_oflag |= O_CREAT;

  if ((fs_qmi_oflag & FS_QMI_O_EXCL) == FS_QMI_O_EXCL)
    efs_oflag |= O_EXCL;

  if ((fs_qmi_oflag & FS_QMI_O_NOCTTY) == FS_QMI_O_NOCTTY)
    efs_oflag |= O_NOCTTY;

  if ((fs_qmi_oflag & FS_QMI_O_TRUNC) == FS_QMI_O_TRUNC)
    efs_oflag |= O_TRUNC;

  if ((fs_qmi_oflag & FS_QMI_O_APPEND) == FS_QMI_O_APPEND)
    efs_oflag |= O_APPEND;

  if ((fs_qmi_oflag & FS_QMI_O_NONBLOCK) == FS_QMI_O_NONBLOCK)
    efs_oflag |= O_NONBLOCK;

  if ((fs_qmi_oflag & FS_QMI_O_SYNC) == FS_QMI_O_SYNC)
    efs_oflag |= O_SYNC;

  if ((fs_qmi_oflag & FS_QMI_FASYNC) == FS_QMI_FASYNC)
    efs_oflag |= FASYNC;

  if ((fs_qmi_oflag & FS_QMI_O_DIRECT) == FS_QMI_O_DIRECT)
    efs_oflag |= O_DIRECT;

  if ((fs_qmi_oflag & FS_QMI_O_LARGEFILE) == FS_QMI_O_LARGEFILE)
    efs_oflag |= O_LARGEFILE;

  if ((fs_qmi_oflag & FS_QMI_O_DIRECTORY) == FS_QMI_O_DIRECTORY)
    efs_oflag |= O_DIRECTORY;

  if ((fs_qmi_oflag & FS_QMI_O_NOFOLLOW) == FS_QMI_O_NOFOLLOW)
    efs_oflag |= O_NOFOLLOW;

  if ((fs_qmi_oflag & FS_QMI_O_ITEMFILE) == FS_QMI_O_ITEMFILE)
    efs_oflag |= O_ITEMFILE;

  if ((fs_qmi_oflag & FS_QMI_O_AUTODIR) == FS_QMI_O_AUTODIR)
    efs_oflag |= O_AUTODIR;

  if ((fs_qmi_oflag & FS_QMI_O_SFS_RSVD) == FS_QMI_O_SFS_RSVD)
    efs_oflag |= O_SFS_RSVD;

  return efs_oflag;
}


fs_qmi_oflag_type
fs_qmi_get_qmi_oflag (int32 efs_oflag)
{
  fs_qmi_oflag_type fs_qmi_oflag = 0;  /* invalid fs_qmi oflag value */

  if ((efs_oflag & O_ACCMODE) == O_ACCMODE)
    fs_qmi_oflag |= FS_QMI_O_ACCMODE;

  if ((efs_oflag & O_RDONLY) == O_RDONLY)
    fs_qmi_oflag |= FS_QMI_O_RDONLY;

  if ((efs_oflag & O_WRONLY) == O_WRONLY)
    fs_qmi_oflag |= FS_QMI_O_WRONLY;

  if ((efs_oflag & O_RDWR) == O_RDWR)
    fs_qmi_oflag |= FS_QMI_O_RDWR;

  if ((efs_oflag & O_CREAT) == O_CREAT)
    fs_qmi_oflag |= FS_QMI_O_CREAT;

  if ((efs_oflag & O_EXCL) == O_EXCL)
    fs_qmi_oflag |= FS_QMI_O_EXCL;

  if ((efs_oflag & O_NOCTTY) == O_NOCTTY)
    fs_qmi_oflag |= FS_QMI_O_NOCTTY;

  if ((efs_oflag & O_TRUNC) == O_TRUNC)
    fs_qmi_oflag |= FS_QMI_O_TRUNC;

  if ((efs_oflag & O_APPEND) == O_APPEND)
    fs_qmi_oflag |= FS_QMI_O_APPEND;

  if ((efs_oflag & O_NONBLOCK) == O_NONBLOCK)
    fs_qmi_oflag |= FS_QMI_O_NONBLOCK;

  if ((efs_oflag & O_SYNC) == O_SYNC)
    fs_qmi_oflag |= FS_QMI_O_SYNC;

  if ((efs_oflag & FASYNC) == FASYNC)
    fs_qmi_oflag |= FS_QMI_FASYNC;

  if ((efs_oflag & O_DIRECT) == O_DIRECT)
    fs_qmi_oflag |= FS_QMI_O_DIRECT;

  if ((efs_oflag & O_LARGEFILE) == O_LARGEFILE)
    fs_qmi_oflag |= FS_QMI_O_LARGEFILE;

  if ((efs_oflag & O_DIRECTORY) == O_DIRECTORY)
    fs_qmi_oflag |= FS_QMI_O_DIRECTORY;

  if ((efs_oflag & O_NOFOLLOW) == O_NOFOLLOW)
    fs_qmi_oflag |= FS_QMI_O_NOFOLLOW;

  if ((efs_oflag & O_ITEMFILE) == O_ITEMFILE)
    fs_qmi_oflag |= FS_QMI_O_ITEMFILE;

  if ((efs_oflag & O_AUTODIR) == O_AUTODIR)
    fs_qmi_oflag |= FS_QMI_O_AUTODIR;

  if ((efs_oflag & O_SFS_RSVD) == O_SFS_RSVD)
    fs_qmi_oflag |= FS_QMI_O_SFS_RSVD;

  return  fs_qmi_oflag;
}
