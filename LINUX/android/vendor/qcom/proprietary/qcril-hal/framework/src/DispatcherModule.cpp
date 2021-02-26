/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/DispatcherModule.h"
#include "framework/Dispatcher.h"
#include "framework/QcrilMainLooper.h"
#include "framework/Log.h"

DispatcherModule::DispatcherModule() {
  mName = "DispatcherModule";
  mLooper = std::unique_ptr<QcrilMainLooper>(new QcrilMainLooper);
}

DispatcherModule::~DispatcherModule() {
    mLooper = nullptr;
}

/* Override default behaviour */
void DispatcherModule::handleMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DispatcherModule]: Handling msg = " + msg->dump());
  Dispatcher::getInstance().forwardMsgToModule(msg);
}

/* Override default behaviour */
void DispatcherModule::handleMessageSync(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[DispatcherModule]: Handling SYNC msg = " +
                       msg->dump());
  Dispatcher::getInstance().forwardMsgToModuleSync(msg);
}
