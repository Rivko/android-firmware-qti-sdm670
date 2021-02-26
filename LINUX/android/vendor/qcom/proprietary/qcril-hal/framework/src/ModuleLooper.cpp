/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/ModuleLooper.h"
#include "framework/Module.h"

void ModuleLooper::handleMessage(std::shared_ptr<Message> msg) {
  mModule->handleMessage(msg);
}

void ModuleLooper::handleMessageSync(std::shared_ptr<Message> msg) {
  mModule->handleMessageSync(msg);
}
