/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#ifdef __cplusplus
#include <memory>
#endif

#include <qmi_client.h>
#include "framework/Log.h"
#include "modules/qmi/QmiStruct.h"

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
);

qmi_client_error_type
qmi_client_sms_send_sync
(
    unsigned int       msg_id,
    void               *req_c_struct,
    int                req_c_struct_len,
    void               *resp_c_struct,
    unsigned int       resp_c_struct_len,
    int                timeout_value
);

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
);

