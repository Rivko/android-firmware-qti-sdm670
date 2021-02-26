/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <mutex>

#define TAG "RILQ"
#include "framework/Dispatcher.h"
#include "framework/Log.h"
#include "framework/QcrilMainMessageQueue.h"

using std::lock_guard;
using std::mutex;

void QcrilMainMessageQueue::enqueue(std::shared_ptr<Message> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("[%s]: null message received .. Not enqueuing", to_string().c_str());
    return;
  }

  /* Treat the message as HIGH priority if all of the modules which
   * registers for this message have dedicated looper */
  bool isPriority = Dispatcher::getInstance().isAllMessageHandlerModulesWithLooper(msg);

  QCRIL_LOG_DEBUG("[%s]: enqueuing priority(%s) message %p Id: %s", to_string().c_str(),
                  isPriority ? "HIGH" : "NORMAL", msg.get(), msg->get_message_name().c_str());

  lock_guard<mutex> lock(mMutex);
  if (isPriority) {
    mPriorityMessages.push_back(msg);
    mPriorityMessages.shrink_to_fit();
  } else {
    mMessages.push_back(msg);
    mMessages.shrink_to_fit();
  }
}

size_t QcrilMainMessageQueue::getSize() {
  lock_guard<mutex> lock(mMutex);
  return (mMessages.size() + mPriorityMessages.size());
}

bool QcrilMainMessageQueue::isEmpty() {
  lock_guard<mutex> lock(mMutex);
  return (mMessages.empty() && mPriorityMessages.empty());
}

void QcrilMainMessageQueue::dumpMessageQueue() {
  lock_guard<mutex> lock(mMutex);
  for (auto &elem : mPriorityMessages) {
#ifndef QMI_RIL_UTF
    QCRIL_LOG_DEBUG("[%s]: dump message %p", to_string().c_str(), elem.get());
#endif
  }
  for (auto &elem : mMessages) {
#ifndef QMI_RIL_UTF
    QCRIL_LOG_DEBUG("[%s]: dump message %p", to_string().c_str(), elem.get());
#endif
  }
}

void QcrilMainMessageQueue::clear() {
  lock_guard<mutex> lock(mMutex);
  mPriorityMessages.clear();
  mMessages.clear();
}

std::shared_ptr<Message> QcrilMainMessageQueue::pop() {
  std::shared_ptr<Message> msg = nullptr;
  {
    lock_guard<mutex> lock(mMutex);
    if (mPriorityMessages.size() > 0) {
      msg = mPriorityMessages.front();
      mPriorityMessages.pop_front();
      mPriorityMessages.shrink_to_fit();
    } else if (mMessages.size() > 0) {
      msg = mMessages.front();
      mMessages.pop_front();
      mMessages.shrink_to_fit();
    }
  }
  QCRIL_LOG_DEBUG("[%s]: msg: %p", to_string().c_str(), msg.get());
  return msg;
}
