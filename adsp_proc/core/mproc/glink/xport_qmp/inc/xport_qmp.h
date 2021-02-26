#ifndef XPORT_QMP_H
#define XPORT_QMP_H

/**
 * @file xport_qmp.h
 *
 * Internal definitions for QMP Glink transport
 */

/*==============================================================================
     Copyright (c) 2016 Qualcomm Technologies Inc.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/glink/xport_qmp/inc/xport_qmp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   dks      Initial version.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink_core_if.h"
#include "glink_transport_if.h"
#include "xport_qmp_config.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

/* Mailbox structure */
typedef PACKED struct _xport_qmp_mailbox_type
{
  uint32 msg_len;
  uint8 msg[];
} xport_qmp_mailbox;


/* Mailbox descriptor structure for communication end point */
typedef PACKED struct _xport_mailbox_endpoint
{
  uint32 link_state;
  uint32 link_state_ack;
  uint32 ch_state;
  uint32 ch_state_ack;
  uint32 mailbox_size;
  uint32 mailbox_offset;  
}xport_mailbox_endpoint;

/* Mailbox descriptor structure for the 2 end points */
typedef PACKED struct _xport_mailbox_desc_type
{
  uint32 magic;
  uint32 core_version; /* Master version */
  uint32 core_features; /* Master features */
  xport_mailbox_endpoint  mCore_desc; /* Master endpoint descriptor */
  xport_mailbox_endpoint  sCore_desc; /* Slave endpoint descriptor */
  uint8 reserved[128];
} xport_qmp_ch_desc_type;


/* Xport transport context */
typedef struct _xport_qmp_ctx_type
{
  /* context structure should start from trasport interface */
  glink_transport_if_type xport_if;
  const xport_qmp_config_type *pcfg;
  volatile xport_qmp_ch_desc_type* ch_desc;
  volatile xport_mailbox_endpoint* local_ch_desc;
  volatile xport_qmp_mailbox* local_mailbox;
  volatile xport_mailbox_endpoint* remote_ch_desc;
  volatile xport_qmp_mailbox* remote_mailbox;
  uint32 local_mailbox_size;
  uint8 current_state;

  os_cs_type *tx_link_lock;
  os_cs_type *rx_link_lock;
  os_cs_type *tx_list_lock;
  uint32 tx_pending_count;

  boolean transmitting;
  boolean receiving;
  uint32 lcid;
  uint32 free_iid;
  uint32 current_in_transmit_intent_iid;
  glink_core_version_type negotiated_version;
} xport_qmp_ctx_type;


#endif //XPORT_QMP_H

