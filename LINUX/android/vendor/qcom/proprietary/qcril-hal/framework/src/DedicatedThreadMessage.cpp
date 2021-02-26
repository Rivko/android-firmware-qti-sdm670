/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/DedicatedThreadMessage.h"

DedicatedThreadMessage::~DedicatedThreadMessage() {
}

DedicatedThreadMessage::HandlerType DedicatedThreadMessage::getHandler() {
    return mHandler;
}

void* DedicatedThreadMessage::getCbData() {
    return mCbData;
}

string DedicatedThreadMessage::dump() {
    return mName;
}
