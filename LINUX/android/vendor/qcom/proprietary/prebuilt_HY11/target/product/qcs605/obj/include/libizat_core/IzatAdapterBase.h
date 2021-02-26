/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IZAT_ADAPTER_BASE_H
#define IZAT_ADAPTER_BASE_H

#include <stdbool.h>
#include <string.h>
#include <LocAdapterBase.h>
#include <LocAdapterProxyBase.h>
#include <geofence.h>
#include <fused_location_extended.h>

#ifdef __cplusplus
extern "C"
{
#endif

using namespace loc_core;

namespace izat_core {

class IzatApiBase;

class IzatAdapterBase : public LocAdapterProxyBase {
protected:
    IzatApiBase* mIzatApi;
    IzatAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                    ContextBase* context);
    virtual ~IzatAdapterBase();
    const MsgTask* mMsgTask;
public:
    virtual void geofenceBreachEvent(size_t count, uint32_t* hwIds, Location& location,
                                     GeofenceBreachType breachType, uint64_t timestamp);
    virtual void geofenceStatusEvent(GeofenceStatusAvailable available);

    inline void sendMsg(const LocMsg* msg) const {
        mMsgTask->sendMsg(msg);
    }
    inline void sendMsg(const LocMsg* msg) {
        mMsgTask->sendMsg(msg);
    }

    virtual bool setUserPreference(bool user_pref);
    virtual void handleEngineUpEvent();
    virtual void handleEngineDownEvent();
    virtual bool updateGfBreachLocation(FlpExtLocation& gpsLocation);

    virtual void reportPositionEvent(UlpLocation &location,
                                     GpsLocationExtended &locationExtended,
                                     enum loc_sess_status status,
                                     LocPosTechMask loc_technology_mask);

    virtual void reportLocationsEvent(const FlpExtLocation* flpLocations, size_t count,
            BatchingMode batchingMode);
    virtual void reportCompletedTripsEvent(uint32_t accumulated_distance);
    virtual void reportBatchStatusChangeEvent(BatchingStatus batchStatus);

    // for GDT AP
    virtual bool onGdtUploadBeginEvent(int32_t service,
                                       int32_t session,
                                       const char* filePath,
                                       uint32_t filePath_len);
    virtual bool onGdtUploadEndEvent(int32_t service, int32_t session, int32_t status);
    virtual bool handleGtpApStatusResp(int32_t status, uint16_t clientSwVer,
                                       uint8_t asnMajVer, uint8_t asnMinVer, uint8_t asnPtVer);
    virtual bool onGdtDownloadBeginEvent(int32_t service, uint32_t session,
                                         uint32_t respTimeoutInterval,
                                         const uint8_t* clientInfo, uint32_t clientInfo_len,
                                         const uint8_t* mobileStatusData, uint32_t mobileStatusData_len,
                                         const char* filePath, uint32_t filePath_len,
                                         uint32_t powerBudgetInfo, int8_t powerBudgetInfo_valid,
                                         uint32_t powerBudgetAllowance, int8_t powerBudgetAllowance_valid,
                                         uint32_t downloadRequestMask, int8_t downloadRequestMask_valid);
    virtual bool onGdtReceiveDoneEvent(int32_t service, uint32_t session, int32_t status);
    virtual bool onGdtDownloadEndEvent(int32_t service, uint32_t session, int32_t status);

    // for CSM
    virtual bool onSendCsmConfigResp(int32_t status, bool onDemandCrowdSourcingSupported,
                                     bool unsolicitedGnssCrowdSourcingSupported,
                                     uint8_t majorVersionSupported, uint8_t minorVersionSupported,
                                     uint16_t maxDataTransferFormatVersionSupported,
                                     uint8_t modemClientInfo_valid,
                                     uint32_t modemClientInfo_len, const uint8_t* modemClientInfo);

    // XTRA Client 2.0
    virtual bool onReceiveXtraServers(const char* server1, const char* server2, const char* server3);
    virtual bool onRequestXtraData();
    virtual bool onRequestNtpTime();
    virtual bool onReceiveXtraConfig(uint32_t status, uint32_t configReqSource,
                                     uint32_t report_mask,
                                     uint16_t absAge, uint64_t relAge,
                                     uint8_t timereqValid, uint16_t prefValidAgeHrs,
                                     const char* ntpServer1, const char* ntpServer2,
                                     const char* ntpServer3, const char* xtraServer1,
                                     const char* xtraServer2, const char* xtraServer3);

#ifdef ON_TARGET_TEST
#endif
};

}  // namespace izat_core

#ifdef __cplusplus
}
#endif

#endif /* IZAT_ADAPTER_BASE_H */
