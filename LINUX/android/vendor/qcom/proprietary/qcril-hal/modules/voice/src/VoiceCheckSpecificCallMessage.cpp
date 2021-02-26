/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/voice/VoiceCheckSpecificCallMessage.h"

static constexpr qcril_qmi_voice_call_filter filter_array[
    VoiceCheckSpecificCallMessage::CALL_TYPE_MAX] = {
        qcril_qmi_voice_call_to_atel };

VoiceCheckSpecificCallMessage::~VoiceCheckSpecificCallMessage() {
}

VoiceCheckSpecificCallMessage::VoiceCheckSpecificCallMessage(
            specific_call_type_e_type type) : SolicitedSyncMessage<SpecificCallResult>(
            get_class_message_id()) {

  if (type < CALL_TYPE_MAX)
    mFilter = filter_array[type];
  else
    mFilter = nullptr;
}

qcril_qmi_voice_call_filter VoiceCheckSpecificCallMessage::getVoiceCallFilter() {
  return mFilter;
}

string VoiceCheckSpecificCallMessage::dump() {
  return VoiceCheckSpecificCallMessage::MESSAGE_NAME;
}
