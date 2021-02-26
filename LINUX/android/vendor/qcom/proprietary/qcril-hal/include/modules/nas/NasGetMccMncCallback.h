/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <modules/uim/UimGetMccMncRequestMsg.h>
#include <framework/GenericCallback.h>

class NasGetMccMncCallback : public GenericCallback<qcril_mcc_mnc_info_type> {
  public:
    inline NasGetMccMncCallback(string clientToken)
      : GenericCallback(clientToken){};

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<qcril_mcc_mnc_info_type> responseDataPtr) override;

    Status getStatus();

    Status mStatus;
    std::shared_ptr<void> mResponseData;
};
