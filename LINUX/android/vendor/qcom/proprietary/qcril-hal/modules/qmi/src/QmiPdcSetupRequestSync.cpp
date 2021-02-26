/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiPdcSetupRequestSync.h"

QmiPdcSetupRequestSync::~QmiPdcSetupRequestSync() {
}

string QmiPdcSetupRequestSync::dump() {
  return mName + "{ client token = " + mToken + "}";
}
