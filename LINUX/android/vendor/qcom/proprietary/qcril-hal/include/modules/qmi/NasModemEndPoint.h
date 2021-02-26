/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/Log.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/NasModemEndPointModule.h"

class NasModemEndPoint : public ModemEndPoint {
  public:
    static constexpr const char *NAME = "NAS";
    NasModemEndPoint() : ModemEndPoint(NAME) {
      mModule = new NasModemEndPointModule("NasModemEndPointModule", *this);
      mModule->init();
    }
    ~NasModemEndPoint() {
      Log::getInstance().d("[NasModemEndPoint]: destructor");
      delete mModule;
      mModule = nullptr;
    }

    void requestSetup(string clientToken, GenericCallback<string>* cb);
};
