/***********************************************************************
 * tftp_msg_modem.c
 *
 * Non_HLOS subsystem log module.
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/src/tftp_msg_modem.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-05-31   rp    Port to SLPI image.
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-30   dks   Fixes to config and log module.
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-08-29   dks   Use actual SSID in MSG macros.
2014-07-28   rp    Fix logs to print F3 messages.
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-06-04   rp    Create

===========================================================================*/


#include "tftp_config_i.h"
#include "tftp_comdef.h"
#include "tftp_msg.h"
#include "tftp_assert.h"
#include "tftp_threads.h"


#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

#include "msg.h"

#if defined (TFTP_MPSS_BUILD)
  #define TFTP_DIAG_PROC_NAME "MPSS "
#elif defined (TFTP_ADSP_BUILD)
  #define TFTP_DIAG_PROC_NAME "ADSP "
#elif defined (TFTP_TN_BUILD)
  #define TFTP_DIAG_PROC_NAME "TN "
#elif defined (TFTP_GSS_BUILD)
  #define TFTP_DIAG_PROC_NAME "GSS "
#elif defined (TFTP_SLPI_BUILD)
  #define TFTP_DIAG_PROC_NAME "SLPI "
#else
  #error "TFTP Config Error : Unknown build"
#endif

#define TFTP_MSG(x_ss_mask, x_fmt, a, b)  \
        MSG_SPRINTF_2(MSG_SSID_RFS, x_ss_mask, TFTP_DIAG_PROC_NAME x_fmt, a, b)

void
tftp_msg_init (void)
{
  (void) NULL;
}

void
tftp_msg_print (enum tftp_log_msg_level_type msg_level, const char *msg_str)
{
  tftp_thread_handle tid;

  tid = tftp_thread_self ();

  switch (msg_level)
  {
    case TFTP_LOG_MSG_LEVEL_ERROR:
      TFTP_MSG (MSG_LEGACY_ERROR, "%lu %s", tid, msg_str);
      break;

    case TFTP_LOG_MSG_LEVEL_WARN:
      TFTP_MSG (MSG_LEGACY_HIGH, "%lu %s", tid, msg_str);
      break;

    case TFTP_LOG_MSG_LEVEL_DEBUG:
      TFTP_MSG (MSG_LEGACY_HIGH, "%lu %s", tid, msg_str);
      break;

    case TFTP_LOG_MSG_LEVEL_INFO:
      TFTP_MSG (MSG_LEGACY_HIGH, "%lu %s", tid, msg_str);
      break;

    default:
      TFTP_ASSERT (0);
      break;
  }
}

