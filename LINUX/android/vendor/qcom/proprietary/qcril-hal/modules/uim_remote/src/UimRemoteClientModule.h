/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include <framework/QcrilInitMessage.h>
#include "modules/uim_remote/UimRmtQmiUimRemoteIndMsg.h"
#include "UimRmtQmiUimRemoteRespMsg.h"
#include "uim_remote_client_service.h"
#include "qcril_uim_remote.h"

using namespace vendor::qti::hardware::radio::uim_remote_client::V1_0::implementation;

class UimRemoteClientModule : public Module
{
  public:
    UimRemoteClientModule();
    ~UimRemoteClientModule();

    void init();

    bool isReady();

    void qcril_uim_remote_client_request_event(int32_t token, qcril_uim_remote_event_req_type *user_req_ptr);
    void qcril_uim_remote_client_request_apdu(int32_t token, qcril_uim_remote_apdu_status apdu_status,
                                              uint32_t  apdu_data_len, const uint8_t *apdu_ptr);

  private:
    bool mReady;
    uint32_t mApduId;
    sp<UimRemoteClientImpl> uimRemoteClient;
    void broadcastReady();

    void handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg);
    void handleUimRemoteRespMessage(std::shared_ptr<UimRmtQmiUimRemoteRespMsg> msg);
    void handleUimRemoteIndMessage(std::shared_ptr<UimRmtQmiUimRemoteIndMsg> msg);
};
