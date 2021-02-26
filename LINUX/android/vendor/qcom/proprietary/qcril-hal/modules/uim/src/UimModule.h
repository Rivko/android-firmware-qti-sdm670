/******************************************************************************
#  @file    UimModem.h
#  @brief   Base class header file for UIM module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include <queue>
#include "modules/qmi/UimModemEndPoint.h"
#include "modules/uim/UimCardPowerReqMsg.h"
#include "modules/uim/UimChangeSubscriptionReqMsg.h"
#include "modules/uim/UimGetAtrRequestMsg.h"
#include "modules/uim/UimGetCardStatusRequestSyncMsg.h"
#include "modules/uim/UimGetIccIdRequestSyncMsg.h"
#include "modules/uim/UimGetImsiRequestSyncMsg.h"
#include "modules/uim/UimGetCardStatusRequestMsg.h"
#include "modules/uim/UimGetIccIdRequestMsg.h"
#include "modules/uim/UimGetImsiRequestMsg.h"
#include "modules/uim/UimGetMccMncRequestMsg.h"
#include "modules/uim/UimGetFacilityLockRequestMsg.h"
#include "modules/uim/UimSetFacilityLockRequestMsg.h"
#include "modules/uim/UimRilRequestMessage.h"
#include "modules/uim/UimLpaReqMessage.h"
#include "modules/uim/UimInitMessage.h"
#include "modules/uim/UimQmiUimIndicationMsg.h"
#include "modules/uim/UimQmiUimRequestMsg.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "UimQmiUimResponseMsg.h"
#include "UimOemHookRequestMsg.h"


class UimModule : public Module
{
  private:
    bool                                              mReady;
    bool                                              mRestart;
    uint8_t                                           mQmiAsyncMsgCount;
    std::queue<std::shared_ptr<UimQmiUimRequestMsg> > mQmiPendingList;

  public:
    UimModule();
    ~UimModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalUimModuleCleanup();
#endif
    int UimSendQmiUimRequest
    (
      uint32                        msg_id,
      const void                  * params,
      void                        * rsp_data_ptr,
      void                        * original_request_ptr,
      qmi_uim_user_async_cb_type    cb_ptr
    );

  private:
    void UimProcessRilInit(std::shared_ptr<Message> msg_ptr);
    void uim_process_ril_request(std::shared_ptr<UimRilRequestMessage> msg);
    void UimProcessQmiCommandCallback(std::shared_ptr<UimQmiUimResponseMsg> msg);
    void UimProcessQmiIndiaction(std::shared_ptr<UimQmiUimIndicationMsg> msg);
    void UimProcessCardPowerReq(std::shared_ptr<UimCardPowerReqMsg> msg);
    void UimProcessChangeSubscription(std::shared_ptr<UimChangeSubscriptionReqMsg> msg);
    void UimProcessGetImsiSyncReq(std::shared_ptr<UimGetImsiRequestSyncMsg> msg);
    void UimProcessGetCardStatusSyncReq(std::shared_ptr<UimGetCardStatusRequestSyncMsg> msg);
    void UimProcessGetIccIdSycnReq(std::shared_ptr<UimGetIccIdRequestSyncMsg> msg);
    void UimProcessGetImsiReq(std::shared_ptr<UimGetImsiRequestMsg> msg);
    void UimProcessGetCardStatusReq(std::shared_ptr<UimGetCardStatusRequestMsg> msg);
    void UimProcessGetIccIdReq(std::shared_ptr<UimGetIccIdRequestMsg> msg);
    void UimProcessGetMccMncReq(std::shared_ptr<UimGetMccMncRequestMsg> msg);
    void UimProcessGetFacilityLockStatusReq(std::shared_ptr<UimGetFacilityLockRequestMsg> msg);
    void UimProcessSetFacilityLockReq(std::shared_ptr<UimSetFacilityLockRequestMsg> msg);
    void uim_process_oem_hook_request(std::shared_ptr<UimOemHookRequestMsg> msg);
    void uim_process_lpa_request(std::shared_ptr<UimLpaReqMessage> msg);
    void UimProcessGetAtrReq(std::shared_ptr<UimGetAtrRequestMsg> msg);
};

UimModule *getUimModule();
