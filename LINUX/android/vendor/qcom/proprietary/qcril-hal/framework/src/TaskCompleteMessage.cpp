/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "framework/TaskCompleteMessage.h"

TaskCompleteMessage::~TaskCompleteMessage() {
}


Module* TaskCompleteMessage::getModule() { return mModule; }

string TaskCompleteMessage::dump() {
    return mName;
}
