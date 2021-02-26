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

class QmiSetupRequest : public SolicitedMessage<string>,
                        public add_message_id<QmiSetupRequest>
{
 public:
  static constexpr const char * MESSAGE_NAME = "QMI_CLIENT_SETUP_REQ";
  inline QmiSetupRequest(string clientToken, size_t dataSize, void *dataPtr,
                         GenericCallback<string> *callback):
      SolicitedMessage<string>(get_class_message_id()),
      mToken(clientToken)
  {
    (void)dataSize;
    (void)dataPtr;
    setCallback(callback);
  }

  ~QmiSetupRequest();

  string dump();

 private:
  string mToken;
};
