/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestGetDataCallProfileMessage.h"

namespace rildata {

RilRequestGetDataCallProfileMessage::~RilRequestGetDataCallProfileMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RilRequestGetDataCallProfileMessage::dump() {
  return mName + "{ }";
}

}// namespace
