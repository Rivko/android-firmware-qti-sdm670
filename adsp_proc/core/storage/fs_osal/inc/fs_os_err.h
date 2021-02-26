/***********************************************************************
 * fs_os_err.h
 *
 * Err implemenation for all FS Modules
 *
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/inc/fs_os_err.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-13   dks   Create

===========================================================================*/

#ifndef __FS_OS_ERR_H__
#define __FS_OS_ERR_H__

#include "fs_os_config_i.h"
#include "assert.h"
#include "err.h"
#include <stringl/stringl.h>

#if defined (__FILENAME__)
  #define FS_OS_SOURCE_FILE_NAME __FILENAME__
#else
  #define FS_OS_SOURCE_FILE_NAME __FILE__
#endif


#define FS_OS_ASSERT(cond)        \
  do                              \
  {                               \
    ASSERT (cond);                \
  } while (0)


#define FS_OS_ASSERT_FATAL(cond)                          \
  do                                                      \
  {                                                       \
    if (!(cond))                                          \
    {                                                     \
      ERR_FATAL ("%d,%s,%d,assert failure[line,file]",    \
                 __LINE__, FS_OS_SOURCE_FILE_NAME,0);     \
    }                                                     \
  } while (0)


#define FS_OS_ERR_FATAL(fmt,a,b,c) \
  do                               \
  {                                \
    ERR_FATAL (fmt, a, b, c);      \
  } while (0)

#endif /* not __FS_OS_ERR_H__ */
