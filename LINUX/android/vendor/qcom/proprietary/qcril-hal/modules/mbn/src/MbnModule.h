/******************************************************************************
#  @file    MbnModule.h
#  @brief   Header file for MbnModule Class. Provides interface to read and update
#           mbn.
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
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/PdcModemEndPoint.h"
#include "modules/nas/NasFlexMapStatusIndMessage.h"

#include "qcril_qmi_pdc.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "qcril_mbn_hw_update.h"
#include "qcril_mbn_sw_update.h"


//TODO add dmsendpoint

class MbnModule : public Module {
public:
    static constexpr const int QCRIL_MBN_SW_MAX_INSTANCE_ID = 4;

protected:
    int mMaxApssInstanceId = 1;
    string mSwConfigId;
    string mSwActiveConfigId;
    int mSwMbnCurInstanceId;
    int mSwMbnCurSubId;
    bool mSwMbnNeedRestart;
    //TODO add remaining global var/structures
    bool mReady;
    bool mWaitingForDmsClient;
    bool mDmsEndPointStatus;

public:
    MbnModule();
    ~MbnModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalPdcModuleCleanup();
#endif
protected:
    void handlePdcQmiIndMessage(std::shared_ptr<Message> msg);
    void handleRilInit(std::shared_ptr<Message> msg);
    void handlePdcEndpointStatusIndMessage(std::shared_ptr<Message> msg);
    void handleFlexMapStatusIndication(std::shared_ptr<Message> msg);
    void handleFileObserverEvents(std::shared_ptr<Message> msg);
    void verifySwMbnUpdateHelper(std::shared_ptr<Message> msg);
    qcril_mbn_sw_modem_switch_state processFlexMapStatus(NasFlexmapState state);

    // Handler for DMS Endpoint Status Indications
    void handleDmsEndpointStatusIndMessage(std::shared_ptr<Message> msg);

};

MbnModule *getMbnModule();
