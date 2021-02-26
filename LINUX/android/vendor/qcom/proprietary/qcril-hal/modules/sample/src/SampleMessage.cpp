/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/sample/SampleMessage.h"

SampleMessage::~SampleMessage() {
  // Log::getInstance().d("RilRequestDialMessage destructor");
  delete[] mDialReqPtr->address;
  mDialReqPtr->address = nullptr;

  delete mDialReqPtr->uusInfo;
  mDialReqPtr->uusInfo = nullptr;

  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }

  delete mDialReqPtr;
  mDialReqPtr = nullptr;
}

void SampleMessage::deepCopy(RIL_Dial const *dialReqPtr) {
  mDialReqPtr = new RIL_Dial;
  char *address = new char[strlen(dialReqPtr->address) + 1];
  strcpy(address, dialReqPtr->address);
  mDialReqPtr->address = address;
  mDialReqPtr->clir = dialReqPtr->clir;
  mDialReqPtr->uusInfo = new RIL_UUS_Info;
  mDialReqPtr->uusInfo->uusType = dialReqPtr->uusInfo->uusType;
  mDialReqPtr->uusInfo->uusDcs = dialReqPtr->uusInfo->uusDcs;
  mDialReqPtr->uusInfo->uusLength = dialReqPtr->uusInfo->uusLength;
  mDialReqPtr->uusInfo->uusData = dialReqPtr->uusInfo->uusData;
}

string SampleMessage::getNumber() { return mDialReqPtr->address; };

RIL_Dial *SampleMessage::getData() { return mDialReqPtr; }

string SampleMessage::dump() {
  return mName + "{ " + mDialReqPtr->address + "}";
}
