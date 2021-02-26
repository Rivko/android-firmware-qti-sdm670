/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Restriction.h"
/* Default restriction is NO restriction on message dispatch.*/
class DefaultRestriction : public Restriction {
 public:
  DefaultRestriction();
  ~DefaultRestriction();

  bool isDispatchAllowed(std::shared_ptr<Message> msg);
  void onMessageCallbackCompletion(std::shared_ptr<Message> msg);
  string to_string();
};
