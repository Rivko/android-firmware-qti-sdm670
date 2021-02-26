/***********************************************************************
 * tftp_os_modem.h
 *
 * NON_HLOS build related OS defines
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/os/inc/tftp_os_modem.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-06-05   rp    Change uid/gid for RAM-dump files.
2015-05-06   dks   Add OS abstraction layer
2015-01-05   dks   Compile server for TN Apps
2014-12-30   dks   Fixes to config and log module.
2014-06-04   rp    Create

===========================================================================*/

#ifndef __TFTP_OS_MODEM_H__
#define __TFTP_OS_MODEM_H__


#include "tftp_config_i.h"
#include "tftp_comdef.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

#include "fs_os.h"
#include "fs_errno.h"
#include "fs_fcntl.h"

#define MAIN_TYPE

#define TFTP_SHARED_GID  (-1)
#define TFTP_RAMDUMP_GID (-1)
#define TFTP_RAMDUMP_UID (-1)

#define OS_PATH_SEPARATOR '/'
#define INVALID_OS_PATH_SEPARATOR '\\'

#define OS_BAD_PATH_STRING1 "/../"
#define OS_BAD_PATH_STRING2 "/.."
#define OS_BAD_PATH_STRING3 "\\"


#endif /* not __TFTP_OS_MODEM_H__ */
