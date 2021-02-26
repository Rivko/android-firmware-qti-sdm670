/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include "modules/sample/SampleMessage.h"

class SampleModule : public Module {
 public:
  SampleModule();
  ~SampleModule();
  void init();

private:
  void handleSampleMessage(std::shared_ptr<SampleMessage> msg);
};
