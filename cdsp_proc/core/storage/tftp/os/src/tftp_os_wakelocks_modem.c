/***********************************************************************
 * tftp_os_wakelocks_modem.c
 *
 * Modem wakelock abstraction.
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/os/src/tftp_os_wakelocks_modem.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-01-06   vm    Create.

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_os_wakelocks.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

void
tftp_os_wakelocks_init(void)
{
}

void
tftp_os_wakelock(void)
{
}

void
tftp_os_wakeunlock(void)
{
}

