/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_lte_direct_disc.h

  @brief
  Handlers for requests, callbacks and indications for QMI LTE.

*/


#ifndef QCRIL_QMI_LTE_DIRECT_DISC_H
#define QCRIL_QMI_LTE_DIRECT_DISC_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "qcril_qmi_lte_direct_disc_misc.h"
#include "lte_v01.h"
#include "qcril_qmi_lte_direct_disc_dispatcher.h"

/*===========================================================================
                        DEFINITIONS AND TYPES
===========================================================================*/
#define QCRIL_QMI_LTE_DIRECT_DISC_OP_NONE       ((uint32_t) 0)
#define QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH    (((uint32_t) 1) << 0)
#define QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE  (((uint32_t) 1) << 1)

typedef struct qcril_qmi_lte_direct_disc_overview_exp_type
{
    char *expression;
    struct qcril_qmi_lte_direct_disc_overview_exp_type *next;
} qcril_qmi_lte_direct_disc_overview_exp_type;

typedef struct qcril_qmi_lte_direct_disc_exec_overview_type
{
    char                                                *os_app_id;
    qcril_qmi_lte_direct_disc_overview_exp_type         *publish_list;
    qcril_qmi_lte_direct_disc_overview_exp_type         *subscribe_list;
    struct qcril_qmi_lte_direct_disc_exec_overview_type *next;
} qcril_qmi_lte_direct_disc_exec_overview_type;

typedef struct
{
    qcril_qmi_lte_direct_disc_exec_overview_type *exec_overview_root;
} qcril_qmi_lte_direct_disc_overview_type;

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        FUNCTIONS
===========================================================================*/

void qcril_qmi_lte_direct_init
(
  void
);

RIL_Errno qcril_qmi_lte_direct_disc_init
(
 void
);

RIL_Errno qcril_qmi_lte_direct_disc_on_radio_state_change
(
  void
);

void qcril_qmi_lte_direct_disc_command_cb
(
 qmi_client_type              user_handle,
 unsigned int                 msg_id,
 void                        *resp_c_struct,
 unsigned int                 resp_c_struct_len,
 void                        *resp_cb_data,
 qmi_client_error_type        transp_err
);

void qcril_qmi_lte_direct_disc_command_cb_helper
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr // Output parameter
);

void qcril_qmi_lte_direct_disc_unsol_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                          *ind_buf,
 unsigned int                   ind_buf_len,
 void                          *ind_cb_data
);

void qcril_qmi_lte_direct_disc_unsol_ind_cb_helper
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr // Output parameter
);

void qcril_qmi_lte_direct_disc_initialize
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_lte_direct_disc_get_device_capability
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_lte_direct_disc_terminate
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_lte_direct_disc_get_service_status
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_lte_direct_disc_publish
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_lte_direct_disc_cancel_publish
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_lte_direct_disc_subscribe
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_lte_direct_disc_cancel_subscribe
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

RIL_Errno qcril_qmi_lte_direct_disc_set_lted_config_sync
(
   com_qualcomm_qti_radioconfiginterface_LtedConfig *config
);
RIL_Errno qcril_qmi_lte_direct_disc_get_lted_config_sync
(
   com_qualcomm_qti_radioconfiginterface_LtedConfig *config /* Out parameter */
);
RIL_Errno qcril_qmi_lte_direct_disc_set_lted_category_sync
(
   com_qualcomm_qti_radioconfiginterface_LtedCategory *category
);
RIL_Errno qcril_qmi_lte_direct_disc_get_lted_category_sync
(
 char     *osAppId,
 uint32_t *category /* Out parameter */
);

void qcril_qmi_lte_direct_disc_terminate_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);
void qcril_qmi_lte_direct_disc_get_service_status_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);
void qcril_qmi_lte_direct_disc_publish_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);
void qcril_qmi_lte_direct_disc_subscribe_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);
void qcril_qmi_lte_direct_disc_cancel_publish_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);
void qcril_qmi_lte_direct_disc_cancel_subscribe_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);
void qcril_qmi_lte_direct_disc_get_subscription_info_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);

void qcril_qmi_lte_direct_disc_notification_ind_hdlr
(
 void *ind_data_ptr
);
void qcril_qmi_lte_direct_disc_broadcast_notification_ind_hdlr
(
 void *ind_data_ptr
);
void qcril_qmi_lte_direct_disc_match_notification_ind_hdlr
(
 void *ind_data_ptr
);
void qcril_qmi_lte_direct_disc_subscription_info_ind_hdlr
(
 void *ind_data_ptr
);
void qcril_qmi_lte_direct_disc_psk_expired_ind_hdlr
(
 void *ind_data_ptr
);

#ifdef __cplusplus
}
#endif

#endif // QCRIL_QMI_LTE_DIRECT_DISC_H
