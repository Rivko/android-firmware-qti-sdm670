/******************************************************************************
#  @file    UimRemoteModemEndPoint.cpp
#  @brief   Base class source file for QMI UIM Remote module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "modules/qmi/UimRemoteModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiUimRemoteSetupRequest.h"

#include "UimRemoteModemEndPointModule.h"

using std::to_string;

UimRemoteModemEndPoint::UimRemoteModemEndPoint() : ModemEndPoint("UimRemote")
{
    mModule = new UimRemoteModemEndPointModule("UimRemoteModemEndPointModule", this);
    mModule->init();
}

void UimRemoteModemEndPoint::requestSetup
(
  string clientToken,
  GenericCallback<string>* callback
)
{
  auto setupMsg = std::make_shared<QmiUimRemoteSetupRequest>(clientToken, 0, nullptr, callback);
  setupMsg->dispatch();
}
