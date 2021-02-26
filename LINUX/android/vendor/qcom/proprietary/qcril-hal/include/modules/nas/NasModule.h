/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "modules/qmi/NasModemEndPoint.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"

#include "modules/nas/NasUimHeaders.h"
#include "modules/nas/NasSetInitialAttachApnRequestMessage.h"
#include "modules/nas/NasSetNetworkSelectionManualRequestMessage.h"
#include "modules/nas/NasRequestDataShutdownMessage.h"

#include <modules/android/RilRequestMessage.h>
#include <modules/ims/ImsServiceStatusInd.h>
#include <modules/ims/ImsWfcSettingsStatusInd.h>
#include "modules/nas/NasSetVoiceDomainPreferenceRequest.h"

//Messages from Data
#include "modules/nas/NasEmbmsEnableDataConMessage.h"
#include "modules/nas/NasEmbmsDisableDataMessage.h"

//Data Unsol Messages
#include "modules/nas/NasDataCache.h"
#include "modules/data/CallStatusMessage.h"
#include "modules/data/DataSysStatusMessage.h"
#include "modules/data/SetPrefDataTechMessage.h"
#include "modules/data/DsdSysStatusMessage.h"
#include "modules/data/NewDDSInfoMessage.h"
#include "modules/data/DDSStatusFollowupMessage.h"

class NasModule : public Module {
  public:
    NasModule();
    ~NasModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalNasModuleCleanup();
#endif

  private:
    bool mReady = false;
    std::recursive_mutex mMutex;

    void handleQcrilInit(std::shared_ptr<Message> msg);
    void handleNasQmiIndMessage(std::shared_ptr<Message> msg);
    void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
    void handleNasEndpointStatusIndMessage(std::shared_ptr<Message> msg);

    void handleGetPreferredNetworkType(std::shared_ptr<RilRequestMessage> msg);
    void handleSetPreferredNetworkType(std::shared_ptr<RilRequestMessage> msg);
    void handleSetCdmaSubscription(std::shared_ptr<RilRequestMessage> msg);
    void handleSetCdmaSubscriptionSource(std::shared_ptr<RilRequestMessage> msg);
    void handleExitEmergencyCallbackMode(std::shared_ptr<RilRequestMessage> msg);
    void handleGetVoiceRadioTech(std::shared_ptr<RilRequestMessage> msg);
    void handleSetInitialAttachApn(std::shared_ptr<NasSetInitialAttachApnRequestMessage> msg);
    void handleSetUiccSubscription(std::shared_ptr<RilRequestMessage> msg);
    void handleGetDcRtInfo(std::shared_ptr<RilRequestMessage> msg);
    void handleShutdown(std::shared_ptr<RilRequestMessage> msg);
    void handleGetRadioCapability(std::shared_ptr<RilRequestMessage> msg);
    void handleSetRadioCapability(std::shared_ptr<RilRequestMessage> msg);
    void handleQueryNetworkSelectionMode(std::shared_ptr<RilRequestMessage> msg);
    void handleSetBandMode(std::shared_ptr<RilRequestMessage> msg);
    void handleGetNeighboringCellIds(std::shared_ptr<RilRequestMessage> msg);
    void handleSetNetworkSelectionAutomatic(std::shared_ptr<RilRequestMessage> msg);
    void handleSetNetworkSelectionManual(
        std::shared_ptr<NasSetNetworkSelectionManualRequestMessage> msg);
    void handleVoiceRegistrationState(std::shared_ptr<RilRequestMessage> msg);
    void handleDataRegistrationState(std::shared_ptr<RilRequestMessage> msg);
    void handleOperator(std::shared_ptr<RilRequestMessage> msg);
    void handleAllowData(std::shared_ptr<RilRequestMessage> msg);
    void handleCdmaSubscription(std::shared_ptr<RilRequestMessage> msg);
    void handleCdmaSetRoamingPreference(std::shared_ptr<RilRequestMessage> msg);
    void handleSetLocationUpdates(std::shared_ptr<RilRequestMessage> msg);
    void handleQueryAvailableNetworks(std::shared_ptr<RilRequestMessage> msg);
    void handleCancelQueryAvailableNetworks(std::shared_ptr<RilRequestMessage> msg);
    void handleQueryAvailableBandMode(std::shared_ptr<RilRequestMessage> msg);
    void handleSignalStrength(std::shared_ptr<RilRequestMessage> msg);
    void handleSetScreenState(std::shared_ptr<RilRequestMessage> msg);
    void handleSetDeviceState(std::shared_ptr<RilRequestMessage> msg);
    void handleSetUnsolRespFilter(std::shared_ptr<RilRequestMessage> msg);
    void handleStartNetworkScan(std::shared_ptr<RilRequestMessage> msg);
    void handleStopNetworkScan(std::shared_ptr<RilRequestMessage> msg);

    // UIM handling
    void handleUimProvisioningStatusIndMsg(std::shared_ptr<UimProvisionStatusIndMsg> msg);
    void handleUimCardStatusIndMsg(std::shared_ptr<UimCardStatusIndMsg> msg);
    void handleUimCardStatusChangeIndMsg(std::shared_ptr<UimCardStatusChangeIndMsg> msg);
    void handleUimCardAppStatusIndMsg(std::shared_ptr<UimCardAppStatusIndMsg> msg);

    // IMS handling
    void handleImsServiceStatusInd(std::shared_ptr<ImsServiceStatusInd> msg);
    void handleImsWfcStatusInd(std::shared_ptr<ImsWfcSettingsStatusInd> msg);
    void handleSetVoiceDomainPreferenceRequest(std::shared_ptr<NasSetVoiceDomainPreferenceRequest> msg);

    //Handlers for Messages from Data
    void handleEmbmsEnableDataConMessage(std::shared_ptr<NasEmbmsEnableDataConMessage> msg);
    void handleEmbmsDisableDataMessage(std::shared_ptr<NasEmbmsDisableDataMessage> msg);

    //Data Indication Handlers
    void handleDataCallStatusMessage(std::shared_ptr<rildata::CallStatusMessage> msg);
    void handleDataSetPrefDataTechMessage(std::shared_ptr<rildata::SetPrefDataTechMessage> msg);
    void handleDataSysStatusMessage(std::shared_ptr<rildata::DataSysStatusMessage> msg);
    void handleDataDsdSysStatusMessage(std::shared_ptr<rildata::DsdSysStatusMessage> msg);
    void handleDataNewDDSInfoMessage(std::shared_ptr<rildata::NewDDSInfoMessage> msg);
    void handleDataDDSStatusFollowupMessage(std::shared_ptr<rildata::DDSStatusFollowupMessage> msg);

    bool isDmsEndpointReady();

    // Handler for DMS Endpoint Status Indications
    void handleDmsEndpointStatusIndMsg(std::shared_ptr<Message> msg);
};

NasModule *getNasModule();
