/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "string"
#include "request/RilRequestGoDormantMessage.h"
#include "DataCommon.h"

namespace rildata {

RilRequestGoDormantMessage::~RilRequestGoDormantMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RilRequestGoDormantMessage::dump() {
  return mName;
}

}// namespace
