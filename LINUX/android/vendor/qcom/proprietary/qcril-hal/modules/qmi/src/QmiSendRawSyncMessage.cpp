/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiSendRawSyncMessage.h"

QmiSendRawSyncMessage::~QmiSendRawSyncMessage() {
}

QmiSyncStruct* QmiSendRawSyncMessage::getData() {
  return mQmiPtr;
}

qmi_client_error_type QmiSendRawSyncMessage::getSendResult() {
  return mSendResult;
}

string QmiSendRawSyncMessage::dump() {
  return mName;
}
