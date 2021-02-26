/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/sample/SampleModule.h"
#include "framework/ModuleLooper.h"
#include "modules/sample/SampleMessage.h"
#include <typeinfo>

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
*/
SampleModule::SampleModule() {
  mName = "SampleModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(SampleMessage, SampleModule::handleSampleMessage),
  };
}

/* Follow RAII.
*/
SampleModule::~SampleModule() {}

/*
 * Module specific initialization that does not belong to RAII .
*/
void SampleModule::init() { Module::init(); }

/*
 * List of individual private handlers for the subscribed messages.
*/
void SampleModule::handleSampleMessage(std::shared_ptr<SampleMessage> shared_msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + shared_msg->dump());

  /* Do some module specific operation for this message handling.*/

  /* If we need to respond back to client*/
  std::shared_ptr<string> shared_pResponse(new string("My awesome response."));
  shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
                           shared_pResponse);
}
