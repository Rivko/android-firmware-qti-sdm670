/******************************************************************************
#  Copyright (c) 2009-2010, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_reqlist.h
  @brief   qcril qmi - request list

  DESCRIPTION
    reqlist is a general purpose module for the RIL to manage outstanding
    RIL_REQUESTs.

******************************************************************************/


#ifndef QCRIL_REQLIST_H
#define QCRIL_REQLIST_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "IxErrno.h"
#include "telephony/ril.h"
#include <framework/legacy.h>
#include "device_management_service_v01.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_INT     ( 1 << 30 )
#define QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_PTR     ( 1 << 29 )
#define QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE  ( 1 << 28 )

#define QMI_RIL_FW_ANDROID_REQUEST_INFO_SZ_MASK        ( (uint32_t)0xFFFFFFFF & ( ~( QMI_RIL_FW_ANDROID_REQUEST_INFO_NO_PAYLOAD |  \
                                                                                     QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_INT | \
                                                                                     QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_PTR | \
                                                                                     QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE | \
                                                                                     QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK \
                                                                                      ) ) )


#define QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE(desc, sz) ( ((uint32_t)sz & QMI_RIL_FW_ANDROID_REQUEST_INFO_SZ_MASK) | desc )

#define QMI_RIL_FW_ANDROID_REQUEST_INFO_DECOMPOSE_SZ(x) ( (uint32_t)x & QMI_RIL_FW_ANDROID_REQUEST_INFO_SZ_MASK )


#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY                      ( (uint32_t)1 << 31 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE                         ( (uint32_t)1 << 30 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT                    ( (uint32_t)1 << 29 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE                       ( (uint32_t)1 << 28 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING                         ( (uint32_t)1 << 27 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_MULTIPLE_AUTO_DROP_ON_DIFF_PARAMS      ( (uint32_t)1 << 26 )
#define QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK       ( (uint32_t)1 << 25 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF                        ( (uint32_t)1 << 24 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF    ( (uint32_t)1 << 23 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FAMILY_RING_DEFINED_PAIR               ( (uint32_t)1 << 22 )
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF                        ( (uint32_t)1 << 21 )

#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_QUEUE_SZ_MASK        ( (uint32_t)0xFFFFFFFF & \
                                                                              ( ~( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_MULTIPLE_AUTO_DROP_ON_DIFF_PARAMS | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FAMILY_RING_DEFINED_PAIR | \
                                                                                   QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF \
                                                                                   ) ) )


#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ(x)   ( ((uint32_t)x & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_QUEUE_SZ_MASK) )

#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( caps_mask, queue_sz )   ( ((uint32_t)queue_sz & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_QUEUE_SZ_MASK) | caps_mask )

#define QCRIL_REQLIST_RESERVERED_REQ_BUF 16

/* ReqList entry states */
typedef enum
{
  QCRIL_REQ_FREE                      = 0x0001, /*!< Request is not waiting for RPC callback or AMSS events */
  QCRIL_REQ_AWAITING_CALLBACK         = 0x0002, /*!< Request is waiting for RPC callback */
  QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS = 0x0004, /*!< Request is waiting for AMSS event. */
  QCRIL_REQ_COMPLETED_SUCCESS         = 0x0008, /*!< Request completed, success */
  QCRIL_REQ_COMPLETED_FAILURE         = 0x0010  /*!< Request completed, failure */
} qcril_req_state_e_type;

typedef enum
{
  QCRIL_REQ_CONCURRENCY_STATE_MIN = -1,
  QCRIL_REQ_CONCURRENCY_STATE_UNKNOWN = 0,
  QCRIL_REQ_CONCURRENCY_STATE_DISPATCHED,
  QCRIL_REQ_CONCURRENCY_STATE_BLOCKED,
  QCRIL_REQ_CONCURRENCY_STATE_REJECTED,
  QCRIL_REQ_CONCURRENCY_STATE_MAX
} qcril_req_concurrency_state_e_type;

/* For simplicity just use a C union to hold the subsystem specific data
   that is to be kept in the reqlist */

typedef struct
{
  /* SMS subsystem specific data for reqlist */
  int dummy;
} qcril_reqlist_sms_type;

typedef struct
{
  /* DATA SERVICE subsystem specific data for reqlist */
  void *info;
} qcril_reqlist_ds_type;

typedef struct
{
  /* UIM subsystem specific data for reqlist */
  int dummy;
} qcril_reqlist_uim_type;

typedef struct
{
  /* Other subsystem specific data for reqlist */
  int dummy;
} qcril_reqlist_other_type;

typedef struct
{
  union
  {
      RIL_SelectUiccSub                select_uicc_sub;
      dms_operating_mode_enum_v01      required_operating_mode;
  };
} qcril_reqlist_nas_type;

