/******************************************************************************
#  Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_imsa.h
  @brief   qcril qmi - IMS Application

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Application.

******************************************************************************/

#ifndef QCRIL_QMI_IMSA_H
#define QCRIL_QMI_IMSA_H

#include "qmi_client.h"
#include "qcrili.h"
#include "ip_multimedia_subsystem_application_v01.h"
#include "device_management_service_v01.h"
#include <modules/ims/imsIF.pb.h>

#ifdef __cplusplus
#include "modules/ims/ImsServiceStatusInd.h"
#include "modules/ims/ImsWfcSettingsStatusInd.h"
#endif

#define qcril_qmi_imsa_srv_status_type imsa_service_status_ind_msg_v01

typedef struct
{
   pthread_mutex_t       imsa_info_lock_mutex;
   boolean inited;
   boolean ims_status_change_registered;
   uint8_t ims_registered_valid;
   uint8_t ims_registered;
   uint8_t ims_registration_error_code_valid;
   uint16_t ims_registration_error_code;
   uint8_t ims_registration_error_string_valid;
   char *ims_registration_error_string;
   uint8_t ims_srv_status_valid;
   qcril_qmi_imsa_srv_status_type ims_srv_status;
   uint8_t registration_network_valid;
   uint8_t registration_network;
   uint8_t uri_list_valid;
   uint32_t uri_list_len;
   p_associated_uri_v01 URI_List[IMSA_MAX_NUM_URIS_V01];
   uint32_t ims_capability_len;
   dms_ims_capability_type_v01 ims_capability[QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01];
   uint8_t max_ims_instances_valid;
   uint8_t max_ims_instances;
} qcril_qmi_imsa_info_type;

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

#ifdef __cplusplus
void qcril_qmi_imsa_command_cb
(
   unsigned int                 msg_id,
   std::shared_ptr<void>        resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
);
#endif

void qcril_qmi_imsa_unsol_ind_cb_helper
(
 unsigned int   msg_id,
 unsigned char *decoded_payload,
 uint32_t       decoded_payload_len
);

void qcril_qmi_imsa_ims_capability_ind_hdlr
(
  dms_ims_capability_ind_msg_v01 * payload
);

void qcril_qmi_imsa_request_get_ims_sub_config
(
  const qcril_request_params_type *const params_ptr,
  QCRIL_UNUSED(qcril_request_return_type *const ret_ptr) // Output parameter
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

void qcril_qmi_imsa_broadcast_service_status
(
 qcril_qmi_imsa_info_type qcril_qmi_imsa_info
);

#endif
