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
#include <memory>

class QmiSendRawAsyncMessage : public SolicitedMessage<qmi_client_error_type>,
                               public add_message_id<QmiSendRawAsyncMessage>
{
 friend class CallbackAsync;

 public:
  class CallbackAsync : public GenericCallback<qmi_client_error_type> {
   private:
   public:
    inline CallbackAsync(string str)
        : GenericCallback<qmi_client_error_type>(str) {}
    inline ~CallbackAsync() {}

    inline Message::Callback *clone() {
      CallbackAsync *clone = new CallbackAsync(
            GenericCallback<qmi_client_error_type>::mClientToken);
      return clone;
    }

    inline void onResponse(std::shared_ptr<Message> solicitedMsg,
                           Message::Callback::Status status,
                           std::shared_ptr<qmi_client_error_type> responseDataPtr) {
      (void)status;
      std::shared_ptr<QmiSendRawAsyncMessage> shared_qmiMsg =
            std::static_pointer_cast<QmiSendRawAsyncMessage>(solicitedMsg);

      shared_qmiMsg->mSendResult = *responseDataPtr;
    }
  };

  static constexpr const char *MESSAGE_NAME = "QMI_RAW_SEND_ASYNC";
  QmiSendRawAsyncMessage() = delete;
  ~QmiSendRawAsyncMessage();

  inline QmiSendRawAsyncMessage(std::shared_ptr<QmiReqAsyncStruct> dataPtr, Module *client) :
        SolicitedMessage<qmi_client_error_type>(get_class_message_id()) {
    mQmiReqPtr = dataPtr;
    mClientModule = client;
    CallbackAsync cb("sendQmiRawAsync");
    setCallback(&cb);
  }

  std::shared_ptr<QmiReqAsyncStruct> getData();
  qmi_client_error_type getSendResult();

  Module *getClientModule() {
    return mClientModule;
  }

  string dump();

 private:
  std::shared_ptr<QmiReqAsyncStruct> mQmiReqPtr;
  Module *mClientModule;
  qmi_client_error_type mSendResult;
};
