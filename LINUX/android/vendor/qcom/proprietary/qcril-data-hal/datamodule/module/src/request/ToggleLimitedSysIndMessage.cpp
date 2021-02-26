/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <string.h>
#include "request/ToggleLimitedSysIndMessage.h"

namespace rildata {

ToggleLimitedSysIndMessage::~ToggleLimitedSysIndMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string ToggleLimitedSysIndMessage::dump() {
  return mName;
}

}// namespace
