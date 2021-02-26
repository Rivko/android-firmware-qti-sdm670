/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/DefaultRestriction.h"
#include "framework/Log.h"

DefaultRestriction::DefaultRestriction() { mName = "DefaultRestriction"; }

DefaultRestriction::~DefaultRestriction() {}

bool DefaultRestriction::isDispatchAllowed(std::shared_ptr<Message> msg) {
  (void)msg;
  Log::getInstance().d("[" + mName + "]: dispatch allowed");
  return true;
}

void DefaultRestriction::onMessageCallbackCompletion(
    std::shared_ptr<Message> msg) {
  (void)msg;
}

string DefaultRestriction::to_string() { return mName; }
