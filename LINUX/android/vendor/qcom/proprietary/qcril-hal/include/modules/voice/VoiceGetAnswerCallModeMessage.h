/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"

class VoiceGetAnswerCallModeMessage : public SolicitedSyncMessage<call_mode_enum_v02>,
                                      public add_message_id<VoiceGetAnswerCallModeMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.voice.get_answer_call_mode";
  ~VoiceGetAnswerCallModeMessage();

  inline VoiceGetAnswerCallModeMessage() :
    SolicitedSyncMessage<call_mode_enum_v02>(get_class_message_id()) {
  }

  string dump();
};
