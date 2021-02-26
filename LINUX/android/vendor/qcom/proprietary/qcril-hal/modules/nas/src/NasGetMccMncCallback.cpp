/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/qcril_qmi_nas.h"
#include "framework/Log.h"
#include "framework/legacy.h"
#include "modules/nas/NasGetMccMncCallback.h"

void NasGetMccMncCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Status status,
    std::shared_ptr<qcril_mcc_mnc_info_type> responseDataPtr) {
  mStatus = status;
  if (mStatus == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[NasGetMccMncCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " status = Message::Callback::Status::SUCCESS");
    mResponseData = responseDataPtr;
    std::shared_ptr<qcril_mcc_mnc_info_type> resp = std::static_pointer_cast<qcril_mcc_mnc_info_type>(responseDataPtr);
    if(resp) {
      qcril_request_params_type params_ptr;
      qcril_request_return_type ret_ptr;
      std::memset(&params_ptr, 0, sizeof(params_ptr));
      params_ptr.modem_id = QCRIL_DEFAULT_MODEM_ID;
      params_ptr.data = static_cast<void*>(new char[sizeof(qcril_mcc_mnc_info_type)]());
      if(params_ptr.data != nullptr) {
          memcpy(params_ptr.data, resp.get(), sizeof(qcril_mcc_mnc_info_type));
      }
      qcril_uim_process_mcc_mnc_info(&params_ptr, &ret_ptr);
    } else {
      Log::getInstance().d("[NasGetMccMncCallback]: resp is NULL..not processing ");
    }
} else {
    Log::getInstance().d("[NasGetMccMncCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " FAILURE !!");
  }
}

Message::Callback::Status NasGetMccMncCallback::getStatus() {
  if (mStatus == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[NasGetMccMncCallback]: Message::Callback::Status::SUCCESS");
  } else {
    Log::getInstance().d("[NasGetMccMncCallback]: not SUCCESS");
  }
  return mStatus;
}

Message::Callback *NasGetMccMncCallback::clone() {
  NasGetMccMncCallback *clone = new NasGetMccMncCallback(mClientToken);
  return clone;
}
