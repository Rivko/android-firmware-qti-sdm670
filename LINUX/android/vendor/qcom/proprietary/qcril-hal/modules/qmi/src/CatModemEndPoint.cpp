/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "modules/qmi/CatModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiCatSetupRequest.h"
#include "modules/qmi/QmiCatSetupRequestSync.h"

#include "CatModemEndPointModule.h"

CatModemEndPoint::CatModemEndPoint() : ModemEndPoint("Cat")
{
    mModule = new CatModemEndPointModule("CatModemEndPointModule", this);

    if (mModule != nullptr)
    {
      mModule->init();
    }
}

void CatModemEndPoint::requestSetup
(
  string clientToken,
  GenericCallback<string>* callback
)
{
  auto setupMsg = std::make_shared<QmiCatSetupRequest>(clientToken, 0, nullptr, callback);
  setupMsg->dispatch();
}


Message::Callback::Status CatModemEndPoint::requestSetupSync
(
  std::shared_ptr<string>& sharedResponse
)
{
  if (getState() != ModemEndPoint::State::OPERATIONAL)
  {
    auto setupMsg = std::make_shared<QmiCatSetupRequestSync>("", 0, nullptr, nullptr);
    return (setupMsg->dispatchSync(sharedResponse));
  }
  else
  {
    sharedResponse = std::make_shared<string>("Service already operational");
    return Message::Callback::Status::SUCCESS;
  }
}

