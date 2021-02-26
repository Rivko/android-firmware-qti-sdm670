/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestStartLceMessage.h"

namespace rildata {

RilRequestStartLceMessage::~RilRequestStartLceMessage() {
  if (mCallback) {
      delete mCallback;
      mCallback = nullptr;
  }
}

string RilRequestStartLceMessage::dump() {
    return mName;
}

}// namespace
