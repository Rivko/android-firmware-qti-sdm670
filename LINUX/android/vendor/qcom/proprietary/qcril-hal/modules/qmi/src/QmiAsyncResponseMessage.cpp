/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/qmi/QmiAsyncResponseMessage.h"

QmiAsyncResponseMessage::~QmiAsyncResponseMessage() {
  if (mData != nullptr) {
    delete mData;
    mData = nullptr;
  }
}

void QmiAsyncResponseMessage::deepCopy(QmiAsyncRespData const *data) {
  mData = new QmiAsyncRespData;
  if (mData == nullptr)
    return;
  mData->msgId = data->msgId;
  mData->respData = data->respData;
  mData->respDataSize = data->respDataSize;
  mData->traspErr = data->traspErr;
  mData->cb = data->cb;
  mData->cbData = data->cbData;
}

QmiAsyncRespData* QmiAsyncResponseMessage::getData() {
  return mData;
}

string QmiAsyncResponseMessage::dump() {
  return mName;
}
