/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <string.h>
#include "request/SetApnInfoMessage.h"

namespace rildata {

SetApnInfoMessage::~SetApnInfoMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }

  if (mApnType != nullptr) {
    delete[] mApnType;
  }
  if (mApnName != nullptr) {
    delete[] mApnName;
  }
}

string SetApnInfoMessage::dump() {
  return mName;
}

}// namespace
