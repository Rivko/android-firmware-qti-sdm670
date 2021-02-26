/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiSendRawAsyncMessage.h"

QmiSendRawAsyncMessage::~QmiSendRawAsyncMessage() {
}

std::shared_ptr<QmiReqAsyncStruct> QmiSendRawAsyncMessage::getData() {
  return mQmiReqPtr;
}

qmi_client_error_type QmiSendRawAsyncMessage::getSendResult() {
  return mSendResult;
}

string QmiSendRawAsyncMessage::dump() {
  return mName;
}
