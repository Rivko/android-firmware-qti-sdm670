/***********************************************************************
 * rfs_errno.h
 *
 * RFS Error codes
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Error codes for Remote File System Module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/api/storage/rfs_errno.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-08-06   dks   Return error for open with encrypt and compress oflags.
2014-01-27   dks   Add support for RFS over RMTS APIs.
2013-11-06   dks   Create

===========================================================================*/

#ifndef __RFS_ERRNO_H__
#define __RFS_ERRNO_H__

#define RFS_ENOERROR         (0)       /**< No error */
#define RFS_ENOENT           (-1)      /**< No such file or directory */
#define RFS_EACCESS          (-2)      /**< Access denied */
#define RFS_ENOMEM           (-3)      /**< Ran out of memory */
#define RFS_EPERM            (-4)      /**< Operation not permitted */
#define RFS_EBADF            (-5)      /**< Bad file descriptor */
#define RFS_EEXISTS          (-6)      /**< Entry exists */
#define RFS_ENODEV           (-7)      /**< No RFS device found. */
#define RFS_EINVAL           (-8)      /**< Invalid argument */
#define RFS_EMFILE           (-9)      /**< Too many open files */
#define RFS_ENOSPC          (-10)      /**< No space left on device */
#define RFS_ENAMETOOLONG    (-11)      /**< File name too long */
#define RFS_EBUSY           (-12)      /**< Resource is busy */
#define RFS_ESYSTEM         (-13)      /**< System Error */
#define RFS_ETIMEOUT        (-14)      /**< Request timed-out  */
#define RFS_ECONFIG         (-15)      /**< Build-config error  */
#define RFS_ENOTSUPPORTED   (-16)      /**< Operation not supported  */


#endif /* not __RFS_ERRNO_H__ */
