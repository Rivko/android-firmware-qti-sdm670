/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/PolicyManager.h"
#include <iostream>
#include "framework/DefaultRestriction.h"
#include "framework/Log.h"

using std::lock_guard;
using std::mutex;

PolicyManager &PolicyManager::getInstance() {
  static PolicyManager sInstance;
  return sInstance;
}

void PolicyManager::setMessageRestriction(
    std::vector<std::string> msgNameList,
    std::shared_ptr<Restriction> restriction) {
  if (restriction) {
    lock_guard<mutex> lock(mMutex);
    /* Overwrite last restriction for this msg.*/
    // Copy operation.
    for (const auto &msgName : msgNameList) {
      if (mMessageRestrictions.find(msgName) == mMessageRestrictions.end()) {
        mMessageRestrictions[msgName] = restriction;
      } else {
        /* Conflicting restrictions on a msg is not allowed. Existing restriction
         * is retained.*/
        std::shared_ptr<Restriction> existingRestriction =
            mMessageRestrictions[msgName];
        Log::getInstance().d(
            "[PolicyManager]: ERROR: Conflicting restriction "
            "overide not allowed current restriciton " +
            existingRestriction->to_string() + " for msg = " + msgName +
            " with " + restriction->to_string());
      }
    }
  }
}

std::shared_ptr<Restriction> PolicyManager::getMessageRestriction(
    std::shared_ptr<Message> msg) {
  lock_guard<mutex> lock(mMutex);
  string msgName{msg->to_string()};
  if (mMessageRestrictions.size() == 0
      || mMessageRestrictions.find(msgName) == mMessageRestrictions.end()) {
    /* Not found. No restriction.*/
    return std::shared_ptr<DefaultRestriction>(new DefaultRestriction);
  } else {
    std::shared_ptr<Restriction> restriction = mMessageRestrictions[msgName];
    // Log::getInstance().d("[PolicyManager]: Restriction for msg = " +
    // msg->dump() + " restriction = " + restriction->to_string());
    return restriction;
  }
}

std::shared_ptr<Restriction> PolicyManager::getMessageRestriction(string msgName) {
  lock_guard<mutex> lock(mMutex);
  if (mMessageRestrictions.size() == 0
      || mMessageRestrictions.find(msgName) == mMessageRestrictions.end()) {
    /* Not found. No restriction.*/
    return std::shared_ptr<DefaultRestriction>(new DefaultRestriction);
  } else {
    std::shared_ptr<Restriction> restriction = mMessageRestrictions[msgName];
    return restriction;
  }
}
