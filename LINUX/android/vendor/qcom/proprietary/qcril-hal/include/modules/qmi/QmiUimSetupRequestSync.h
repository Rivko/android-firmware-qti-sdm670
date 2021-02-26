/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <string>
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/Util.h"
#include "framework/add_message_id.h"
#include "framework/Dispatcher.h"

using std::string;

class QmiUimSetupRequestSync : public SolicitedSyncMessage<string>,
                               public add_message_id<QmiUimSetupRequestSync>
{
 public:
  static constexpr const char *MESSAGE_NAME = "QMI_UIM_CLIENT_SETUP_SYNC_REQ";

  inline QmiUimSetupRequestSync(string clientToken, size_t dataSize, void *dataPtr,
                                GenericCallback<string> *callback):
    SolicitedSyncMessage< string >(get_class_message_id())
  {
    ignore(callback);
    ignore(dataSize);
    ignore(dataPtr);
    mToken = clientToken;
    mName = MESSAGE_NAME;
  }

  ~QmiUimSetupRequestSync()
  {
  }

  string dump()
  {
    return mName + "{ client token = " + mToken + "}";
  }

 private:
  string mToken;
};
