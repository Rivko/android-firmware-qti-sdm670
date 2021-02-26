/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <string.h>
#include "request/RilRequestStartKeepaliveMessage.h"

namespace rildata {

RilRequestStartKeepaliveMessage::~RilRequestStartKeepaliveMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string RilRequestStartKeepaliveMessage::dump() {
  return mName;
}

}// namespace