typedef union
{
  qcril_reqlist_sms_type sms;
  qcril_reqlist_ds_type ds;
  qcril_reqlist_uim_type uim;
  qcril_reqlist_other_type other;
  qcril_reqlist_nas_type nas;
} qcril_reqlist_u_type;

struct qcril_reqlist_buf_tag;

typedef void (*qcril_reqlist_dispatch_blocked_req_handler)
(
  qcril_instance_id_e_type instance_id,
  struct qcril_reqlist_buf_tag *req,
  void *data,
  size_t datalen
);

typedef struct qcril_reqlist_public_struct
{
  uint16 req_id;                                            /* Reqlist entry ID */
  RIL_Token t;                                              /* Token */
  uint32_t t_id;                                              /* Token ID */
  qcril_evt_e_type request;                                 /* RIL Request or QCRIL Event */
  boolean valid_sub_id;                                     /* Indicates whether sub id is valid */
  uint32 sub_id;                                            /* Subsystem ID (e.g Call ID, SS Ref, Invoke ID, Supplementary Service Type
                                                              or event ID) */
  qcril_req_state_e_type state[ QCRIL_MAX_MODEM_NUM ];      /* State of ReqList Entry */
  qcril_req_concurrency_state_e_type con_state;             /* Concurrency state of ReqList Entry */
  qcril_evt_e_type pending_event_id[ QCRIL_MAX_MODEM_NUM ];   /* Pending Event ID */
  qcril_reqlist_u_type sub;                                 /* Union of subsystem specific information */
  qcril_reqlist_dispatch_blocked_req_handler handler;       /* Handler for dispatch this request if it was blocked before */
  size_t req_datalen;                                       /* Size of additional data */
  void* req_data;                                           /* Additional data needed for handler execution */
  boolean allocated_on_heap;                                /* indicate whether this request is allocated on heap */
  int handler_invoked_under_mtx_lock;
  int auto_respond_duplicate;
} qcril_reqlist_public_type;

typedef struct qcril_reqlist_buf_tag
{
  qcril_reqlist_public_type pub;
  struct qcril_reqlist_buf_tag *prev_ptr;
  struct qcril_reqlist_buf_tag *next_ptr;
  struct qcril_reqlist_buf_tag *following_ptr;        /* points to request who follows this request */
  struct qcril_reqlist_buf_tag *followed_ptr;         /* points to request which this request follows */
} qcril_reqlist_buf_type;

typedef struct
{
  qcril_reqlist_buf_type *head_ptr;
  qcril_reqlist_buf_type *tail_ptr;
} qcril_reqlist_struct_type;

typedef qcril_req_concurrency_state_e_type (*qcril_reqlist_check_concurrency_handler)
(
  qcril_instance_id_e_type instance_id,
  qcril_reqlist_public_type* req,
  void *data,
  size_t datalen,
  qcril_reqlist_buf_type **followed_req
);

typedef struct
{
  uint16 max_concurrency;
  uint16 max_pending;
  uint16 req_ids_num;
  qcril_evt_e_type *req_ids;
} qcril_reqlist_generic_concurency_requests_requirement_type;

typedef struct
{
   qmi_ril_atomic_lock_info_type                            guard;
   qmi_ril_fw_android_request_kind_execution_overview_type* android_request_kind_info;
   int                                                      in_exec_on_main_thread;
   uint32_t                                                 common_moniker_book;
} qmi_ril_fw_android_request_flow_control_overview_type;

extern qmi_ril_fw_android_request_flow_control_overview_type qmi_ril_fw_android_request_flow_control_overview;

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


void qcril_reqlist_init( void );

void qcril_reqlist_default_entry( RIL_Token t, qcril_evt_e_type request, qcril_modem_id_e_type modem_id, qcril_req_state_e_type state,
                                  qcril_evt_e_type pending_event_id, qcril_reqlist_u_type *sub_ptr,
                                  qcril_reqlist_public_type *req_ptr );

IxErrnoType qcril_reqlist_new( qcril_instance_id_e_type instance_id, qcril_reqlist_public_type *entry_ptr );

IxErrnoType qcril_reqlist_new_with_concurency_control
(
  qcril_instance_id_e_type instance_id,
  qcril_reqlist_public_type *entry_ptr,
  qcril_reqlist_check_concurrency_handler check_dispatchable_handler,
  void *check_dispatchable_handler_data,
  size_t check_dispatchable_handler_datalen,
  qcril_reqlist_dispatch_blocked_req_handler dispatch_blocked_req_handler,
  void *req_data,
  size_t req_datalen,
  qcril_reqlist_buf_type **req_buf
);

