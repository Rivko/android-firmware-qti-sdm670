/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <framework/UnSolicitedMessage.h>

UnSolicitedMessage::UnSolicitedMessage(message_id_t id) : Message(id) {
}

void UnSolicitedMessage::broadcast() {
  std::shared_ptr<Message> shared_msgPtr = shared_from_this();
  Dispatcher::getInstance().dispatch(shared_msgPtr);
}

std::shared_ptr<UnSolicitedMessage> UnSolicitedMessage::clone() {
    return std::shared_ptr<UnSolicitedMessage>(nullptr);
}

Message::MessageType UnSolicitedMessage::getMessageType() {
  return (Message::MessageType::UnSolicitedMessage);
}
