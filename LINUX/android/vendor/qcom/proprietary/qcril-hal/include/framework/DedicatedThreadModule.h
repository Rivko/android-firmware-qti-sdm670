/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include "framework/DedicatedThreadMessage.h"

class DedicatedThreadModule : public Module {
 public:
  DedicatedThreadModule();
  ~DedicatedThreadModule();
  void init();

 private:
  void handleDedicatedThreadMessage(std::shared_ptr<DedicatedThreadMessage> msg);
};
