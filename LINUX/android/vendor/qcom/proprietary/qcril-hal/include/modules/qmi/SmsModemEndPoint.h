/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Log.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/SmsModemEndPointModule.h"

class SmsModemEndPoint : public ModemEndPoint {
  public:
    static constexpr const char *NAME = "SMS";
    SmsModemEndPoint() : ModemEndPoint(NAME) {
      mModule = new SmsModemEndPointModule("SmsModemEndPointModule", *this);
      mModule->init();
    }

    ~SmsModemEndPoint() {
      Log::getInstance().d("[SmsModemEndPoint]: destructor");
      delete mModule;
      mModule = nullptr;
    }

    void requestSetup(string clientToken, GenericCallback<string>* cb);
};
