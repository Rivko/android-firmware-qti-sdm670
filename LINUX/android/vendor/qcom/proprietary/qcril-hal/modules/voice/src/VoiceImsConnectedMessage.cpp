/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/VoiceImsConnectedMessage.h"
#include "telephony/ril.h"

VoiceImsConnectedMessage::~VoiceImsConnectedMessage() {
}

string VoiceImsConnectedMessage::dump() {
  return VoiceImsConnectedMessage::MESSAGE_NAME;
}
