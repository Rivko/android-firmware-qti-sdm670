/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestPullLceDataMessage.h"

namespace rildata {

RilRequestPullLceDataMessage::~RilRequestPullLceDataMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RilRequestPullLceDataMessage::dump() {
  return mName;
}

}// namespace
