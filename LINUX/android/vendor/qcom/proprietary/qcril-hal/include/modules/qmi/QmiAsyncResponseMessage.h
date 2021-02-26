/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "framework/Message.h"
#include "telephony/ril.h"
#include "modules/qmi/QmiStruct.h"

class QmiAsyncResponseMessage : public SolicitedMessage<void>,
                                public add_message_id<QmiAsyncResponseMessage> {
  private:
    QmiAsyncRespData *mData;

    void deepCopy(QmiAsyncRespData const *data);

  public:
    static constexpr const char *const MESSAGE_NAME = "com.qualcomm.qti.qcril.qmi_async_resp";

    QmiAsyncResponseMessage() = delete;

    ~QmiAsyncResponseMessage();

    inline QmiAsyncResponseMessage(QmiAsyncRespData *data) :
      SolicitedMessage<void>(get_class_message_id()) {
      mData = nullptr;
      mName = MESSAGE_NAME;
      deepCopy(data);
    }

    QmiAsyncRespData *getData();

    string dump();
};
