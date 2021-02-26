/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/qcril_nas_legacy.h"
#include "modules/nas/NasModule.h"
#include "modules/qmi/NasModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"

#define TAG "QCRIL_NAS_LEGACY"

/*===========================================================================

  FUNCTION:  qmi_client_nas_send_sync

===========================================================================*/
/*!
    @brief
    Wraper for using NAS modem end point module

    @return
    None.
*/
/*=========================================================================*/
qmi_client_error_type
qmi_client_nas_send_sync
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

    rc = ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint()->sendRawSync(msg_id,
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

  FUNCTION:  qmi_client_nas_send_async

===========================================================================*/
/*!
    @brief
    Wraper for using NAS modem end point module

    @return
    None.
*/
/*=========================================================================*/
qmi_client_error_type
qmi_client_nas_send_async
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

    rc = ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint()->sendRawAsync(msg_id,
        req_ptr,
        req_struct_len,
        resp_struct_len,
        resp_cb,
        resp_cb_data,
        getNasModule());

    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

