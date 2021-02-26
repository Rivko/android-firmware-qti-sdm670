/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <string.h>
#include "request/ToggleDormancyIndMessage.h"

namespace rildata {

ToggleDormancyIndMessage::~ToggleDormancyIndMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string ToggleDormancyIndMessage::dump() {
  return mName;
}

}// namespace
