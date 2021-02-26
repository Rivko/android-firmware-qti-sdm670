/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IZAT_API_V02_H
#define IZAT_API_V02_H

#include <LocApiV02.h>
#include <IzatApiBase.h>
#include <geofence.h>
#include <vector>

namespace lbs_core {
    class LocApiProxyV02;
};

using namespace loc_core;
using namespace lbs_core;

namespace izat_core {

class IzatApiV02 : public IzatApiBase {
    size_t mBatchSize, mDesiredBatchSize;
    size_t mTripBatchSize, mDesiredTripBatchSize;

    LocationError queryBatchBuffer(size_t desiredSize,
            size_t &allocatedSize, BatchingMode batchMode);
    LocationError releaseBatchBuffer(BatchingMode batchMode);
    void readModemLocations(FlpExtLocation* pLocationPiece, size_t count,
            BatchingMode batchingMode, size_t& numbOfEntries);
    void setOperationMode(GnssSuplMode mode);

    void fillGeneralAssistData(GnssAidingDataMask flags,
                               qmiLocDeleteAssistDataReqMsgT_v02 &req);
    void fillSvInfoAssistData(GnssAidingDataMask flags,
                              qmiLocDeleteAssistDataReqMsgT_v02 &req);
    bool needsNewTripBatchRestart(uint32_t newTripDistance, uint32_t newTripTBFInterval,
            uint32_t &accumulatedDistance, uint32_t &numOfBatchedPositions);
public:
    IzatApiV02(LocApiProxyV02* locApiProxy);
    inline virtual ~IzatApiV02() {}

    inline virtual void* getSibling() { return (IzatApiBase*)this; }

    /* event callback registered with the loc_api v02 interface */
    virtual int eventCb(locClientHandleType client_handle,
                         uint32_t loc_event_id,
                         locClientEventIndUnionType loc_event_payload);

    // Tracking
    LocationError startTimeBasedTracking(const LocationOptions& options);
    LocationError stopTimeBasedTracking();
    LocationError startDistanceBasedTracking(uint32_t sessionId, const LocationOptions& options);
    LocationError stopDistanceBasedTracking(uint32_t sessionId);

    // Batching
    LocationError startBatching(uint32_t sessionId, const LocationOptions& options,
                                uint32_t accuracy, uint32_t timeout);
    LocationError stopBatching(uint32_t sessionId);
    LocationError startOutdoorTripBatching(uint32_t tripDistance,
            uint32_t tripTbf, uint32_t timeout);
    LocationError stopOutdoorTripBatching(bool deallocBatchBuffer = true);
    LocationError getBatchedLocations(size_t count);
    LocationError getBatchedTripLocations(size_t count, uint32_t accumulatedDistance);
    virtual void setBatchSize(size_t size);
    virtual void setTripBatchSize(size_t size);
    LocationError queryAccumulatedTripDistance(uint32_t &accumulatedTripDistance,
            uint32_t &numOfBatchedPositions);

    // Geofence
    virtual LocationError addGeofence(uint32_t& hwId, uint32_t clientId,
                                      const GeofenceOption& options, const GeofenceInfo& info,
                                      uint8_t confidence);
    virtual LocationError removeGeofence(uint32_t hwId, uint32_t clientId);
    virtual LocationError pauseGeofence(uint32_t hwId, uint32_t clientId);
    virtual LocationError resumeGeofence(uint32_t hwId, uint32_t clientId);
    virtual LocationError modifyGeofence(uint32_t hwId, uint32_t clientId,
                                         const GeofenceOption& options);

    virtual int setUserPref(bool user_pref);
    virtual int injectApCache(APCacheData data);
    virtual int injectApCacheBlacklist(APCacheBlacklistData data);
    virtual int sendBestAvailablePosReq(LocGpsLocation &zppLoc);
    virtual int sendBestAvailablePosReq(LocGpsLocation &zppLoc, LocPosTechMask &tech_mask);

    virtual int injectLocation(FlpExtLocation location);
    virtual bool isMessageSupported(LocCheckingMessagesID msgID);
    virtual bool isFeatureSupported(uint8_t featureVal);
    virtual bool gnssConstellationConfig();
    virtual void setInSession(bool inSession);

    // for TDP
    virtual int sendGtpEnhancedCellConfigBlocking(e_premium_mode mode);
    virtual int sendGtpCellConfigurationsBlocking(const char* data, uint32_t len);

