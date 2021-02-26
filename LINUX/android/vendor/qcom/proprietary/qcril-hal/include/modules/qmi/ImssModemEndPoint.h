/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/Log.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ImssModemEndPointModule.h"

class ImssModemEndPoint : public ModemEndPoint {
  public:
    static constexpr const char *NAME = "IMSS";

    ImssModemEndPoint() : ModemEndPoint(NAME) {
      mModule = new ImssModemEndPointModule("ImssModemEndPointModule", *this);
      mModule->init();
    }

    ~ImssModemEndPoint() {
      Log::getInstance().d("[" + mName + "]: destructor");
      delete mModule;
      mModule = nullptr;
    }

    void requestSetup(string clientToken, GenericCallback<string>* cb);
};
