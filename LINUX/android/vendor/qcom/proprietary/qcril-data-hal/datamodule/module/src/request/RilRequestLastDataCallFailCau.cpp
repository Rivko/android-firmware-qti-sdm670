/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestLastDataCallFailCauseMessage.h"

namespace rildata {

RilRequestLastDataCallFailCauseMessage::~RilRequestLastDataCallFailCauseMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RilRequestLastDataCallFailCauseMessage::dump() {
  return mName + "{ }";
}

}// namespace
