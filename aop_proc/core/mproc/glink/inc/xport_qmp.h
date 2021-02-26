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

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/mproc/glink/inc/xport_qmp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/16   dks      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include"glink_os_utils.h"
#include"glink_internal.h"
#include"xport_qmp_config.h"
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
typedef PACKED struct _xport_mailbox_desc_endpoint
{
  uint32 link_state;
  uint32 link_state_ack;
  uint32 ch_state;
  uint32 ch_state_ack;
  uint32 mailbox_size;
  uint32 mailbox_offset;  
}xport_mailbox_desc_endpoint;

/* Mailbox descriptor structure for communication channel */
typedef PACKED struct _xport_mailbox_desc_type
{
  uint32 magic;
  uint32 Core_version; /* Master version */
  uint32 Core_features; /* Master features */
  xport_mailbox_desc_endpoint  mCore_desc; /* Master endpoint descriptor */
  xport_mailbox_desc_endpoint  sCore_desc; /* Slave endpoint descriptor */
  uint8 reserved[128];
} xport_qmp_ch_desc_type;


typedef struct xport_qmp_tx_q_type
{
  glink_packet_ctx tx_pkt_q[XPORT_QMP_TX_Q_SIZE];
  uint32 tx_q_head;
   uint32 tx_q_tail; 
  uint32 count;
} xport_qmp_tx_q;


typedef enum _xport_qmp_local_state xport_qmp_local_state;

/* Channel context descriptor */
typedef struct _xport_qmp_ctx_type
{
  glink_transport_if xport_if;
  const xport_qmp_config_type *pcfg;
  volatile xport_qmp_ch_desc_type* ch_desc;
  volatile xport_mailbox_desc_endpoint* local_ch_desc;
  volatile xport_qmp_mailbox* local_mailbox;
  volatile xport_mailbox_desc_endpoint* remote_ch_desc;
  volatile xport_qmp_mailbox* remote_mailbox;
  uint32 local_mailbox_size;
  uint8 current_state;
  boolean transmitting;
  xport_qmp_tx_q tx_q;
  glink_packet_ctx pkt_ctx;
} xport_qmp_ctx_type;


/*===========================================================================
FUNCTION      xport_qmp_init
===========================================================================*/
/**

  Initializes qmp transport.
  Must be called before any other operations are done.

  @param[in]  arg   The argument.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_qmp_init(void *arg);


/*===========================================================================
FUNCTION      xport_qmp_ch_open
===========================================================================*/
/**

  Sends the open command

  @param[in]  if_ptr   Pointer to transport interface instance.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_qmp_ch_open(glink_transport_if *if_ptr);


/*===========================================================================
FUNCTION      xport_qmp_ch_close
===========================================================================*/
/**

  Sends the close command - expected response is to be delivered 
  via glink_rx_cmd_ch_close_ack().

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  lcid     Local channel ID.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_qmp_ch_close(glink_transport_if *if_ptr);


/*===========================================================================
FUNCTION      xport_qmp_tx
===========================================================================*/
/**

  Send a data packet or a fragment of it.

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  pkt_ctx  Packet context to transfer.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_qmp_tx (glink_transport_if *ctx, 
                                    glink_packet_ctx *pkt_ctx);
                              

/*===========================================================================
FUNCTION      xport_qmp_rx_done
===========================================================================*/
/**

  Send receive-done command.

  @param[in]  if_ptr   Pointer to transport interface instance.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_qmp_rx_done (glink_transport_if *if_ptr, const void *ptr);


#endif //XPORT_QMP_H

