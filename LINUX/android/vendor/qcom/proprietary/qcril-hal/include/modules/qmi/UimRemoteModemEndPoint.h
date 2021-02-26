/******************************************************************************
#  @file    UimRemoteModemEndPoint.h
#  @brief   Base class header file for QMI UIM module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#pragma once
#include <vector>

#include "modules/qmi/ModemEndPoint.h"
#include "framework/Log.h"

class UimRemoteModemEndPoint : public ModemEndPoint {
 public:
  UimRemoteModemEndPoint();
  ~UimRemoteModemEndPoint() {
      Log::getInstance().d("[UimRemoteModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }

  void requestSetup(string clientToken, GenericCallback<string>* cb);
  Message::Callback::Status requestSetupSync(std::shared_ptr<string>& sharedResponse);
};
