/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/voice/VoiceGetAnswerCallModeMessage.h"

VoiceGetAnswerCallModeMessage::~VoiceGetAnswerCallModeMessage() {
}

string VoiceGetAnswerCallModeMessage::dump() {
  return VoiceGetAnswerCallModeMessage::MESSAGE_NAME;
}
