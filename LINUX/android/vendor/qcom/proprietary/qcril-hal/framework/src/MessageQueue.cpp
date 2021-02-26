/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <mutex>

#define TAG "RILQ"
#include "framework/Log.h"
#include "framework/MessageQueue.h"

using std::mutex;
using std::lock_guard;

void MessageQueue::enqueue(std::shared_ptr<Message> msg) {
  if(msg == nullptr){
    QCRIL_LOG_DEBUG("[%s]: null message received .. Not enqueuing",
        to_string().c_str());
    return;
  }
  QCRIL_LOG_DEBUG("[%s]: enqueuing message %p. Id: %s", to_string().c_str(),
      msg.get(), msg->get_message_name().c_str());
  lock_guard<mutex> lock(mMutex);
  mMessages.push_back(msg);
  mMessages.shrink_to_fit();
}

size_t MessageQueue::getSize() {
  lock_guard<mutex> lock(mMutex);
  return mMessages.size();
}

bool MessageQueue::isEmpty() {
  lock_guard<mutex> lock(mMutex);
  return mMessages.empty();
}

void MessageQueue::dumpMessageQueue() {
  lock_guard<mutex> lock(mMutex);
  for (auto& elem : mMessages) {
#ifndef QMI_RIL_UTF
    QCRIL_LOG_DEBUG("[%s]: dequeued message %p", to_string().c_str(), elem.get());
#endif
  }
}

void MessageQueue::clear() {
  lock_guard<mutex> lock(mMutex);
  mMessages.clear();
}

std::shared_ptr<Message> MessageQueue::pop() {
  if(getSize() > 0) {
    lock_guard<mutex> lock(mMutex);
    std::shared_ptr<Message> msg = mMessages.front();
    QCRIL_LOG_DEBUG("[%s]: msg: %p", to_string().c_str(), msg.get());
    mMessages.pop_front();
    mMessages.shrink_to_fit();
    return msg;
  } else {
      return nullptr;
  }
}
