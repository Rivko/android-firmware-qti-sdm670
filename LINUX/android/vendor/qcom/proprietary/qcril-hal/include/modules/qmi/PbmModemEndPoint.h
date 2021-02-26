/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Log.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/PbmModemEndPointModule.h"

class PbmModemEndPoint : public ModemEndPoint {
  public:
    static constexpr const char *NAME = "PBM";
    PbmModemEndPoint() : ModemEndPoint(NAME) {
      mModule = new PbmModemEndPointModule("PbmModemEndPointModule", *this);
      mModule->init();
    }
    ~PbmModemEndPoint() {
      Log::getInstance().d("[PbmModemEndPoint]: destructor");
      delete mModule;
      mModule = nullptr;
    }

    void requestSetup(string clientToken, GenericCallback<string>* cb);
};
