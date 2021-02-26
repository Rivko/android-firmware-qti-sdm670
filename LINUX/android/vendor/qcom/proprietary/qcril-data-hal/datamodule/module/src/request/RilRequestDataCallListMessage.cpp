/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "modules/android/ril_message_factory.h"
#include "DataCommon.h"
#include "request/RilRequestDataCallListMessage.h"

namespace rildata {

RilRequestDataCallListMessage::~RilRequestDataCallListMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RilRequestDataCallListMessage::dump() {
  return mName + "{ }";
}

}// namespace
