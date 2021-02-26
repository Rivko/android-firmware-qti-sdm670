/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/Dispatcher.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"

class DedicatedThreadMessage : public SolicitedMessage<void>,
                               public add_message_id<DedicatedThreadMessage>
{
 public:
  static constexpr const char * MESSAGE_NAME = "DEDICATED_THREAD_MESSAGE";
  typedef void (*HandlerType)(void*);

  DedicatedThreadMessage() = delete;
  ~DedicatedThreadMessage();

  inline DedicatedThreadMessage(HandlerType handler, void* cbData):
            SolicitedMessage<void>(get_class_message_id())  {
    mName = MESSAGE_NAME;
    mHandler = handler;
    mCbData = cbData;
    setCallback(nullptr);
  }

  HandlerType getHandler();
  void* getCbData();
  string dump();

 private:
  HandlerType mHandler;
  void* mCbData;
};
