/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <vector>

#include "qmi_client.h"

#include "modules/qmi/DmsModemEndPointModule.h"
#include "modules/qmi/ModemEndPoint.h"
#include "framework/Log.h"

class DmsModemEndPoint : public ModemEndPoint {
 public:
  DmsModemEndPoint() :ModemEndPoint("DMS") {
    mModule = new DmsModemEndPointModule("DmsModemEndPointModule", *this);
    mModule->init();
  }
  ~DmsModemEndPoint() {
      Log::getInstance().d("[DmsModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }

  void requestSetup(string clientToken, GenericCallback<string>* cb);

  Message::Callback::Status getOperatingModeSync(
      std::shared_ptr<int>& operatingMode);
};
