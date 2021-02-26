/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/Util.h"
#include "framework/add_message_id.h"
#include "framework/Dispatcher.h"

class GetOperatingModeSyncMessage : public SolicitedSyncMessage<int>,
                                    public add_message_id<GetOperatingModeSyncMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "GET_OPERATING_MODE";
  inline GetOperatingModeSyncMessage(GenericCallback<int> *callback) :
    SolicitedSyncMessage< int >(get_class_message_id())
  {
    ignore(callback);
    mName = MESSAGE_NAME;
  }
  ~GetOperatingModeSyncMessage();

  string dump();
};
