/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/qmi/QmiStruct.h"

class QmiSendRawSyncMessage : public SolicitedMessage<qmi_client_error_type>,
                              public add_message_id<QmiSendRawSyncMessage>
{
 friend class CallbackSync;

 public:
  class CallbackSync : public GenericCallback<qmi_client_error_type> {
   public:
    inline CallbackSync(string str)
        : GenericCallback<qmi_client_error_type>(str) {}
    inline ~CallbackSync() {}

    inline Message::Callback *clone() {
      CallbackSync *clone = new CallbackSync(
            GenericCallback<qmi_client_error_type>::mClientToken);
      return clone;
    }

    inline void onResponse(std::shared_ptr<Message> solicitedMsg,
                           Message::Callback::Status status,
                           std::shared_ptr<qmi_client_error_type> responseDataPtr) {
      (void)status;
      std::shared_ptr<QmiSendRawSyncMessage> shared_qmiMsg =
            std::static_pointer_cast<QmiSendRawSyncMessage>(solicitedMsg);

      shared_qmiMsg->mSendResult = *responseDataPtr;
    }
  };

  static constexpr const char *MESSAGE_NAME = "QMI_RAW_SEND_SYNC";
  QmiSendRawSyncMessage() = delete;
  ~QmiSendRawSyncMessage();

  inline QmiSendRawSyncMessage(QmiSyncStruct *dataPtr) :
        SolicitedMessage<int>(get_class_message_id()) {
    mQmiPtr = dataPtr;
    CallbackSync cb("sendQmiRawSync");
    setCallback(&cb);
  }

  QmiSyncStruct* getData();
  qmi_client_error_type getSendResult();

  string dump();

 private:
  QmiSyncStruct *mQmiPtr;
  qmi_client_error_type mSendResult;
};
