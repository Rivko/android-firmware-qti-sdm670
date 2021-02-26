/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <deque>
#include <memory>
#include <unordered_map>

#include "framework/Restriction.h"
class RejectSimultaneousDispatchRestriction : public Restriction {
 public:
  RejectSimultaneousDispatchRestriction();
  ~RejectSimultaneousDispatchRestriction();

  bool isDispatchAllowed(std::shared_ptr<Message> msg);
  void onMessageCallbackCompletion(std::shared_ptr<Message> msg);

  string to_string();

 private:
  std::mutex mMutex;
  bool allowNextMessage;
};
