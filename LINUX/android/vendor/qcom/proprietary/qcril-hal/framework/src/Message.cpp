/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <shared_mutex>

#include "framework/Message.h"
#include "framework/Dispatcher.h"
#include "framework/SyncApiSession.h"

using std::lock_guard;

Message::~Message() {
  // Log::getInstance().d("Message base destructor");
}

Message::Callback::~Callback() {
  // Log::getInstance().d("Base Callback desctructor");
}

void Message::handlerExecuting() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mExecutionStatus = ExecutionStatus::HANDLER_INPROGRESS;
}

void Message::handlerExecuted() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mExecutionStatus = ExecutionStatus::HANDLER_EXECUTED;
}

void Message::callbackExecuting() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mExecutionStatus = ExecutionStatus::CALLBACK_INPROGRESS;
}

void Message::callbackExecuted() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mExecutionStatus = ExecutionStatus::CALLBACK_EXECUTED;
}

Message::ExecutionStatus Message::getExecutionStatus() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  return mExecutionStatus;
}

void Message::cancelling() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mExecutionStatus = ExecutionStatus::CANCELLATION_INPROGRESS;
}

void Message::cancelled() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mExecutionStatus = ExecutionStatus::CANCELLED;
}

bool Message::isHandlerExecuted() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  if (mExecutionStatus == ExecutionStatus::HANDLER_EXECUTED) {
    return true;
  } else {
    return false;
  }
}

bool Message::isHandlerExecuting() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  if (mExecutionStatus == ExecutionStatus::HANDLER_INPROGRESS) {
    return true;
  }
  else {
    return false;
  }
}

bool Message::isCallbackExecuted() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  if (mExecutionStatus == ExecutionStatus::CALLBACK_EXECUTED) {
    return true;
  }
  else {
    return false;
  }
}

bool Message::isCallbackExecuting() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  if (mExecutionStatus == ExecutionStatus::CALLBACK_INPROGRESS) {
    return true;
  }
  else {
    return false;
  }
}

bool Message::isExpired() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  if (mExecutionStatus == ExecutionStatus::CANCELLATION_INPROGRESS ||
      mExecutionStatus == ExecutionStatus::CANCELLED) {
    return true;
  } else {
    return false;
  }
}

void Message::setTimerId(TimeKeeper::timer_id tid) {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mTid = tid;
}

/* by default, only when they point to the same area will it return true */
bool Message::isSameMessage(std::shared_ptr<Message> msg) {
  return (this == msg.get());
}

TimeKeeper::timer_id Message::getTimerId() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  return mTid;
}

void Message::informDispatchFailure(std::shared_ptr<Message> solicitedMsg,
                           Message::Callback::Status status) {
  lock_guard<std::recursive_mutex> lock(mMutex);
  if (mCallback != nullptr) {
    mCallback->handleAsyncResponse(solicitedMsg, status, nullptr);
  }
  // Dispatcher::getInstance().informMessageCallbackFinished(solicitedMsg);
}

void Message::setMessageExpiryTimer(TimeKeeper::millisec timeOut) {
  lock_guard<std::recursive_mutex> lock(mMutex);
  mExpiryTimer = timeOut;
}

TimeKeeper::millisec Message::getMessageExpiryTimer() {
  lock_guard<std::recursive_mutex> lock(mMutex);
  return mExpiryTimer;
}
