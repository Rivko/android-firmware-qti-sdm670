/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/qmi/QmiIndMessage.h"

QmiIndMessage::~QmiIndMessage() {
  if (mIndDataPtr != nullptr) {
    if (mIndDataPtr->indData != nullptr) {
        delete[] mIndDataPtr->indData;
    }
    delete mIndDataPtr;
    mIndDataPtr = nullptr;
  }
}

void QmiIndMessage::deepCopy(QmiIndMsgDataStruct const *dataPtr) {
  mIndDataPtr = new QmiIndMsgDataStruct;
  if (mIndDataPtr == nullptr)
    return;
  mIndDataPtr->msgId = dataPtr->msgId;

  mIndDataPtr->indData = new unsigned char[dataPtr->indSize];
  if(mIndDataPtr->indData != nullptr)
  {
    memcpy(mIndDataPtr->indData,dataPtr->indData,dataPtr->indSize);
  }

  mIndDataPtr->indSize = dataPtr->indSize;
  mIndDataPtr->cbData = dataPtr->cbData;
}

QmiIndMsgDataStruct* QmiIndMessage::getData() { return mIndDataPtr; }

string QmiIndMessage::dump() {
  return mName;
}
