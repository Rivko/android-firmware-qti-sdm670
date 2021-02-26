/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <deque>
#include <memory>
#include <deque>
#include <unordered_map>

#include "framework/Restriction.h"
class SingleDispatchAndFamilyPairRestriction : public Restriction {
 public:
  SingleDispatchAndFamilyPairRestriction();
  ~SingleDispatchAndFamilyPairRestriction();

  bool isDispatchAllowed(std::shared_ptr<Message> msg);
  void onMessageCallbackCompletion(std::shared_ptr<Message> msg);
  void abandonPendingMessages();
  bool setMessagePair(std::string msgStr1, std::string msgStr2);

  string to_string();

 private:
  std::mutex mMutex;
  std::shared_ptr<Message> mHandlingMsg;
  std::shared_ptr<Message> mRedispatchedMsg;
  /* Map of restricted messages for which callback are not yet executed.
  Key = Message string, Value = Deque of client message shared pointer*/
  std::deque<std::shared_ptr<Message> > mRestrictedMessagesQueue;
  std::unordered_map<std::string, std::string> mPairMessages;
};
