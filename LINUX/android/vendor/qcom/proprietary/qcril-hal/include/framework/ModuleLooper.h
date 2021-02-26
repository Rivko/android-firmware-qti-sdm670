/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Looper.h"
class ModuleLooper : public Looper {
 public:
  void handleMessage(std::shared_ptr<Message> msg) override;
  void handleMessageSync(std::shared_ptr<Message> msg) override;
};
