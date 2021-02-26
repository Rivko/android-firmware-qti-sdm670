/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <deque>
#include <mutex>

#include "framework/Message.h"

using std::deque;

class Message;
class MessageQueue {
 protected:
  string mName;
  std::mutex mMutex;
  deque<std::shared_ptr<Message> > mMessages;

 public:
  MessageQueue();
  virtual ~MessageQueue() {}
  virtual void init(string name) { mName = name + "-MessageQueue"; }
  virtual void enqueue(std::shared_ptr<Message> msg);
  virtual size_t getSize();
  virtual void dumpMessageQueue();
  virtual bool isEmpty();
  virtual void clear();
  virtual std::shared_ptr<Message> pop();
  string to_string();

  // void dump();
};

inline MessageQueue::MessageQueue() = default;
inline string MessageQueue::to_string() { return mName; }
