/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/DedicatedThreadModule.h"
#include "framework/ModuleLooper.h"
#include "framework/TaskCompleteMessage.h"
#include <typeinfo>

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
*/
DedicatedThreadModule::DedicatedThreadModule() {
  mName = "DedicatedThreadModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(DedicatedThreadMessage, DedicatedThreadModule::handleDedicatedThreadMessage),
  };
}

/* Follow RAII.
*/
DedicatedThreadModule::~DedicatedThreadModule() {}

/*
 * Module specific initialization that does not belong to RAII .
*/
void DedicatedThreadModule::init() { Module::init(); }

/*
 * List of individual private handlers for the subscribed messages.
*/
void DedicatedThreadModule::handleDedicatedThreadMessage(
        std::shared_ptr<DedicatedThreadMessage> shared_msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + shared_msg->dump());

  auto mHandler = shared_msg->getHandler();

  if (mHandler) {
    mHandler(shared_msg->getCbData());
  }

  /* no need to send the resp since callback is nullptr */

  /* notify the we are idle now */
  auto msg = std::make_shared<TaskCompleteMessage>(this);
  Dispatcher::getInstance().dispatch(msg);
}
