/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/android/LegacyHandlerSubModule.h"
#include "modules/android/LegacyHandlerMessage.h"
#include "framework/ModuleLooper.h"
#include <typeinfo>

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
*/
LegacyHandlerSubModule::LegacyHandlerSubModule() {
  mName = "LegacyHandlerSubModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  mMessageHandler = {
      HANDLER(LegacyHandlerMessage, LegacyHandlerSubModule::handleLegacyHandlerMessage),
  };
}

/* Follow RAII.
*/
LegacyHandlerSubModule::~LegacyHandlerSubModule() {}

/*
 * List of individual private handlers for the subscribed messages.
*/
void LegacyHandlerSubModule::handleLegacyHandlerMessage(
        std::shared_ptr<LegacyHandlerMessage> shared_msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + shared_msg->dump());

  auto mHandler = shared_msg->getLegacyHandler();

  if (mHandler) {
    qcril_request_return_type ret_ptr;
    mHandler(shared_msg->getLegacyParams(), &ret_ptr);
  }

  /* after legacy handler completes, send response for clean up resources if necessary */
  shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS, nullptr);
}
