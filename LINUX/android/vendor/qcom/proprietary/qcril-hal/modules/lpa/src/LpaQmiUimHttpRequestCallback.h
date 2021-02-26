/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "modules/lpa/LpaUimHttpSrvc.h"

class LpaQmiUimHttpRequestCallback : public GenericCallback<lpa_qmi_uim_http_rsp_data_type>
{
 public:
  inline LpaQmiUimHttpRequestCallback(string str) : GenericCallback(str) {}
  inline ~LpaQmiUimHttpRequestCallback() {}
  Message::Callback *clone();
  void onResponse(std::shared_ptr<Message>                        solicitedMsg,
                  Message::Callback::Status                       status,
                  std::shared_ptr<lpa_qmi_uim_http_rsp_data_type> responseDataPtr);
  string mClientToken;
};
