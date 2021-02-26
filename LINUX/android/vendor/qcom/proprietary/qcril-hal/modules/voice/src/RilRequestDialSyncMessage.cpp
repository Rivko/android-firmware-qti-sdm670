/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/RilRequestDialSyncMessage.h"
#include "telephony/ril.h"

RilRequestDialSyncMessage::~RilRequestDialSyncMessage() {
  // Log::getInstance().d("RilRequestDialSyncMessage destructor");
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

void RilRequestDialSyncMessage::deepCopy(RIL_Dial const *dialReqPtr) {
  int addrLen = strlen(dialReqPtr->address);
  char *address = new char[addrLen + 1];
  mDialReqPtr = new RIL_Dial;
  if(mDialReqPtr && address)
  {
    strlcpy(address, dialReqPtr->address, (sizeof(char) * (addrLen + 1)));
    mDialReqPtr->address = address;
    mDialReqPtr->clir = dialReqPtr->clir;
    mDialReqPtr->uusInfo = new RIL_UUS_Info;
    if(mDialReqPtr->uusInfo) {
      mDialReqPtr->uusInfo->uusType = dialReqPtr->uusInfo->uusType;
      mDialReqPtr->uusInfo->uusDcs = dialReqPtr->uusInfo->uusDcs;
      mDialReqPtr->uusInfo->uusLength = dialReqPtr->uusInfo->uusLength;
      mDialReqPtr->uusInfo->uusData = dialReqPtr->uusInfo->uusData;
    }
  }
}

string RilRequestDialSyncMessage::getNumber() { return mDialReqPtr->address; };

RIL_Dial *RilRequestDialSyncMessage::getData() { return mDialReqPtr; }

string RilRequestDialSyncMessage::dump() {
  return mName + "{ " + mDialReqPtr->address + "}";
}
