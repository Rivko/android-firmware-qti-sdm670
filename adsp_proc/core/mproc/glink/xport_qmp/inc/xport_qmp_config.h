#ifndef XPORT_QMP_CONFIG_H
#define XPORT_QMP_CONFIG_H

/**
 * @file xport_qmp_config.h
 *
 * Internal definitions for QMP Glink transport configurations
 */

/*==============================================================================
     Copyright (c) 2016 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/glink/xport_qmp/inc/xport_qmp_config.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   dks      Initial version.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_os_utils.h"
#include "smem_type.h"
#include "smem_list.h"
#include "glink_transport_if.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
typedef void (*xport_qmp_clear_interrupt_fn_type)(void);


typedef struct _xport_qmp_config_type
{
  char *ch_name;                  /* Logical channel name    */
  char *remote_ss;                /* Remote proc */
  smem_host_type remote_host;           /* Remote host ID          */  
  uint32 mailbox_area_size_bytes;       /* Full Mailbox size       */
  uint32 master_mailbox_size_bytes;     /* Local Mailbox size      */
  uint32 max_tx_pending_items;          /* Tx mailbox size         */
  os_ipc_intr_type irq_out;             /* Outgoing interrupt      */
  uint32 irq_in;                        /* Incoming interrupt      */
  
  boolean is_master;                    /* Is it the master of the link */
  uint64 mailbox_desc_start;            /* Mailbox descriptor physical start address */
  xport_qmp_clear_interrupt_fn_type clear_interrupt; /* Function to clear interrupt */  
} xport_qmp_config_type;


/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      xport_qmp_config_init
===========================================================================*/
/**
  This function initializes the configuration. Returns the this processor  
  host id and number of configurations (number of remote subsystems).

  @param[out]  this_host    This processor host id.

  @return   Number of configurations (number of remote subsystems).
*/
/*=========================================================================*/
uint32 xport_qmp_config_init(smem_host_type *this_host);

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
  Allocates the mailbox memory.

  @param[in]  ind    Index.
  @param[out] size   Size of the mailbox.

  @return   Mailbox memory.
*/
/*=========================================================================*/
void *xport_qmp_alloc_mailbox(uint32 ind, uint32* size);

#endif //XPORT_QMP_CONFIG_H


