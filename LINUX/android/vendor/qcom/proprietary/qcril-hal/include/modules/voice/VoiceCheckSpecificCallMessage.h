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

typedef struct {
  bool has_call;
  call_mode_enum_v02 call_mode;
} SpecificCallResult;

class VoiceCheckSpecificCallMessage : public SolicitedSyncMessage<SpecificCallResult>,
                                      public add_message_id<VoiceCheckSpecificCallMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.voice.check_specific_call";
  typedef enum specific_call_type_enum {
    CALL_TO_ATEL,
    CALL_TYPE_MAX
  } specific_call_type_e_type;

  VoiceCheckSpecificCallMessage() = delete;
  ~VoiceCheckSpecificCallMessage();
  VoiceCheckSpecificCallMessage(specific_call_type_e_type type);

  qcril_qmi_voice_call_filter getVoiceCallFilter();
  string dump();

 private:
  qcril_qmi_voice_call_filter mFilter;
};
