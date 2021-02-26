/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiSetupRequest.h"

QmiSetupRequest::~QmiSetupRequest() {
    if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
    }
}

string QmiSetupRequest::dump() {
  return mName + "{ client token = " + mToken + "}";
}
