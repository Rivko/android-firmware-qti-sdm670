/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/SetIsDataEnabledMessage.h"

namespace rildata {

SetIsDataEnabledMessage::~SetIsDataEnabledMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string SetIsDataEnabledMessage::dump() {
  return mName + "{ }";
}

}// namespace
