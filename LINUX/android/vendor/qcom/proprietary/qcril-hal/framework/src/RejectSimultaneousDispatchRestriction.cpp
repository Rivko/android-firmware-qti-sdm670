/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/RejectSimultaneousDispatchRestriction.h"
#include "framework/Dispatcher.h"
#include "framework/Log.h"

using std::lock_guard;
using std::mutex;

RejectSimultaneousDispatchRestriction::RejectSimultaneousDispatchRestriction() {
  mName = "RejectSimultaneousDispatchRestriction";
  allowNextMessage = true;
}

RejectSimultaneousDispatchRestriction::~RejectSimultaneousDispatchRestriction() {}

bool RejectSimultaneousDispatchRestriction::isDispatchAllowed(
    std::shared_ptr<Message> msg) {
  lock_guard<mutex> lock(mMutex);

  string msgStr = msg->to_string();
  bool okToDispatchCurrMsg = false;

  if (allowNextMessage) {
    /* Message is restricted but no previous message is outstanding. Clear to go
     * ahead.*/
    Log::getInstance().d(
        "[" + mName + "]: Restrictions enabled for " +
        msg->to_string() + " but allowed to dispatch.");
    allowNextMessage = false;
    okToDispatchCurrMsg = true;
  } else {
    Log::getInstance().d("[" + mName + "]: Can not forward msg = " +
                         msg->to_string());
    msg->informDispatchFailure(msg,
        Message::Callback::Status::FAILURE);
  }

  return okToDispatchCurrMsg;
}

void RejectSimultaneousDispatchRestriction::onMessageCallbackCompletion(
    std::shared_ptr<Message> msg) {
  lock_guard<mutex> lock(mMutex);
  string msgStr = msg->to_string();

  allowNextMessage = true;

    /* Re-enqueue the pending mesgs in the same order they came previously.*/
  Log::getInstance().d("[" + mName + "]: onMessageCallbackCompletion msg = " +
                       msg->dump());
}

string RejectSimultaneousDispatchRestriction::to_string() { return mName; }
