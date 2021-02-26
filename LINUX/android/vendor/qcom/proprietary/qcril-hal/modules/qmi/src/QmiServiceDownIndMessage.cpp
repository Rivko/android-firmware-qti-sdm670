/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiServiceDownIndMessage.h"

QmiServiceDownIndMessage::~QmiServiceDownIndMessage() {
}

qmi_client_error_type QmiServiceDownIndMessage::getErrorCode() {
  return mErrCode;
}

string QmiServiceDownIndMessage::dump() {
  return mName;
}
