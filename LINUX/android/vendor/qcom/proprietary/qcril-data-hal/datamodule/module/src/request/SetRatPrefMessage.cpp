/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/SetRatPrefMessage.h"

namespace rildata {

SetRatPrefMessage::~SetRatPrefMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string SetRatPrefMessage::dump() {
  return mName + "{ }";
}

}// namespace
