/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "modules/uim/qcril_gstk_srvc.h"
#include "GstkQmiCatResponseMsg.h"

class GstkQmiCatRequestCallback : public GenericCallback<qmi_cat_rsp_data_type> {
 public:
  string mClientToken;
  inline GstkQmiCatRequestCallback(string str) : GenericCallback(str) {}
  inline ~GstkQmiCatRequestCallback() {}
  inline Message::Callback *clone()
  {
    return new GstkQmiCatRequestCallback(mClientToken);
  }
  inline void onResponse(std::shared_ptr<Message>               solicitedMsg,
                         Message::Callback::Status              status,
                         std::shared_ptr<qmi_cat_rsp_data_type> responseDataPtr)
  {
    auto resp_msg_ptr = std::make_shared<GstkQmiCatResponseMsg>(responseDataPtr);

    (void) solicitedMsg;
    (void) status;

    if (resp_msg_ptr != nullptr)
    {
      resp_msg_ptr->dispatch();
    }
  }
};

