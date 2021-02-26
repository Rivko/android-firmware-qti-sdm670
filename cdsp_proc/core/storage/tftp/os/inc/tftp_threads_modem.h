/***********************************************************************
 * tftp_threads_modem.h
 *
 * TFTP Modem Sockets.
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/os/inc/tftp_threads_modem.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2014-12-30   dks   Fixes to config and log module.
2014-07-28   rp    Add api to get the thread-id.
2014-06-04   rp    Create

===========================================================================*/

#ifndef __TFTP_THREADS_MODEM_H__
#define __TFTP_THREADS_MODEM_H__

#include "tftp_config_i.h"
#include "tftp_comdef.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

#include "fs_os.h"

typedef fs_os_thread_t tftp_thread_handle;
typedef fs_os_mutex_t tftp_mutex_handle;
typedef fs_os_thread_return_type tftp_thread_return_type;

#define tftp_thread_return() fs_os_thread_exit()

#endif /* not __TFTP_THREADS_MODEM_H__ */
