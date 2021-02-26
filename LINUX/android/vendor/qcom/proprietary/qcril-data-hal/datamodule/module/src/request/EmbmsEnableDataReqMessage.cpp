/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/EmbmsEnableDataReqMessage.h"

namespace rildata {

EmbmsEnableDataReqMessage::~EmbmsEnableDataReqMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string EmbmsEnableDataReqMessage::dump() {
  return "<TODO>";
}

}// namespace
