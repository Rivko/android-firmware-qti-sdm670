/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Dispatcher.h"
#include "framework/add_message_id.h"

class QmiServiceUpIndMessage : public SolicitedMessage<void>,
                               public add_message_id<QmiServiceUpIndMessage>
{
 public:
  static constexpr const char* MESSAGE_NAME = "QMI_SERVICE_UP_IND";
  inline QmiServiceUpIndMessage(void* token):
      SolicitedMessage<void>(get_class_message_id())
  {
    mToken = token;
  }

  inline string dump() { return mName; }

  inline ~QmiServiceUpIndMessage(){};
  void* mToken;
};
