#ifndef XPORT_QMP_CONFIG_H
#define XPORT_QMP_CONFIG_H

/*===========================================================================

            GLink QMP transport Configuration Structures

=============================================================================

  @file
    xport_qmp_config.h

    Contains structures to be used in Glink QMP trasnport configuration.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/mproc/glink/inc/xport_qmp_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   dks      Initial version.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_os_utils.h"
#include "glink_internal.h"

/* Mailbox configuraitons */
typedef struct _xport_qmp_config_type
{
  const char *remote_ss;
  const char *ch_name;                  /* Logical channel name    */
  uint32 mailbox_area_size_bytes;       /* Full Mailbox size       */
  uint32 master_mailbox_size_bytes;     /* Local Mailbox size      */
  glink_os_ipc_intr_type irq_out;       /* Outgoing interrupt      */
  uint32 irq_in;                        /* Incoming interrupt      */
  uint64 mailbox_desc_start;            /* Mailbox descriptopr physical start address */
  boolean clear_intr;                   /* Clear interrupt needed  */
} xport_qmp_config_type;

#ifndef XPORT_QMP_TX_Q_SIZE
  #define XPORT_QMP_TX_Q_SIZE 3
#endif

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      xport_qmp_get_config
===========================================================================*/
/**
  Return the configuration (remote subsystem) of given index.

  @param[in]  ind    Index.

  @return   Configuration.
*/
/*=========================================================================*/
const xport_qmp_config_type* xport_qmp_get_config(uint32 ind);

/*===========================================================================
  FUNCTION      xport_qmp_alloc_mailbox
===========================================================================*/
/**
  Allocates memory for mailbox communication

  @param[in]  ind    Index.

  @return   Mailbox memory
*/
/*=========================================================================*/
void* xport_qmp_alloc_mailbox(uint32 ind);

#endif /* XPORT_QMP_CONFIG_H */


