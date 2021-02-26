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

class QmiCatSetupRequest : public SolicitedMessage<string>,
                           public add_message_id<QmiCatSetupRequest>
{
 public:
  static constexpr const char * MESSAGE_NAME = "QMI_CAT_CLIENT_SETUP_REQ";
  inline QmiCatSetupRequest(string clientToken, size_t dataSize, void *dataPtr,
                            GenericCallback<string> *callback):
      SolicitedMessage<string>(get_class_message_id()),
      mToken(clientToken)
  {
    (void)dataSize;
    (void)dataPtr;
    mName = MESSAGE_NAME;
    setCallback(callback);
  }

  ~QmiCatSetupRequest()
  {
    if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
    }
  }

  string dump()
  {
    return mName + "{ client token = " + mToken + "}";
  }

 private:
  string mToken;
};

