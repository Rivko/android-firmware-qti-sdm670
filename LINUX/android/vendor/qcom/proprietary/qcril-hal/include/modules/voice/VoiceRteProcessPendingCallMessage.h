/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/message_id.h"
#include "framework/add_message_id.h"

class VoiceRteProcessPendingCallMessage :  public UnSolicitedMessage,
                              public add_message_id<VoiceRteProcessPendingCallMessage>
{
 private:
  unsigned int mTechFamily;

 public:
  static constexpr const char *MESSAGE_NAME = "RTE_PROCESS_PENDING_CALL";
  VoiceRteProcessPendingCallMessage() = default;
  ~VoiceRteProcessPendingCallMessage();

  VoiceRteProcessPendingCallMessage(unsigned int techFamily) :
        UnSolicitedMessage(get_class_message_id()) {
    mTechFamily = techFamily;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<VoiceRteProcessPendingCallMessage>(mTechFamily));
  }

  unsigned int getCallRadioTechFamily();

  string dump();
};
