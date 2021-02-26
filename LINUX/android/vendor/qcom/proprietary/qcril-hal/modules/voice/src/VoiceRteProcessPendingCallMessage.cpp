/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/VoiceRteProcessPendingCallMessage.h"
#include "telephony/ril.h"

VoiceRteProcessPendingCallMessage::~VoiceRteProcessPendingCallMessage() {
}

string VoiceRteProcessPendingCallMessage::dump() {
  return VoiceRteProcessPendingCallMessage::MESSAGE_NAME;
}

unsigned int VoiceRteProcessPendingCallMessage::getCallRadioTechFamily()
{
  return mTechFamily;
}
