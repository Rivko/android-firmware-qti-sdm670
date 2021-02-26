/******************************************************************************
  @file    qcril_qmi_imsa.h
  @brief   qcril qmi - IMS Application

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Application.

  ---------------------------------------------------------------------------

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_QMI_IMSA_H
#define QCRIL_QMI_IMSA_H

#include "qmi_client.h"
#include "qcrili.h"
#include "ip_multimedia_subsystem_application_v01.h"
#include "device_management_service_v01.h"

#define qcril_qmi_imsa_srv_status_type imsa_service_status_ind_msg_v01

void qcril_qmi_imsa_pre_init(void);

void qcril_qmi_imsa_init();

void qcril_qmi_imsa_cleanup();

void qcril_qmi_imsa_set_init_state(boolean inited);

void qcril_qmi_imsa_request_ims_registration_state
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imsa_request_query_ims_srv_status
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imsa_request_get_rtp_statistics(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imsa_set_geo_loc(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imsa_request_get_vowifi_call_quality(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imsa_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
);

void qcril_qmi_imsa_command_cb
(
   qmi_client_type              user_handle,
   unsigned int                 msg_id,
   void                        *resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
);

void qcril_qmi_imsa_unsol_ind_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

void qcril_qmi_imsa_command_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

boolean qcril_qmi_imsa_is_ims_voip_in_service();

qcril_qmi_imsa_srv_status_type qcril_qmi_imsa_get_srv_status();

void qcril_qmi_ims_free_ims_registration(ims_Registration* ims_reg_ptr);

uint8_t qcril_qmi_imsa_ims_registered_wlan_status();
uint8_t qcril_qmi_imsa_compare_ims_registered_report_unsol();
void qcril_qmi_imsa_fetch_ims_reg_status(uint8_t *ims_registered_valid, uint8_t *ims_registered);
uint8_t qcril_qmi_imsa_is_ims_registered_for_voip_vt_service();
void qcril_qmi_imsa_ims_capability_ind_hdlr
(
  dms_ims_capability_ind_msg_v01 * payload
);

void qcril_qmi_imsa_request_get_ims_sub_config
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

void qcril_qmi_imsa_store_ims_capability
(
    uint8_t max_ims_instances_valid,
    uint8_t max_ims_instances,
    uint8_t ims_capability_valid,
    uint32_t ims_capability_len,
    dms_ims_capability_type_v01 ims_capability[]
);

uint8_t copy_qmi_blk_status_to_ims_registration_block_status
(
    imsa_ims_block_status_ind_msg_v01 *src,
    ims_RegistrationBlockStatus *dest
);

#endif
