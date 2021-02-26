/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/qcril_qmi_nas.h"
#include "framework/Log.h"
#include "framework/legacy.h"
#include "modules/ims/ImsSetVoiceDomainPreferenceFollowupMessage.h"
#include "ImsSetVoiceDomainPreferenceRequestCallback.h"

void ImsSetVoiceDomainPreferenceRequestCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Status status,
    std::shared_ptr<RIL_Errno> responseDataPtr) {
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  mStatus = status;

  if (mStatus == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[ImsSetVoiceDomainPreferenceRequestCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " status = Message::Callback::Status::SUCCESS");
    mResponseData = responseDataPtr;
  } else {
    Log::getInstance().d("[ImsSetVoiceDomainPreferenceRequestCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " FAILURE !!");
    mResponseData = nullptr;
  }

  if (mResponseData != nullptr) {
    result = *mResponseData;
  }

  auto followupMsg = std::make_shared<ImsSetVoiceDomainPreferenceFollowupMessage>(mReqId);
  if (followupMsg != nullptr) {
    followupMsg->setResult(result);
    followupMsg->dispatch();
    followupMsg = nullptr;
  }
}

Message::Callback::Status ImsSetVoiceDomainPreferenceRequestCallback::getStatus() {
  if (mStatus == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[ImsSetVoiceDomainPreferenceRequestCallback]: Message::Callback::Status::SUCCESS");
  } else {
    Log::getInstance().d("[ImsSetVoiceDomainPreferenceRequestCallback]: not SUCCESS");
  }
  return mStatus;
}

Message::Callback *ImsSetVoiceDomainPreferenceRequestCallback::clone() {
  ImsSetVoiceDomainPreferenceRequestCallback *clone = new ImsSetVoiceDomainPreferenceRequestCallback(mClientToken, mReqId);
  return clone;
}
