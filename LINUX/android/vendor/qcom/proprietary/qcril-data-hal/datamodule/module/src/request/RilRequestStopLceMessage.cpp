/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestStopLceMessage.h"

namespace rildata {

RilRequestStopLceMessage::~RilRequestStopLceMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RilRequestStopLceMessage::dump() {
  return mName;
}

}// namespace
