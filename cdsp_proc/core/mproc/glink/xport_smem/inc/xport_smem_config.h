#ifndef XPORT_SMEM_CONFIG_H
#define XPORT_SMEM_CONFIG_H

/**
 * @file xport_smem_config.h
 *
 * Internal definitions for SMEM Glink transport configurations
 */

/*==============================================================================
     Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/xport_smem/inc/xport_smem_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/15   an      QoS support
06/17/14   an      Initial version
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

typedef boolean (*xport_smem_alloc_fifo_fn_type)(void *ctx_ptr);
typedef void (*xport_smem_clear_interrupt_fn_type)(void);

#ifdef MPROC_MODEM_PROC
boolean xport_smem_alloc_spss_fifo(void *ctx_ptr);
void xport_smem_notify_spss_ctrl_info
(
  uint32 ctrl_info_addr,
  uint32 ctrl_info_size
);
void xport_smem_clear_spss_interrupt(void);
#endif /* MPROC_MODEM_PROC */

typedef struct _xport_smem_config_type
{
  const char* remote_ss;                /* Remote host name        */
  smem_host_type remote_host;           /* Remote host ID          */
  uint32 fifo_size;                     /* Tx FIFO size            */
  uint32 mtu_size;                      /* Tx MTU size             */
  os_ipc_intr_type irq_out;             /* Outgoing interrupt      */
  uint32 irq_in;                        /* Incoming interrupt      */
  glink_transport_qos_if_type *qos_api; /* QoS interface           */
  xport_smem_alloc_fifo_fn_type alloc_fifo; /* Function to allocate FIFO */
  xport_smem_clear_interrupt_fn_type clear_interrupt; /* Function to clear interrupt */
} xport_smem_config_type;       

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      xport_smem_config_init
===========================================================================*/
/**
  This function initializes the configuration. Returns the this processor  
  host id and number of configurations (number of remote subsystems).

  @param[out]  this_host    This processor host id.

  @return   Number of configurations (number of remote subsystems).
*/
/*=========================================================================*/
uint32 xport_smem_config_init(smem_host_type *this_host);

/*===========================================================================
  FUNCTION      xport_smem_get_config
===========================================================================*/
/**
  Return the configuration (remote subsystem) of given index.

  @param[in]  ind    Index.

  @return   Configuration.
*/
/*=========================================================================*/
const xport_smem_config_type* xport_smem_get_config(uint32 ind);

#endif //XPORT_SMEM_CONFIG_H

