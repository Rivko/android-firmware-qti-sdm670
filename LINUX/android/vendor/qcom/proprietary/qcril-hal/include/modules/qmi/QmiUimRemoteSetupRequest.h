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

class QmiUimRemoteSetupRequest : public SolicitedMessage<string>,
                                 public add_message_id<QmiUimRemoteSetupRequest>
{
 public:
  static constexpr const char * MESSAGE_NAME = "QMI_UIM_REMOTE_CLIENT_SETUP_REQ";
  inline QmiUimRemoteSetupRequest(string clientToken, size_t dataSize, void *dataPtr,
                                  GenericCallback<string> *callback):
      SolicitedMessage<string>(get_class_message_id()),
      mToken(clientToken)
  {
    (void)dataSize;
    (void)dataPtr;
    mName = MESSAGE_NAME;
    setCallback(callback);
  }

  ~QmiUimRemoteSetupRequest()
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
