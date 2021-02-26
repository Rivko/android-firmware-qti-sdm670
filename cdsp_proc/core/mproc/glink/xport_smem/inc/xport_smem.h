#ifndef XPORT_SMEM_H
#define XPORT_SMEM_H

/**
 * @file xport_smem.h
 *
 * Internal definitions for SMEM Glink transport
 */

/*==============================================================================
     Copyright (c) 2014 - 2015 Qualcomm Technologies Inc.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/xport_smem/inc/xport_smem.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/15   bc      Secure processor support
02/23/15   bc      Added tracer packet command
12/09/14   db      Modified channel cmd type to accomodate xport priority in command
06/18/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink_core_if.h"
#include "glink_transport_if.h"
#include "xport_smem_config.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

typedef enum {
  /* Version request command */
  XPORT_SMEM_CMD_VERSION_REQ,
  /* Version ACK command */
  XPORT_SMEM_CMD_VERSION_ACK,
  /* Open channel command */
  XPORT_SMEM_CMD_OPEN_CHANNEL,
  /* Close channel command */
  XPORT_SMEM_CMD_CLOSE_CHANNEL,
  /* Open channel ACK command */
  XPORT_SMEM_CMD_OPEN_CHANNEL_ACK,
  /* Rx intent command */
  XPORT_SMEM_CMD_RX_INTENT,
  /* Rx done command */
  XPORT_SMEM_CMD_RX_DONE,
  /* Intent request command */
  XPORT_SMEM_CMD_INTENT_REQ,
  /* Intent ACK command */
  XPORT_SMEM_CMD_INTENT_ACK,
  /* Tx data command */
  XPORT_SMEM_CMD_TX_DATA,
  /* Tx data command */
  XPORT_SMEM_CMD_TX_DATA_ZERO_COPY,
  /* Close channel ACK command */
  XPORT_SMEM_CMD_CLOSE_CHANNEL_ACK,
  /* Tx data continued command */
  XPORT_SMEM_CMD_TX_DATA_CONT,
  /* Read notification command */
  XPORT_SMEM_CMD_READ_REQ,
  /* Rx done with intent reuse command */
  XPORT_SMEM_CMD_RX_DONE_REUSE,
  /* Tx signals command */
  XPORT_SMEM_CMD_TX_SIGNALS,
  /* Tracer packet */
  XPORT_SMEM_CMD_TRACER_PACKET,
  /* Tracer packet continued */
  XPORT_SMEM_CMD_TRACER_PACKET_CONT
} xport_smem_cmd_id_type;

typedef struct _xport_smem_generic_cmd_type
{
  uint16 id;
  uint16 arg0;
  uint16 arg1;
  uint16 arg2;
} xport_smem_generic_cmd_type;

typedef struct _xport_smem_version_cmd_type
{
  uint16 id;
  uint16 version;
  uint32 features;
} xport_smem_version_cmd_type;

typedef struct _xport_smem_channel_cmd_type
{
  uint16 id;
  uint16 cid;
  uint16 name_len;
  uint16 prio;  
} xport_smem_channel_cmd_type;

typedef struct _xport_smem_rx_intent_cmd_type
{
  uint16 id;
  uint16 cid;
  uint32 intent_num;
} xport_smem_rx_intent_cmd_type;

typedef struct _xport_smem_rx_done_cmd_type
{
  uint16 id;
  uint16 cid;
  uint32 iid;
} xport_smem_rx_done_cmd_type;

typedef struct _xport_smem_intent_req_cmd_type
{
  uint16 id;
  uint16 cid;
  uint32 size;
} xport_smem_intent_req_cmd_type;

typedef struct _xport_smem_intent_ack_cmd_type
{
  uint16 id;
  uint16 cid;
  uint32 ack;
} xport_smem_intent_ack_cmd_type;

typedef struct _xport_smem_tx_cmd_type
{
  uint16 id;
  uint16 cid;
  uint32 iid;
} xport_smem_tx_cmd_type;

typedef struct _xport_smem_tx_sigs_cmd_type
{
  uint16 id;
  uint16 cid;
  uint32 sigs;
} xport_smem_sigs_cmd_type;

typedef struct _xport_smem_rx_intent_type
{
  uint32 size;
  uint32 id;
} xport_smem_rx_intent_type;

typedef struct _xport_smem_tx_data_type
{
  uint32 fragment_size;
  uint32 remaining_size;
} xport_smem_tx_data_type;

typedef union _xport_smem_cmd_type
{
  uint64 raw64;
  xport_smem_generic_cmd_type gen_cmd;
  xport_smem_version_cmd_type ver_cmd;
  xport_smem_channel_cmd_type ch_cmd;
  xport_smem_rx_intent_cmd_type rx_int_cmd;
  xport_smem_rx_done_cmd_type rx_done_cmd;
  xport_smem_intent_req_cmd_type int_req_cmd;
  xport_smem_intent_ack_cmd_type int_ack_cmd;
  xport_smem_tx_cmd_type tx_cmd;
  xport_smem_sigs_cmd_type sigs_cmd;
  xport_smem_rx_intent_type int_desc;
  xport_smem_tx_data_type data_desc;
} xport_smem_cmd_type;

typedef struct _xport_smem_cmd_item {
  struct _xport_smem_cmd_item* next;
  xport_smem_cmd_type cmd;
  uint32 attachment_size;
  void* pattachment;
} xport_smem_cmd_item_type;

/* SMEM channel descriptor */
typedef struct _xport_smem_ind_type
{
  uint32 read_ind;
  uint32 write_ind;
} xport_smem_ind_type;

typedef struct _xport_smem_ch_desc_type
{
  xport_smem_ind_type shared_ind[2];
  uint32 reserved[4];
} xport_smem_ch_desc_type;

typedef struct smem_mb_ctrl_info
{
  xport_smem_ind_type fifo_0_ind;
  uint32              fifo_0_size;
  xport_smem_ind_type fifo_1_ind;
  uint32              fifo_1_size;
  char                fifo[];
}smem_mb_ctrl_info_type;

/* SMEM transport context */
typedef struct _xport_smem_ctx_type
{
  /* context structure should start from trasport interface */
  glink_transport_if_type xport_if; 
  const xport_smem_config_type *pcfg;
  smem_mb_ctrl_info_type       *spss_ctrl_info;
  volatile xport_smem_ch_desc_type* ch_desc;
  volatile xport_smem_ind_type* tx_desc;
  volatile xport_smem_ind_type* rx_desc;
  char* tx_fifo;
  char* rx_fifo;
  uint32 tx_fifo_size;
  uint32 rx_fifo_size;
  boolean rx_fifo_cached;  
  uint32 mtu_size;
  xport_smem_ind_type local_ind;
  os_cs_type *tx_link_lock;
  os_cs_type *rx_link_lock;
  os_cs_type *tx_list_lock;
  smem_list_type tx_list;
  os_cs_type *tx_free_list_lock;
  smem_list_type tx_free_list;
  boolean resume_pending;
  
  glink_core_version_type negotiated_version;
} xport_smem_ctx_type;


#endif //XPORT_SMEM_H

