/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/VoiceRteChangePropagationMessage.h"
#include "telephony/ril.h"

VoiceRteChangePropagationMessage::~VoiceRteChangePropagationMessage() {
}

string VoiceRteChangePropagationMessage::dump() {
  return VoiceRteChangePropagationMessage::MESSAGE_NAME;
}
