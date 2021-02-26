/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/GenericCallback.h>
#include <modules/android/RilRequestMessage.h>

//TODO: Need to move the callbacks to internal location and remove this file.

class NasDataSetLteAttachProfileCallback : public GenericCallback<RIL_Errno> {
private:
        legacy_request_payload params;
  public:
    inline NasDataSetLteAttachProfileCallback(string clientToken,
        const qcril_request_params_type &p) : GenericCallback(clientToken), params(p) {};

    inline ~NasDataSetLteAttachProfileCallback() {};

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<RIL_Errno> responseDataPtr) override;

    qcril_request_params_type &get_params() {
        return params.get_params();
    }
};

class NasDataGetDdsSubIdCallback : public GenericCallback<DDSSubIdInfo> {
private:
        legacy_request_payload params;
        boolean from_allow_data;
        boolean from_set_initial_attch_apn;
  public:
    inline NasDataGetDdsSubIdCallback(string clientToken,
        const qcril_request_params_type &p, boolean from_allow_data, boolean from_set_initial_attch_apn) :
          GenericCallback(clientToken), params(p), from_allow_data(from_allow_data), from_set_initial_attch_apn(from_set_initial_attch_apn) {};

    inline ~NasDataGetDdsSubIdCallback() {};

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<DDSSubIdInfo> responseDataPtr) override;

    qcril_request_params_type &get_params() {
        return params.get_params();
    }
};

class NasDataRequestDdsSwitchCallback : public GenericCallback<RIL_Errno> {
private:
        legacy_request_payload params;
  public:
    inline NasDataRequestDdsSwitchCallback(string clientToken,
        const qcril_request_params_type &p) :
          GenericCallback(clientToken), params(p){};

    inline ~NasDataRequestDdsSwitchCallback() {};

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<RIL_Errno> responseDataPtr) override;

    qcril_request_params_type &get_params() {
        return params.get_params();
    }
};
