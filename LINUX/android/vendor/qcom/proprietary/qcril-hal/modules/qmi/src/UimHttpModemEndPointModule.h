/******************************************************************************
#  @file    UimModemEndPointModule.h
#  @brief   QMI UIM HTTP module class header file
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#pragma once

#include "qmi_client.h"

#include "modules/qmi/ModemEndPoint.h"
#include "modules/lpa/LpaQmiUimHttpIndicationMsg.h"
#include "modules/lpa/LpaUimHttpRequestMsg.h"

class UimHttpModemEndPointModule : public Module
{
  public:
    UimHttpModemEndPointModule(string name, ModemEndPoint* owner);
    ~UimHttpModemEndPointModule();

    void onQmiUnsolIndCb(qmi_client_type user_handle, unsigned long msg_id,
                         unsigned char* ind_buf, int ind_buf_len);

    void onQmiNotifyCb(qmi_client_type user_handle,
                       qmi_idl_service_object_type service_obj,
                       qmi_client_notify_event_type service_event);

    void qcril_uim_http_transaction_completed_callback(qmi_client_type user_handle,
                                                       unsigned int msg_id,
                                                       void  * qmi_http_rsp_ptr,
                                                       unsigned int  qmi_http_rsp_len,
                                                       qmi_client_error_type transp_err);
#ifdef QMI_RIL_UTF
    void cleanUpQmiSvcClient();
#endif

  private:
    uim_http_transaction_ind_msg                * http_trasaction_ind_ptr;
    std::shared_ptr<LpaUimHttpRequestMsg>         http_transaction_req_ptr;
    qmi_idl_service_object_type                   mServiceObject;
    qmi_client_type                               mQmiSvcClient;
    qmi_client_type                               mQmiNotifyHandle;
    qmi_client_os_params                          mNotifierOsParams;
    qmi_client_os_params                          mOsParams;
    std::vector<std::shared_ptr<Message>>         mMessageList;
    ModemEndPoint* mOwner;

    void handleQmiClientSetup(std::shared_ptr<Message> msg);

    void handleQmiServiceUp(std::shared_ptr<Message> msg);

    void handleQmiServiceDown(std::shared_ptr<Message> msg);

    void qcril_uim_http_send_error_response(uint32_t token_id);

    void qcril_uim_http_client_transaction_request(std::shared_ptr<Message> msg_ptr);

    void qcril_uim_http_handle_transaction_ind(std::shared_ptr<Message> msg_ptr);

    void qcril_uim_http_process_transaction_completed_qmi_callback(std::shared_ptr<Message> msg_ptr);

    uint8_t  * qcril_uim_http_copy_indication
    (
      qmi_client_type                user_handle_ptr,
      unsigned int                   msg_id,
      unsigned char                * qmi_http_ind_ptr,
      unsigned int                   qmi_http_ind_len,
      uint32_t                     * out_len_ptr
    );
};
