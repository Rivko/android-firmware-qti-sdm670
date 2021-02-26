/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "modules/android/ril_message_factory.h"
#include "request/RequestDdsSwitchMessage.h"

namespace rildata {

RequestDdsSwitchMessage::~RequestDdsSwitchMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RequestDdsSwitchMessage::dump() {
  return mName + "{ }";
}

}// namespace
