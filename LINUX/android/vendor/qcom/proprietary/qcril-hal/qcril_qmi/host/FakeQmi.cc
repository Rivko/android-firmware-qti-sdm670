/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#ifdef HOST_EXECUTABLE_BUILD
#ifdef __cplusplus
#include <stdlib.h>
extern "C" {
#endif

#include "../inc/qmi_client.h"
#include "ril.h"

qmi_client_error_type qmi_client_send_msg_sync(
    qmi_client_type    user_handle,
    unsigned int       msg_id,
    void               *req_c_struct,
    unsigned int       req_c_struct_len,
    void               *resp_c_struct,
    unsigned int       resp_c_struct_len,
    unsigned int       timeout_msecs
) {
    return QMI_NO_ERR;
}

qmi_client_error_type qmi_client_send_msg_async
(
    qmi_client_type                 user_handle,
    unsigned int                    msg_id,
    void                            *req_c_struct,
    unsigned int                    req_c_struct_len,
    void                            *resp_c_struct,
    unsigned int                    resp_c_struct_len,
    qmi_client_recv_msg_async_cb    resp_cb,
    void                            *resp_cb_data,
    qmi_txn_handle                  *txn_handle
) {
    return QMI_NO_ERR;
}

qmi_client_error_type qmi_client_release
(
  qmi_client_type     user_handle
) {
    return QMI_NO_ERR;
}

int32_t qmi_idl_get_message_c_struct_len
(
  const qmi_idl_service_object_type p_service,
  qmi_idl_type_of_message_type message_type,
  uint16_t message_id,
  uint32_t *c_struct_len
) {
  return QMI_NO_ERR;
}

qmi_client_error_type
    qmi_client_notifier_init
    (
        qmi_idl_service_object_type               service_obj,
        qmi_client_os_params                      *os_params,
        qmi_client_type                           *user_handle
    ) {
    return QMI_NO_ERR;
}

qmi_client_error_type qmi_client_register_notify_cb
(
    qmi_client_type user_handle,
    qmi_client_notify_cb notify_cb,
    void *notify_cb_data
) {
    notify_cb(user_handle, nullptr, QMI_CLIENT_SERVICE_COUNT_INC, notify_cb_data);
    return QMI_NO_ERR;
}

qmi_client_error_type
qmi_client_message_decode
(
 qmi_client_type                         user_handle,
 qmi_idl_type_of_message_type            req_resp_ind,
 unsigned int                            message_id,
 const void                              *p_src,
 unsigned int                            src_len,
 void                                    *p_dst,
 unsigned int                            dst_len
 ) {
    return QMI_NO_ERR;
}

int32_t qmi_idl_get_max_message_len
(
  const qmi_idl_service_object_type p_service,
  qmi_idl_type_of_message_type message_type,
  uint16_t message_id,
  uint32_t *message_len
) {
    return QMI_NO_ERR;
}

int32_t qmi_idl_message_encode
(
  /* Pointer to service object, from auto-generated header from IDL */
  const qmi_idl_service_object_type p_service,

  /* See enumeration above */
  qmi_idl_type_of_message_type req_resp_ind,

  /* Message ID from IDL */
  uint16_t message_id,

  /* Pointer to C structure containing message data */
  const void *p_src,

  /* Length of above buffer */
  uint32_t src_len,

  /* Pointer to buffer for encoded message data */
  void *p_dst,

  /* Length of above buffer */
  uint32_t dst_len,

  /* Length of the encoded message */
  uint32_t *dst_encoded_len
) {
    return QMI_NO_ERR;
}

int32_t qmi_idl_message_decode
(
  /* Pointer to service object, from auto-generated header from IDL */
  const qmi_idl_service_object_type p_service,

  /* See enumeration above */
  qmi_idl_type_of_message_type req_resp_ind,

  /* Message ID from IDL */
  uint16_t message_id,

  /* Pointer to BEGINNING OF FIRST TLV IN MESSAGE */
  const void *p_src,

  /* Length field from QMI header */
  uint32_t src_len,

  /* Pointer to C structure for decoded data */
  void *p_dst,

  /* Size of above structure, for sanity check */
  uint32_t dst_len
) {
    return QMI_NO_ERR;
}

qmi_client_error_type
qmi_client_get_service_instance
(
 qmi_idl_service_object_type service_obj,
 qmi_service_instance instance_id,
 qmi_service_info *service_info
 )
{
    return QMI_NO_ERR;
}

const char* qmi_errstr(int error_code) {
    return "(0) QMI_NO_ERR";
}

const char* qmisvc_errstr(int error_code) {
    return "QMI_ERR_NONE";
}

qmi_client_error_type qmi_client_register_error_cb
(
 qmi_client_type user_handle,
 qmi_client_error_cb err_cb,
 void *err_cb_data
 )
{
    return QMI_NO_ERR;
}

qmi_client_error_type qmi_client_init_instance
(
    qmi_idl_service_object_type service_obj,
    qmi_service_instance        instance_id,
    qmi_client_ind_cb           ind_cb,
    void                        *ind_cb_data,
    qmi_client_os_params        *os_params,
    uint32_t                    timeout,
    qmi_client_type             *user_handle
) {
    qmi_client_type dummy = (qmi_client_type)malloc(sizeof(qmi_client_type));
    *user_handle = dummy;
    return QMI_NO_ERR;
}

int check_system_health(void)
{ return 0; }

int
acquire_wake_lock(int lock, const char* id)
{
    return 0;
}

int
release_wake_lock(const char* id)
{
    return 0;
}

unsigned long long time_get_from_timetick()
{
    return 0;
}

#define CREATE_DUMMY_SERVICE_OBJECT(service) qmi_idl_service_object_type service \
    (int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version) { \
        qmi_idl_service_object_type dummy = (qmi_idl_service_object_type)malloc(sizeof(qmi_idl_service_object_type)); \
        return dummy; \
    } \

CREATE_DUMMY_SERVICE_OBJECT(dms_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(voice_get_service_object_internal_v02)
CREATE_DUMMY_SERVICE_OBJECT(cat_get_service_object_internal_v02)
CREATE_DUMMY_SERVICE_OBJECT(uim_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(uim_remote_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(ims_qmi_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(imsp_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(Qtuner_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(pbm_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(nas_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(wms_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(wds_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(sar_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(rfrpe_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(lte_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(pdc_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(cxm_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(common_qmi_idl_type_table_object_v01)
CREATE_DUMMY_SERVICE_OBJECT(uim_http_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(imsa_get_service_object_internal_v01)
CREATE_DUMMY_SERVICE_OBJECT(imss_get_service_object_internal_v01)

#ifdef __cplusplus
}
#endif
#endif
#endif