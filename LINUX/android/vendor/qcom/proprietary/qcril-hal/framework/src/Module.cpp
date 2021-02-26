/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/Module.h"
#include "framework/Dispatcher.h"
#include "framework/Log.h"

using std::mutex;
using std::lock_guard;

void Module::onMessageArrival(std::shared_ptr<Message> msg) {
  /* TODO: Validate invariant: This method must be invoked in dispather looper context */
  if (mLooper != nullptr) {
    mLooper->dispatch(msg);
  } else {
    handleMessage(msg);
  }
}

void Module::dispatch(std::shared_ptr<Message> msg) {
  if (!msg->get_next_hop()) {
    msg->set_next_hop(this);
  }

  if (mLooper != nullptr) {
    mLooper->dispatch(msg);
  } else {
    Dispatcher::getInstance().dispatch(msg);
  }
}

void Module::dispatchSync(std::shared_ptr<Message> msg) {
  if (mLooper != nullptr) {
    mLooper->dispatchSync(msg);
  } else {
    handleMessageSync(msg);
  }
}

void Module::init() {
  if (mLooper != nullptr) {
    mLooper->init(this);
  }

  for (const auto &elem : mMessageHandler) {
    auto h = elem.first;
    Log::getInstance().d("[" +to_string()+"]: Register msg=" + h->get_name()+" handler with dispatcher.");

    Dispatcher::getInstance().registerHandler(h, this);
    Dispatcher::getInstance().dump_message_id(h);
  }
}

Module::~Module() {

    Dispatcher::getInstance().unRegisterHandler(this);
    mLooper = nullptr;
    mMessageHandler.clear();
}

void Module::handleMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Waiting for lock for msg = " +
                       msg->to_string());
  //lock_guard<std::recursive_mutex> lock(mModuleHandlerExecutionMutex);
  Log::getInstance().d("[" + mName + "]: Acquired lock for msg = " +
                       msg->to_string());

  message_id_t id = msg->get_message_id();
  if (mMessageHandler.find(id) != mMessageHandler.end()) {
    if (!msg->isExpired()) {
      msg->handlerExecuting();
      /* Set timer to timeout this message if module is taking too long to
      * call callback*/
      if (msg->getMessageType() != Message::MessageType::UnSolicitedMessage) {
        Dispatcher::getInstance().setTimeoutForMessage(msg, msg->getMessageExpiryTimer());
      }
      /* Found a handler */
      auto handler{mMessageHandler[id]};
      handler(msg);
      if(!msg->isCallbackPresent()) {
          Log::getInstance().d("[" + mName + "]: No callback present, " +
                         "finishing the transaction for msg = " +
                         msg->get_message_name());
          Dispatcher::getInstance().informMessageCallbackFinished(msg);
      }
      msg->handlerExecuted();
    } else {
      Log::getInstance().d(
          "[" + mName +
          "]: Handler can not be invoked, msg is already cancelled = " +
          msg->dump());
    }
  } else {
    Log::getInstance().d("[" + mName + "]: No known handler found for " +
                         msg->get_message_name());
  }
  Log::getInstance().d("[" + mName + "]: Releasing lock for msg = " +
                       msg->to_string());
}

void Module::handleMessageSync(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Waiting for lock for msg = " +
                       msg->to_string());
  //lock_guard<std::recursive_mutex> lock(mModuleHandlerExecutionMutex);
  Log::getInstance().d("[" + mName + "]: Acquired lock for msg = " +
                       msg->to_string());

  string msgName = msg->to_string();
  if (mMessageHandler.find(msg->get_message_id()) != mMessageHandler.end()) {
    msg->handlerExecuting();
    /* Found a handler */
    Log::getInstance().d("[" + mName +
                         "]: Executing handler synchronously for  " + msgName);
    auto handler{mMessageHandler[msg->get_message_id()]};
    handler(msg);
    if(!msg->isCallbackPresent()) {
        Log::getInstance().d("[" + mName + "]: No callback present, " +
                       "finishing the transaction for msg = " +
                       msg->get_message_name());
        Dispatcher::getInstance().informMessageCallbackFinished(msg);
    }
    msg->handlerExecuted();
  } else {
    Log::getInstance().d("[" + mName + "]: No known handler found for " +
                         msgName);
  }
  Log::getInstance().d("[" + mName + "]: Releasing lock for msg = " +
                       msg->to_string());
}

std::thread *Module::getLooperThread() {
  if (mLooper != nullptr) {
    return mLooper->getLooperThread();
  } else {
    return nullptr;
  }
}

bool Module::hasValidLooper() {
  if (mLooper != nullptr) {
    return true;
  }
  return false;
}

size_t Module::getUnProcessedMessageCount() {
  if (mLooper != nullptr) {
    return mLooper->getSize();
  }
  return 0;
}

void Module::waitForLooperToConsumeAllMsgs() {
  if (mLooper != nullptr) {
    mLooper->waitForLooperToConsumeAllMsgs();
  }
}

void Module::killLooper() {
  if (mLooper != nullptr) {
    mLooper->killLooper();
  }
}

string Module::to_string() { return mName; }
