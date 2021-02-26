/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <deque>
#include <memory>
#include <deque>

#include "framework/Restriction.h"
class SingleDispatchRestrictionHandleDuplicate : public Restriction {
 public:
  SingleDispatchRestrictionHandleDuplicate();
  ~SingleDispatchRestrictionHandleDuplicate();

  bool isDispatchAllowed(std::shared_ptr<Message> msg);
  void onMessageCallbackCompletion(std::shared_ptr<Message> msg);
  void abandonPendingMessages();

  string to_string();

 private:
  std::mutex mMutex;
  std::shared_ptr<Message> mHandlingMsg;
  std::shared_ptr<Message> mRedispatchedMsg;
  /* Map of restricted messages for which callback are not yet executed.
  Key = Message string, Value = Deque of client message shared pointer*/
  std::deque<std::shared_ptr<Message> > mRestrictedMessagesQueue;
};
