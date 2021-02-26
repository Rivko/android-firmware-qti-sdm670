/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiSetupRequestSync.h"

QmiSetupRequestSync::~QmiSetupRequestSync() {
}

string QmiSetupRequestSync::dump() {
  return mName + "{ client token = " + mToken + "}";
}
