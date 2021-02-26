/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <string.h>
#include "request/UpdateMtuMessage.h"

namespace rildata {

UpdateMtuMessage::~UpdateMtuMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string UpdateMtuMessage::dump() {
  return mName;
}

}// namespace
