/******************************************************************************
#  @file    UimModemEndPoint.cpp
#  @brief   Base class source file for QMI UIM module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "modules/qmi/UimModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiUimSetupRequest.h"
#include "modules/qmi/QmiUimSetupRequestSync.h"
#include "UimModemEndPointModule.h"

UimModemEndPoint::UimModemEndPoint() : ModemEndPoint("Uim")
{
    mModule = new UimModemEndPointModule("UimModemEndPointModule", this);

    if (mModule != nullptr)
    {
      mModule->init();
    }
}

void UimModemEndPoint::requestSetup
(
  string clientToken,
  GenericCallback<string>* callback
)
{
  auto setupMsg = std::make_shared<QmiUimSetupRequest>(clientToken, 0, nullptr, callback);
  setupMsg->dispatch();
}


Message::Callback::Status UimModemEndPoint::requestSetupSync
(
  std::shared_ptr<string>& sharedResponse
)
{
  if (getState() != ModemEndPoint::State::OPERATIONAL)
  {
    auto setupMsg = std::make_shared<QmiUimSetupRequestSync>("", 0, nullptr, nullptr);
    return (setupMsg->dispatchSync(sharedResponse));
  }
  else
  {
    sharedResponse = std::make_shared<string>("Service already operational");
    return Message::Callback::Status::SUCCESS;
  }
}
