/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/voice/VoiceCheckCallPresentMessage.h"

VoiceCheckCallPresentMessage::~VoiceCheckCallPresentMessage() {
}

VoiceCheckCallPresentMessage::call_type_e_type
VoiceCheckCallPresentMessage::getCallType() {
  return mType;
}

string VoiceCheckCallPresentMessage::dump() {
  return VoiceCheckCallPresentMessage::MESSAGE_NAME;
}
