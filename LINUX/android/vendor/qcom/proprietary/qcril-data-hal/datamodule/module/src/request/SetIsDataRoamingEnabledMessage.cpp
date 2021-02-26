/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "request/SetIsDataRoamingEnabledMessage.h"

namespace rildata {

SetIsDataRoamingEnabledMessage::~SetIsDataRoamingEnabledMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string SetIsDataRoamingEnabledMessage::dump() {
  return mName + "{ }";
}

}// namespace
