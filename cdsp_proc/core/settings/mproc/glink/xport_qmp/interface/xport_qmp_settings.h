#ifndef XPORT_QMP_SETTINGS_H
#define XPORT_QMP_SETTINGS_H

/**
 * @file xport_qmp_settings.h
 *
 * Internal definitions for Glink QMP transport configurations
 */

/*==============================================================================
     Copyright (c) 2014-2015, 2017 QUALCOMM Technologies, Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/settings/mproc/glink/xport_qmp/interface/xport_qmp_settings.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/17   vr      Moved device configuration settings to core/settings directory
05/07/15   an      QoS support
06/17/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "com_dtypes.h"
#include "smem_type.h"
#include "DDIIPCInt.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/** Local processor info */
typedef struct _xport_qmp_devcfg_proc_type
{
  const char     *name;                 /* Local host name */
  smem_host_type host;                  /* Local host ID   */
} xport_qmp_devcfg_proc_type;       

/** Remote subsystem configuration */
typedef struct _xport_qmp_devcfg_remote_type
{
  const char *ch_name;                  /* Logical channel name    */
  const char *remote_ss;                /* Remote proc */
  smem_host_type remote_host;           /* Remote host ID          */  
  uint32 mailbox_area_size_bytes;       /* Full Mailbox size       */
  uint32 master_mailbox_size_bytes;     /* Local Mailbox size      */
  uint32 max_tx_pending_items;          /* Tx mailbox size         */
  DalIPCIntProcessorType irq_processor;    /* Remote processor DAL ID        */
  DalIPCIntInterruptType irq_out;          /* Outgoing interrupt             */
  uint32 irq_in;                        /* Incoming interrupt      */
  boolean is_master;                    /* Is it the master of the link */
  uint64 mailbox_desc_start;            /* Mailbox descriptor physical start address */
} xport_qmp_devcfg_remote_type;       


#endif //XPORT_QMP_SETTINGS_H

