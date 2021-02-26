/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <memory>
#include "framework/Message.h"

class Restriction {
 public:
  Restriction();
  virtual ~Restriction() = 0;

  virtual bool isDispatchAllowed(std::shared_ptr<Message> msg) = 0;
  virtual void onMessageCallbackCompletion(std::shared_ptr<Message> msg) = 0;
  virtual void abandonPendingMessages() {}

  virtual string to_string() = 0;

 protected:
  string mName;
};
