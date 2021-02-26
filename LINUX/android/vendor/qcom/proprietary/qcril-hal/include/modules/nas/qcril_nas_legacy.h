/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <memory>
#include <qmi_client.h>
#include "framework/Log.h"
#include "modules/nas/NasUimHeaders.h"
#include "modules/qmi/QmiStruct.h"
#include "modules/qmi/NasModemEndPoint.h"

typedef struct
{
    qcril_provision_info_type provision_info;
    int                       repost_event_id;
} qcril_nas_dsds_activate_repost_params;

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
    void                            *req_ptr,
    int                             req_struct_len,
    int                             resp_struct_len,
    qmiAsyncCbType                  resp_cb,
    void                            *resp_cb_data
);


/*=========================================================================
  FUNCTION:  qcril_qmi_nas_minority_command_cb

===========================================================================*/
void qcril_qmi_nas_minority_command_cb_ex
(
    unsigned int                   msg_id,
    std::shared_ptr<void>          resp_c_struct,
    unsigned int                   resp_c_struct_len,
    void                           *resp_cb_data,
    qmi_client_error_type          transp_err
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_update_card_status

===========================================================================*/
void qcril_qmi_nas_dms_update_card_status
(
  qcril_instance_id_e_type instance_id,
  uint8 slot,
  boolean pwr_oprt_in_progress,
  qcril_card_status_e_type card_status
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_conv_nas_mode_pref_to_qcril

===========================================================================*/
qcril_subs_mode_pref qcril_qmi_nas_conv_nas_mode_pref_to_qcril
(
    uint8_t info_valid,
    uint16_t qmi_nas_mode_pref,
    RIL_SelectUiccSub* master_request_ref
);


