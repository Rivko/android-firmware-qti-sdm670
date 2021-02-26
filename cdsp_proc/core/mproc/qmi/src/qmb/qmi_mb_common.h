#ifndef QMI_MB_COMMON_H_
#define QMI_MB_COMMON_H_
/******************************************************************************
  @file    qmi_mb_common.h
  @brief   The QMI Message Bus (QMB) common header file

  DESCRIPTION
  QMI Message Bus common defines
   
  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "qmi_mb.h"
#include "qmi_mb_xport.h"
#include "qmi_mb_os.h"
#include "qmi_common.h"
#include "qmi_idl_lib.h"

/*=============================================================================
     Type definitions
=============================================================================*/

/* Receive buffer description */
typedef struct qmi_mb_recv_s
{
  LINK(struct qmi_mb_recv_s, link);
  qmi_mb_ep_type sender;
  unsigned int   msg_id;
  void           *c_struct;
  unsigned int   c_struct_len;
} qmi_mb_recv_type;

struct qmi_mb_ep_s;
typedef struct qmi_mb_ep_s qmi_mb_ep_handle_type;

/* Endpoint description */
struct qmi_mb_ep_s
{
  LINK(qmi_mb_ep_handle_type, link);
  uint8_t addr[MAX_ADDR_LEN];
  uint32_t uid;
};

struct qmi_mb_struct;
typedef struct qmi_mb_struct qmi_mb_handle_type;

struct qmi_mb_xport_struct;
typedef struct qmi_mb_xport_struct qmi_mb_xport_type;

/* Xport structure for each user/transport */
struct qmi_mb_xport_struct
{
  LINK(qmi_mb_xport_type, link);
  uint32_t xid;
  qmi_mb_xport_ops_type *ops;
  void *xport_data;
  void *handle;
  qmi_mb_handle_type *user;
  uint32_t addr_len;
};

/* Description storing user parameters */
struct qmi_mb_struct
{
  struct
  {
    uint32_t clid;
    qmi_mb_lock_type ref_count_lock;
    int ref_count;
    LINK(qmi_mb_handle_type, link);
  } priv;

  LIST(qmi_mb_xport_type, xports);
  qmi_mb_lock_type xports_lock;

  LIST(struct qmi_mb_recv_s, rx_q);
  LIST(struct qmi_mb_recv_s, free_list);
  qmi_mb_lock_type rx_q_lock;

  qmi_idl_service_object_type service_obj;


  uint32_t service_id;
  uint32_t idl_version;
  uint32_t subgroup;
  uint32_t max_msg_len;

  uint8_t  has_reg;
  uint8_t  raw;
  uint8_t  rx_enabled;

  qmi_mb_msg_cb msg_cb;
  void *msg_cb_data;

  qmi_mb_event_cb event_cb;
  void *event_cb_data;

  qmi_mb_release_cb release_cb;
  void *release_cb_data;
};

#endif

