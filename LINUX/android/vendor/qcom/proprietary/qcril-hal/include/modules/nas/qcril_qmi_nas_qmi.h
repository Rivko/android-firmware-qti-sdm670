/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <memory>
#include <qmi_client.h>
#include "modules/qmi/QmiStruct.h"

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
    unsigned int       resp_c_struct_len
);

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
    std::shared_ptr<void>           req_ptr,
    int                             req_struct_len,
    int                             resp_struct_len,
    qmiAsyncCbType                  resp_cb,
    void                            *resp_cb_data
);

