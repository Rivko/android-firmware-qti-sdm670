/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <string.h>
#include "request/RilRequestStopKeepaliveMessage.h"

namespace rildata {

RilRequestStopKeepaliveMessage::~RilRequestStopKeepaliveMessage() {
  if (mCallback) {
      delete mCallback;
      mCallback = nullptr;
  }
}

string RilRequestStopKeepaliveMessage::dump() {
    return mName;
}

}// namespace
