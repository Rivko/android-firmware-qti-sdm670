/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include "modules/android/LegacyHandlerMessage.h"

class LegacyHandlerSubModule : public Module {
 public:
  LegacyHandlerSubModule();
  ~LegacyHandlerSubModule();

 private:
  void handleLegacyHandlerMessage(std::shared_ptr<LegacyHandlerMessage> msg);
};
