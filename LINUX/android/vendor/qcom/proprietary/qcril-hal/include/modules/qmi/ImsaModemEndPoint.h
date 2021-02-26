/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/Log.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ImsaModemEndPointModule.h"

class ImsaModemEndPoint : public ModemEndPoint {
  public:
    static constexpr const char *NAME = "IMSA";

    ImsaModemEndPoint() : ModemEndPoint(NAME) {
      mModule = new ImsaModemEndPointModule("ImsaModemEndPointModule", *this);
      mModule->init();
    }

    ~ImsaModemEndPoint() {
      Log::getInstance().d("[" + mName + "]: destructor");
      delete mModule;
      mModule = nullptr;
    }

    void requestSetup(string clientToken, GenericCallback<string>* cb);
};
