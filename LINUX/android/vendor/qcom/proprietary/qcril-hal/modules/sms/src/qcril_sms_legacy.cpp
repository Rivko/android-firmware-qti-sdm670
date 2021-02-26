/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qcril_sms_legacy.h"
#include "SmsModule.h"
#include "modules/qmi/SmsModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"

#define TAG "QCRIL_SMS_LEGACY"

/*===========================================================================

  FUNCTION:  qmi_client_sms_send_sync

===========================================================================*/
/*!
    @brief
    Wraper for using SMS modem end point module

    @return
    None.
*/
/*=========================================================================*/
qmi_client_error_type
qmi_client_sms_send_sync
(
    unsigned int       msg_id,
    void               *req_c_struct,
    int                req_c_struct_len,
    void               *resp_c_struct,
    unsigned int       resp_c_struct_len
)
{
    return qmi_client_sms_send_sync(msg_id,
                                    req_c_struct,
                                    req_c_struct_len,
                                    resp_c_struct,
                                    resp_c_struct_len,
                                    ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
}

qmi_client_error_type
qmi_client_sms_send_sync
(
    unsigned int       msg_id,
    void               *req_c_struct,
    int                req_c_struct_len,
    void               *resp_c_struct,
    unsigned int       resp_c_struct_len,
    int                timeout_value
)
{
    qmi_client_error_type rc = QMI_INTERNAL_ERR;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_DEBUG("%s", __FUNCTION__);

    rc = ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint()->sendRawSync(msg_id,
        req_c_struct,
        req_c_struct_len,
        resp_c_struct,
        resp_c_struct_len,
        timeout_value);

    QCRIL_LOG_DEBUG("transport err - %d", rc);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*===========================================================================

  FUNCTION:  qmi_client_sms_send_async

===========================================================================*/
/*!
    @brief
    Wraper for using SMS modem end point module

    @return
    None.
*/
/*=========================================================================*/
qmi_client_error_type
qmi_client_sms_send_async
(
    unsigned long                   msg_id,
    void                            *req_ptr,
    int                             req_struct_len,
    int                             resp_struct_len,
    qmiAsyncCbType                  resp_cb,
    void                            *resp_cb_data
)
{
    qmi_client_error_type rc = QMI_INTERNAL_ERR;

    QCRIL_LOG_FUNC_ENTRY();

    rc = ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint()->sendRawAsync(msg_id,
        req_ptr,
        req_struct_len,
        resp_struct_len,
        resp_cb,
        resp_cb_data,
        getSmsModule());

    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

