/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/SingleDispatchRestriction.h"
#include "framework/Dispatcher.h"
#include "framework/Log.h"

using std::lock_guard;
using std::mutex;

SingleDispatchRestriction::SingleDispatchRestriction() {
  mName = "SingleDispatchRestriction";
  mHandlingMsg = nullptr;
  mRedispatchedMsg = nullptr;
}

SingleDispatchRestriction::~SingleDispatchRestriction() {}

/*
 * Below logic ensures that msg which are pending in a separate queue due to restriction
 * are processed exactly in the order they arrived. This algo addresses a particular
 * race-condition where pending msg is re-enqueued to main dispatcher queue just after
 * a new msg from telephony is enqueued and hence ends up with incorrect order.
 * Here is the flow:
 * - Forward the incoming msg to module if msg dispatch is not restricted. Set mHandlingMsg
 *   to this msg.
 * - Append the incoming msg to mRestrictedMessagesQueue if restriction is applicable (There
 *   is current handling msg or re-dispatched pending msg or queue is not empty)
 * - Upon msg's callback execution by respective module, pop the head of mRestrictedMessagesQueue
 *   and redispatch. While doing that save the refrence to this msg in mRedispatchedMsg.
 * - If the incoming msg is mRedispatchedMsg then set mHandlingMsg to mRedispatchedMsg and
 *   mRedispatchedMsg to nullptr. Now forward this msg to respective module.
 */
bool SingleDispatchRestriction::isDispatchAllowed(
    std::shared_ptr<Message> msg) {
  lock_guard<mutex> lock(mMutex);

  string msgStr = msg->to_string();
  bool okToDispatch = false;

  if (msg == mRedispatchedMsg) {
    /* case 1: if the msg is the expected re-dispatched msg, handle it directly */
    assert(mHandlingMsg == nullptr);
    mHandlingMsg = msg;
    mRedispatchedMsg = nullptr;
    okToDispatch = true;
  } else if (!mHandlingMsg && !mRedispatchedMsg && !mRestrictedMessagesQueue.size()) {
    /* case 2: if nothing is under handling and nothing is in the waiting queue, and
     * no re-dispatched message is waiting execution, handle it immediately */
    mHandlingMsg = msg;
    okToDispatch = true;
  } else {
    /* Add this msg to the waiting list */
    mRestrictedMessagesQueue.push_back(msg);
    okToDispatch = false;
  }

  if (okToDispatch) {
    /* Message is restricted but no message is outstanding. Clear to go * ahead.*/
    Log::getInstance().d(
        "[" + mName + "]: Restrictions enabled for " +
        msg->to_string() + " but allowed to dispatch.");
    return true;
  } else {
    /* One message from the groupd is still inprogress and callback is not yet
     * executed. Wait.*/
    Log::getInstance().d("[" + mName + "]: Can not forward msg = " +
                         msg->to_string() + " until restriction is cleared");
    return false;
  }
}

void SingleDispatchRestriction::onMessageCallbackCompletion(
    std::shared_ptr<Message> msg) {
  {
    lock_guard<mutex> lock(mMutex);

    string msgStr = msg->to_string();
    Log::getInstance().d("[" + mName + "]: onMessageCallbackCompletion msg = " +
                       msg->dump());

    if (msg != mHandlingMsg) {
      Log::getInstance().d("[" + mName + "]: msg mismatch.. should not be here");
      return;
    }
    mHandlingMsg = nullptr;
    mRedispatchedMsg = nullptr;
    /* fetch the front msg in the queue and dispatch */
    if (!mRestrictedMessagesQueue.empty()) {
      mRedispatchedMsg = mRestrictedMessagesQueue.front();
      mRestrictedMessagesQueue.pop_front();
      Log::getInstance().d("[" + mName + "]: Requeue msg = " +
                         mRedispatchedMsg->dump());
    }
  }

  if(nullptr != mRedispatchedMsg) {
    Dispatcher::getInstance().dispatch(mRedispatchedMsg);
  }
}

void SingleDispatchRestriction::abandonPendingMessages() {
  lock_guard<mutex> lock(mMutex);
  Log::getInstance().d("[" + mName + "]: abandonPendingMessages");

  if (!mRestrictedMessagesQueue.empty()) {
    for (const auto &pendingMsg : mRestrictedMessagesQueue) {
      if (pendingMsg) {
        Log::getInstance().d("[" + mName + "]: informing CANCELLED");
        pendingMsg->informDispatchFailure(pendingMsg, Message::Callback::Status::CANCELLED);
      }
    }
    mRestrictedMessagesQueue.clear();
  }
}

string SingleDispatchRestriction::to_string() { return mName; }
