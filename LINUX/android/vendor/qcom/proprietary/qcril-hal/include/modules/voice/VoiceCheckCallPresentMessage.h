/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "voice_service_v02.h"
#include "qcril_qmi_voice.h"

class VoiceCheckCallPresentMessage : public SolicitedSyncMessage<bool>,
                                      public add_message_id<VoiceCheckCallPresentMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.voice.check_call_present";
  typedef enum call_type_enum {
    ALL_CALL,
    VOICE_CALL,
    MAX_TYPE_NUM
  } call_type_e_type;

  VoiceCheckCallPresentMessage() = delete;
  ~VoiceCheckCallPresentMessage();

  inline VoiceCheckCallPresentMessage(call_type_e_type type) :
    SolicitedSyncMessage<bool>(get_class_message_id()) {
    mType = type;
  }

  VoiceCheckCallPresentMessage::call_type_e_type getCallType();
  string dump();

 private:
  call_type_enum mType;
};
