/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/VoiceLimitIndMessage.h"
#include "telephony/ril.h"

VoiceLimitIndMessage::~VoiceLimitIndMessage() {
}

string VoiceLimitIndMessage::dump() {
  return VoiceLimitIndMessage::MESSAGE_NAME;
}

bool VoiceLimitIndMessage::isIndLimited()
{
  return mIsLimited;
}
