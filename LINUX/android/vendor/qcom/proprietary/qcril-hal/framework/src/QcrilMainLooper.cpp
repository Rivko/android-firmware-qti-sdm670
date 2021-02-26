/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/QcrilMainLooper.h"
#include "framework/Module.h"

void QcrilMainLooper::handleMessage(std::shared_ptr<Message> msg) {
  mModule->handleMessage(msg);
}

void QcrilMainLooper::handleMessageSync(std::shared_ptr<Message> msg) {
  mModule->handleMessageSync(msg);
}
