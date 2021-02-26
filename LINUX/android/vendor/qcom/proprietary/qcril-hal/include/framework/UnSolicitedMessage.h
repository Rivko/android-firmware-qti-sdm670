/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <memory>
#include "framework/Dispatcher.h"
#include "framework/Message.h"
#include "framework/GenericCallback.h"

class UnSolicitedMessage : public Message {
public:
  static constexpr Message::MessageType TYPE = Message::MessageType::UnSolicitedMessage;
  UnSolicitedMessage() = delete;
  UnSolicitedMessage(message_id_t id);
  void broadcast();
  virtual std::shared_ptr<UnSolicitedMessage> clone() = 0;
  Message::MessageType getMessageType() override;
};

