/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <vector>

#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/VoiceModemEndPointModule.h"
#include "framework/Log.h"

class VoiceModemEndPoint : public ModemEndPoint {
 public:
  static constexpr const char *NAME = "VOICE";
  VoiceModemEndPoint() : ModemEndPoint(NAME) {
    mModule = new VoiceModemEndPointModule("VoiceModemEndPointModule", *this);
    mModule->init();
  }
  ~VoiceModemEndPoint() {
      Log::getInstance().d("[VoiceModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }

  void requestSetup(string clientToken, GenericCallback<string>* cb);

  Message::Callback::Status getOperatingModeSync(
      std::shared_ptr<int>& operatingMode);
};
