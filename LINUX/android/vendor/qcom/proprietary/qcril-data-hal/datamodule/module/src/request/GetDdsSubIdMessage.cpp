/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "modules/android/ril_message_factory.h"
#include "request/GetDdsSubIdMessage.h"

namespace rildata {

GetDdsSubIdMessage::~GetDdsSubIdMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string GetDdsSubIdMessage::dump() {
  return mName + "{ }";
}

}// namespace