IxErrnoType qcril_reqlist_free( qcril_instance_id_e_type instance_id, RIL_Token t );

IxErrnoType qcril_reqlist_free_deferred ( qcril_instance_id_e_type instance_id, RIL_Token t, int32_t token_id );

boolean qcril_reqlist_has_follower( qcril_instance_id_e_type instance_id, RIL_Token t );

int qcril_reqlist_under_follower_handler_exec( qcril_instance_id_e_type instance_id, RIL_Token t );

RIL_Token qcril_reqlist_get_follower_token( qcril_instance_id_e_type instance_id, RIL_Token t );

int qcril_reqlist_is_auto_respond_duplicate( qcril_instance_id_e_type instance_id, RIL_Token t );

void qcril_reqlist_set_auto_respond_duplicate( qcril_instance_id_e_type instance_id, RIL_Token t );

void qcril_free_request_and_dispatch_follower_request_cb(qcril_timed_callback_handler_params_type * handler_params);

IxErrnoType qcril_reqlist_free_and_dispatch_follower_req
(
  RIL_Token t,
  int32_t token_id,
  qcril_instance_id_e_type instance_id,
  void *data,
  size_t datalen
);

qcril_reqlist_buf_type* qcril_reqlist_get_root_followed_req( qcril_reqlist_buf_type* req_buf );

IxErrnoType qcril_reqlist_update_state( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id, RIL_Token t,
                                        qcril_req_state_e_type state );

IxErrnoType qcril_reqlist_update_pending_event_id( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id,
                                                   RIL_Token t, qcril_evt_e_type pending_event_id );

IxErrnoType qcril_reqlist_update_sub_id( qcril_instance_id_e_type instance_id, RIL_Token t, uint32 sub_id );

IxErrnoType qcril_reqlist_query( qcril_instance_id_e_type instance_id, RIL_Token t, qcril_reqlist_public_type *info_ptr );
IxErrnoType qcril_reqlist_query_possibly_freed_token( qcril_instance_id_e_type instance_id, RIL_Token t, uint32_t token_id, qcril_reqlist_public_type *info_ptr );

IxErrnoType qcril_reqlist_query_by_request( qcril_instance_id_e_type instance_id, qcril_evt_e_type request,  qcril_reqlist_public_type *info_ptr );

IxErrnoType qcril_reqlist_get_entry_from_group ( qcril_instance_id_e_type instance_id, qcril_evt_group grp,
                                                         qcril_reqlist_public_type *req_info );

IxErrnoType qcril_reqlist_query_by_req_id( uint16 req_id, qcril_instance_id_e_type *instance_id_ptr,
                                           qcril_reqlist_public_type *info_ptr );

IxErrnoType qcril_reqlist_query_by_event( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id,
                                          qcril_evt_e_type pending_event_id, qcril_reqlist_public_type *info_ptr );

IxErrnoType qcril_reqlist_query_by_event_and_sub_id( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id,
                                                     qcril_evt_e_type pending_event_id, uint32 sub_id, qcril_reqlist_public_type *info_ptr );

IxErrnoType qcril_reqlist_query_by_event_all_states( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id,
                                                     qcril_evt_e_type pending_event_id, qcril_reqlist_public_type *info_ptr );

IxErrnoType qcril_reqlist_query_by_event_and_sub_id_all_states( qcril_instance_id_e_type instance_id,
                                                                qcril_modem_id_e_type modem_id, qcril_evt_e_type pending_event_id,
                                                                uint32 sub_id, qcril_reqlist_public_type *info_ptr );

IxErrnoType qcril_reqlist_update_sub_info( qcril_instance_id_e_type instance_id, RIL_Token t, qcril_reqlist_u_type *sub_ptr );

IxErrnoType qcril_reqlist_complete_all_amss_events( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id, RIL_Token t,
                                                    qcril_req_state_e_type state, qcril_modem_ids_list_type *modem_ids_done_list,
                                                    IxErrnoType *result );

void qcril_reqlist_reply_blocked_req_with_exsting_result
(
  qcril_instance_id_e_type instance_id,
  qcril_reqlist_buf_type *req,
  void *data,
  size_t datalen
);

qcril_req_concurrency_state_e_type qcril_reqlist_generic_check_concurrency_from_set_of_requests
(
  qcril_instance_id_e_type instance_id,
  qcril_reqlist_public_type *req,
  void *data,
  size_t datalen,
  qcril_reqlist_buf_type **followed_req
);

const char *qcril_reqlist_lookup_state_name
(
   qcril_req_state_e_type state
);
void qcril_free_request_list_entry_deferred( qcril_timed_callback_handler_params_type * handler_params );


#ifdef __cplusplus
}
#endif

#endif /* QCRIL_REQLIST_H */
