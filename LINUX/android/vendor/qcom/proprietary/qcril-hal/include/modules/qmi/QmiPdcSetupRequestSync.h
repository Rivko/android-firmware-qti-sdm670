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

class QmiPdcSetupRequestSync : public SolicitedSyncMessage<string>,
                            public add_message_id<QmiPdcSetupRequestSync>
{
 public:
  static constexpr const char *MESSAGE_NAME = "QMI_PDC_CLIENT_SETUP_REQ";

  inline QmiPdcSetupRequestSync(string clientToken):
    SolicitedSyncMessage<string>(get_class_message_id())
  {
    mToken = clientToken;
    mName = MESSAGE_NAME;
  }

  ~QmiPdcSetupRequestSync();

  string dump();

 private:
  string mToken;
};
