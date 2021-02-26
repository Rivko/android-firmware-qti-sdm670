/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/Dispatcher.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"
#include "framework/qcril_event.h"
#include "qcrili.h"

class LegacyHandlerMessage : public SolicitedMessage<int>,
                             public add_message_id<LegacyHandlerMessage>
{
 public:
  static constexpr const char * MESSAGE_NAME = "LEGACY_HANDLER_MESSAGE";
  class LegacyHandlerCallback : public GenericCallback<int> {
   private:
   public:
    inline LegacyHandlerCallback(string str)
        : GenericCallback<int>(str) {}
    inline ~LegacyHandlerCallback() {}

    inline Message::Callback *clone() {
      LegacyHandlerCallback *clone = new LegacyHandlerCallback(
            GenericCallback<int>::mClientToken);
      return clone;
    }

    inline void onResponse(std::shared_ptr<Message> solicitedMsg,
                           Message::Callback::Status status,
                           std::shared_ptr<int> responseDataPtr) {
      (void)status;
      (void)responseDataPtr;

      std::shared_ptr<LegacyHandlerMessage> shared_msg =
            std::static_pointer_cast<LegacyHandlerMessage>(solicitedMsg);
      if (shared_msg != nullptr) {
          // TODO: use a message way to clean up the module. Need to create
          // a module that does common things
          Module* pModule = shared_msg->getSubModule();
          if (pModule) {
            qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                              QCRIL_DEFAULT_MODEM_ID,
                              QCRIL_DATA_ON_STACK,
                              QCRIL_EVT_CLEAN_UP_MODULE,
                              &pModule,
                              sizeof(pModule),
                              (RIL_Token)QCRIL_TOKEN_ID_INTERNAL);
          }
      }
    }
  };

  LegacyHandlerMessage() = delete;
  ~LegacyHandlerMessage();

  inline LegacyHandlerMessage(qcril_request_params_type* params, qcril_req_handler_type* handler,
                              Module* subModule, bool delSubModule):
            SolicitedMessage<int>(get_class_message_id())  {
    mParams = params;
    mHandler = handler;
    mSubModule = subModule;

    if (delSubModule) {
        LegacyHandlerCallback cb("LegacyHandlerCallback");
        setCallback(&cb);
    } else {
        setCallback(nullptr);
    }
  }

  Module* getSubModule();
  qcril_request_params_type* getLegacyParams();
  qcril_req_handler_type* getLegacyHandler();
  string dump();

 private:
  qcril_request_params_type* mParams;
  qcril_req_handler_type* mHandler;
  Module* mSubModule;
};