    virtual int sendGdtUploadBeginResponse(int32_t service, int32_t session, int32_t status);
    virtual int sendGdtUploadEndResponse(int32_t service, int32_t session, int32_t status);

    virtual int sendGtpApStatus(int32_t gtpApDbStatus, uint64_t gtpApPcid64, int8_t gtpApPcid64_valid,
                                const char* oemId, int8_t oemId_valid,
                                const char* modelId, int8_t modelId_valid);

    virtual int sendGdtDownloadBeginResponse(int32_t service, uint32_t session, int32_t status,
                                             const uint8_t* respLocInfo, int8_t respLocInfo_valid,
                                             uint32_t respLocInfo_len, uint32_t remainingThrottleTime,
                                             int8_t remainingThrottleTime_valid);
    virtual int sendGdtDownloadReadyStatus(int32_t service, uint32_t session, int32_t status,
                                           const char* filePath);
    virtual int sendGdtReceiveDoneResponse(int32_t service, uint32_t session, int32_t status);
    virtual int sendGdtDownloadEndResponse(int32_t service, uint32_t session, int32_t status);

    // for CSM
    virtual int sendCsmConfig(bool enable_cs_mask_valid, uint32_t enable_cs_mask,
                              bool source_mask_valid, uint32_t source_mask,
                              bool local_config_valid, bool on_demand_enabled, bool on_unsolicted_gnss_enabled,
                              bool battery_level_throttling_enabled, bool rtt_cs_enabled, bool rtt_3_cs_enabled,
                              int32_t rtt_signal_dbm_threshold, uint16_t max_data_transfer_format_ver_supported,
                              bool server_config_len_valid, uint32_t server_config_len, const char * server_config);

    virtual int readMpCsData();

    // XTRA Client 2.0
    virtual int setXtraVersionCheck(enum XtraVersionCheck check);
    virtual int injectIzatPcid(uint64_t pcid);
    virtual int injectXtraData_legacy(const char* data, uint32_t len);
    virtual int injectXtraData(const char* data, uint32_t len);
    virtual int injectNtpTime(const int64_t time, const int64_t reference, const int64_t uncertainty);
    virtual int requestXtraServers();
    virtual int requestXtraConfigInfo(const uint32_t configMask, const uint32_t configReqSource);

private:
    void batchFullEvent(const qmiLocEventBatchFullIndMsgT_v02* batchFullInfo);
    void batchStatusEvent(const qmiLocEventBatchingStatusIndMsgT_v02* batchStatusInfo);
    void onDbtPosReportEvent(const qmiLocEventDbtPositionReportIndMsgT_v02* pDbtPosReport);
    void geofenceBreachEvent(const qmiLocEventGeofenceBreachIndMsgT_v02* breachInfo);
    void geofenceBreachEvent(const qmiLocEventGeofenceBatchedBreachIndMsgT_v02* batchedBreachInfo);
    void geofenceStatusEvent(const qmiLocEventGeofenceGenAlertIndMsgT_v02* alertInfo);
    void geofenceDwellEvent(const qmiLocEventGeofenceBatchedDwellIndMsgT_v02 *dwellEvent);
    void onGdtUploadBeginEvent(const qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02* pInfo);
    void onGdtUploadEndEvent(const qmiLocEventGdtUploadEndReqIndMsgT_v02* pInfo);
    void onGdtDownloadBeginEvent(const qmiLocEventGdtDownloadBeginReqIndMsgT_v02* pInfo);
    void onGdtReceiveDoneEvent(const qmiLocEventGdtReceiveDoneIndMsgT_v02* pInfo);
    void onGdtDownloadEndEvent(const qmiLocEventGdtDownloadEndReqIndMsgT_v02* pInfo);
    void onCsmControlResp(const qmiLocCrowdSourceManagerControlIndMsgT_v02* pInfo);

    locClientStatusEnumType locSyncSendReq(uint32_t req_id, locClientReqUnionType req_payload,
            uint32_t timeout_msec, uint32_t ind_id, void* ind_payload_ptr);
    locClientStatusEnumType locClientSendReq(uint32_t req_id, locClientReqUnionType req_payload);
};

}  // namespace izat_core

#endif //IZAT_API_V02_H
