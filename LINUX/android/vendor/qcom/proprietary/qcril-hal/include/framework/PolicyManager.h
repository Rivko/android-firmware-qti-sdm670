/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "framework/Message.h"
#include "framework/Restriction.h"

class PolicyManager {
 public:
  inline ~PolicyManager(){};

  PolicyManager(PolicyManager const &) = delete;             // Copy construct
  PolicyManager(PolicyManager &&) = delete;                  // Move construct
  PolicyManager &operator=(PolicyManager const &) = delete;  // Copy assign
  PolicyManager &operator=(PolicyManager &&) = delete;       // Move assign

  static PolicyManager &getInstance();

  std::shared_ptr<Restriction> getMessageRestriction(
      std::shared_ptr<Message> msg);
  std::shared_ptr<Restriction> getMessageRestriction(string msgName);
  void setMessageRestriction(std::vector<std::string> msgNameList,
                             std::shared_ptr<Restriction> restriction);
  inline void reset() {
      mMessageRestrictions.clear();
  }
 private:
  std::mutex mMutex;
  inline PolicyManager() {}
  std::unordered_map<std::string, std::shared_ptr<Restriction> >
      mMessageRestrictions;
};
