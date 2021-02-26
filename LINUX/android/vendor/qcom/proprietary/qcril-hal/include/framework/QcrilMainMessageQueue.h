/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Message.h"
#include "framework/MessageQueue.h"

class QcrilMainMessageQueue : public MessageQueue {
 private:
  deque<std::shared_ptr<Message> > mPriorityMessages;

 public:
  QcrilMainMessageQueue() {}
  ~QcrilMainMessageQueue() {}
  void enqueue(std::shared_ptr<Message> msg);
  size_t getSize();
  void dumpMessageQueue();
  bool isEmpty();
  void clear();
  std::shared_ptr<Message> pop();
};
