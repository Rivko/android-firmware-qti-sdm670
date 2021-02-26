/***********************************************************************
 * rfs_fcntl.h
 *
 * Posix File control options for RFS APIs
 * Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/api/storage/rfs_fcntl.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2013-11-07   dks   Create

===========================================================================*/

#ifndef __RFS_FCNTL_H__
#define __RFS_FCNTL_H__

#define RFS_O_RDONLY       0x00000
#define RFS_O_WRONLY       0x00001
#define RFS_O_RDWR         0x00002
#define RFS_O_ACCMODE      0x00003

#define RFS_O_APPEND       0x00008
#define RFS_O_CREAT        0x00200
#define RFS_O_TRUNC        0x00400

#define RFS_O_ENCRYPT      0x08000
#define RFS_O_COMPRESS     0x10000

#define RFS_SEEK_SET       0       /**< Seek from beginning of file.  */
#define RFS_SEEK_CUR       1       /**< Seek from current position.  */
#define RFS_SEEK_END       2       /**< Seek from end of file.  */

#endif /* not __RFS_FCNTL_H__ */
