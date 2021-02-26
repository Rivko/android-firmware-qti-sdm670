/******************************************************************************
#  @file    UimHttpModemEndPoint.cpp
#  @brief   Base class source file for QMI UIM HTTP module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "modules/qmi/UimHttpModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiUimHttpSetupRequest.h"

#include "UimHttpModemEndPointModule.h"

using std::to_string;

UimHttpModemEndPoint::UimHttpModemEndPoint() : ModemEndPoint("UimHttp")
{
  mModule = new UimHttpModemEndPointModule("UimHttpModemEndPointModule", this);

  if (mModule != nullptr)
  {
    mModule->init();
  }
}

void UimHttpModemEndPoint::requestSetup
(
  string clientToken,
  GenericCallback<string>* callback
)
{
  auto setupMsg = std::make_shared<QmiUimHttpSetupRequest>(clientToken, 0, nullptr, callback);

  if (setupMsg != nullptr)
  {
    setupMsg->dispatch();
  }
}
