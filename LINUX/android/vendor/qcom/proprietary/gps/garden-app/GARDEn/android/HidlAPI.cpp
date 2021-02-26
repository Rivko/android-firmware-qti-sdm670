/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "Garden"
#define LOG_NDEBUG 0

#include "GardenUtil.h"
#include <loc_cfg.h>
#include "HidlAPI.h"
#include <mutex>
#include <android/hardware/gnss/1.0/IGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
using android::OK;
using android::sp;
using android::wp;
using android::status_t;

using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_vec;
using android::hardware::hidl_death_recipient;
using android::hidl::base::V1_0::IBase;

using android::hardware::gnss::V1_0::IAGnss;
using android::hardware::gnss::V1_0::IAGnssCallback;
using android::hardware::gnss::V1_0::IAGnssCallback;
using android::hardware::gnss::V1_0::IAGnssRil;
using android::hardware::gnss::V1_0::IAGnssRilCallback;
using android::hardware::gnss::V1_0::IGnss;
using android::hardware::gnss::V1_0::IGnssBatching;
using android::hardware::gnss::V1_0::IGnssBatchingCallback;
using android::hardware::gnss::V1_0::IGnssCallback;
using android::hardware::gnss::V1_0::IGnssConfiguration;
using android::hardware::gnss::V1_0::IGnssDebug;
using android::hardware::gnss::V1_0::IGnssGeofenceCallback;
using android::hardware::gnss::V1_0::IGnssGeofencing;
using android::hardware::gnss::V1_0::IGnssMeasurement;
using android::hardware::gnss::V1_0::IGnssMeasurementCallback;
using android::hardware::gnss::V1_0::IGnssNavigationMessage;
using android::hardware::gnss::V1_0::IGnssNavigationMessageCallback;
using android::hardware::gnss::V1_0::IGnssNi;
using android::hardware::gnss::V1_0::IGnssNiCallback;
using android::hardware::gnss::V1_0::IGnssXtra;
using android::hardware::gnss::V1_0::IGnssXtraCallback;
using android::hardware::gnss::V1_0::GnssLocation;

using vendor::qti::gnss::V1_0::ILocHidlGnss;
using vendor::qti::gnss::V1_0::ILocHidlFlpService;
using vendor::qti::gnss::V1_0::ILocHidlGeofenceService;
using vendor::qti::gnss::V1_0::ILocHidlFlpServiceCallback;
using vendor::qti::gnss::V1_0::ILocHidlGeofenceServiceCallback;
using vendor::qti::gnss::V1_0::LocHidlLocation;
using vendor::qti::gnss::V1_0::LocHidlBatchOptions;
using vendor::qti::gnss::V1_0::LocHidlBatchStatusInfo;
using std::mutex;
using lock_guard = std::lock_guard<mutex>;

namespace garden {
struct GnssDeathRecipient;
sp<GnssDeathRecipient> gnssHalDeathRecipient = nullptr;
sp<GnssDeathRecipient> gnssLocHalDeathRecipient = nullptr;
sp<IGnss> gnssHal = nullptr;
sp<IGnssXtra> gnssXtraIface = nullptr;
sp<IAGnssRil> agnssRilIface = nullptr;
sp<IGnssGeofencing> gnssGeofencingIface = nullptr;
sp<IAGnss> agnssIface = nullptr;
sp<IGnssBatching> gnssBatchingIface = nullptr;
sp<IGnssDebug> gnssDebugIface = nullptr;
sp<IGnssConfiguration> gnssConfigurationIface = nullptr;
sp<IGnssNi> gnssNiIface = nullptr;
sp<IGnssMeasurement> gnssMeasurementIface = nullptr;
sp<IGnssNavigationMessage> gnssNavigationMessageIface = nullptr;
sp<ILocHidlGnss> gnssHidlHal =  nullptr;
sp<ILocHidlFlpService> locHidlFlpIface = nullptr;
sp<ILocHidlGeofenceService> locHidlGeofenceIface = nullptr;
static GnssCbBase* sGnssCbs = nullptr;
static GnssCbBase* sFlpCbs = nullptr;
static GnssCbBase* sGeofenceCbs = nullptr;
static mutex sGnssLock;
static mutex sFlpLock;
static mutex sGeofenceLock;
static sp<ILocHidlFlpServiceCallback> gnssFlpServiceProviderCb = nullptr;

struct GnssDeathRecipient : virtual public hidl_death_recipient {
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        gardenPrint("%s] IGnss died with cookie: %" PRIu64, __FILE__, cookie);
        gnssHal = nullptr;
        gnssHidlHal = nullptr;
    }
};
static void translateLocation(Location& location, const GnssLocation& gnssLocation) {
    location.size = sizeof(Location);
    location.flags = static_cast<uint16_t>(gnssLocation.gnssLocationFlags);
    location.timestamp = gnssLocation.timestamp;
    location.latitude = gnssLocation.latitudeDegrees;
    location.longitude = gnssLocation.longitudeDegrees;
    location.altitude = gnssLocation.altitudeMeters;
    location.speed = gnssLocation.speedMetersPerSec;
    location.bearing = gnssLocation.bearingDegrees;
    location.accuracy = gnssLocation.horizontalAccuracyMeters;
    location.verticalAccuracy = gnssLocation.horizontalAccuracyMeters;
    location.speedAccuracy = gnssLocation.speedAccuracyMetersPerSecond;
    location.bearingAccuracy = gnssLocation.bearingAccuracyDegrees;
}

static void translateLocation(Location& location, const LocHidlLocation& hidlLocation) {
    location.size = sizeof(Location);
    location.flags = hidlLocation.locationFlagsMask;
    location.timestamp = hidlLocation.timestamp;
    location.latitude = hidlLocation.latitude;
    location.longitude = hidlLocation.longitude;
    location.altitude = hidlLocation.altitude;
    location.speed = hidlLocation.speed;
    location.bearing = hidlLocation.bearing;
    location.accuracy = hidlLocation.accuracy;
    location.verticalAccuracy = hidlLocation.verticalAccuracy;
    location.speedAccuracy = hidlLocation.speedAccuracy;
    location.bearingAccuracy = hidlLocation.bearingAccuracy;
    location.techMask = hidlLocation.locationTechnologyMask;
}
static void translateStatus(LocationError& error, IGnssGeofenceCallback::GeofenceStatus& status) {
    switch (status) {
        case IGnssGeofenceCallback::GeofenceStatus::OPERATION_SUCCESS:
            error = LOCATION_ERROR_SUCCESS;
            break;
        case IGnssGeofenceCallback::GeofenceStatus::ERROR_TOO_MANY_GEOFENCES:
            error = LOCATION_ERROR_GEOFENCES_AT_MAX;
            break;
        case IGnssGeofenceCallback::GeofenceStatus::ERROR_ID_EXISTS:
            error = LOCATION_ERROR_ID_EXISTS;
            break;
        case IGnssGeofenceCallback::GeofenceStatus::ERROR_ID_UNKNOWN:
            error = LOCATION_ERROR_ID_UNKNOWN;
            break;
        case IGnssGeofenceCallback::GeofenceStatus::ERROR_INVALID_TRANSITION:
            error = LOCATION_ERROR_INVALID_PARAMETER;
            break;
        default:
            error = LOCATION_ERROR_GENERAL_FAILURE;
    }
}

/*
 * GnssCallback class implements the callback methods for IGnss interface.
 */
struct GnssCallback : public IGnssCallback {
    Return<void> gnssLocationCb(const GnssLocation& location) override;
    Return<void> gnssStatusCb(const IGnssCallback::GnssStatusValue status) override;
    Return<void> gnssSvStatusCb(const IGnssCallback::GnssSvStatus& svStatus) override;
    Return<void> gnssNmeaCb(android::hardware::gnss::V1_0::GnssUtcTime timestamp,
            const ::android::hardware::hidl_string& nmea) override;
    Return<void> gnssSetCapabilitesCb(uint32_t capabilities) override;
    Return<void> gnssAcquireWakelockCb() override;
    Return<void> gnssReleaseWakelockCb() override;
    Return<void> gnssRequestTimeCb() override;
    Return<void> gnssSetSystemInfoCb(const IGnssCallback::GnssSystemInfo& info) override;
};

/*
 * GnssBatchingCallback interface implements the callback methods
 * required by the IGnssBatching interface.
 */
struct GnssBatchingCallback : public IGnssBatchingCallback {
    /*
    * Methods from ::android::hardware::gps::V1_0::IGnssBatchingCallback
    * follow.
    */
    Return<void> gnssLocationBatchCb(
            const ::android::hardware::hidl_vec<GnssLocation>& locations) override;
};

struct GnssGeofenceCallback : public IGnssGeofenceCallback {
    // Methods from ::android::hardware::gps::V1_0::IGnssGeofenceCallback follow.
    Return<void> gnssGeofenceTransitionCb(int32_t geofenceId, const GnssLocation& location,
            GeofenceTransition transition,
            android::hardware::gnss::V1_0::GnssUtcTime timestamp) override;
    Return<void> gnssGeofenceStatusCb(GeofenceAvailability status,
            const GnssLocation& location) override;
    Return<void> gnssGeofenceAddCb(int32_t geofenceId, GeofenceStatus status) override;
    Return<void> gnssGeofenceRemoveCb(int32_t geofenceId, GeofenceStatus status) override;
    Return<void> gnssGeofencePauseCb(int32_t geofenceId, GeofenceStatus status) override;
    Return<void> gnssGeofenceResumeCb(int32_t geofenceId, GeofenceStatus status) override;
};

Return<void> GnssCallback::gnssLocationCb(const GnssLocation& gnssLocation) {
    Location location;
    memset(&location, 0, sizeof(Location));
    translateLocation(location, gnssLocation);
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, location.flags);

    lock_guard guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssLocationCb(location);
    }
    return Void();
}

Return<void> GnssCallback::gnssStatusCb(const IGnssCallback::GnssStatusValue status) {
    lock_guard guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssStatusCb(static_cast<uint32_t>(status));
    }
    return Void();
}

Return<void> GnssCallback::gnssSvStatusCb(const IGnssCallback::GnssSvStatus& svStatus) {
    GnssSvNotification gnssSvNotification;
    memset(&gnssSvNotification, 0, sizeof(GnssSvNotification));
    gnssSvNotification.size = sizeof(GnssSvNotification);
    gnssSvNotification.count = svStatus.numSvs;
    if (svStatus.numSvs > 0) {
        for (int i=0; i< svStatus.numSvs; ++i) {
            gnssSvNotification.gnssSvs[i].size = sizeof(GnssSv);
            gnssSvNotification.gnssSvs[i].svId = static_cast<uint16_t>(svStatus.gnssSvList[i].svid);
            gnssSvNotification.gnssSvs[i].type =
                static_cast<GnssSvType>(svStatus.gnssSvList[i].constellation);
            gnssSvNotification.gnssSvs[i].cN0Dbhz = svStatus.gnssSvList[i].cN0Dbhz;
            gnssSvNotification.gnssSvs[i].elevation = svStatus.gnssSvList[i].elevationDegrees;
            gnssSvNotification.gnssSvs[i].azimuth = svStatus.gnssSvList[i].azimuthDegrees;
            gnssSvNotification.gnssSvs[i].gnssSvOptionsMask =
                static_cast<uint16_t>(svStatus.gnssSvList[i].svFlag);
        }
    }
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, gnssSvNotification.count);
    lock_guard guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssSvStatusCb(gnssSvNotification);
    }
    return Void();
}

Return<void> GnssCallback::gnssNmeaCb(android::hardware::gnss::V1_0::GnssUtcTime timestamp,
        const ::android::hardware::hidl_string& nmea) {
    lock_guard guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssNmeaCb(timestamp, nmea.c_str(), nmea.size());
    }
    return Void();
}

Return<void> GnssCallback::gnssSetCapabilitesCb(uint32_t capabilities) {
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, capabilities);
    lock_guard guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssSetCapabilitiesCb(capabilities);
    }
    return Void();
}

Return<void> GnssCallback::gnssAcquireWakelockCb() {
    return Void();
}

Return<void> GnssCallback::gnssReleaseWakelockCb() {
    return Void();
}

Return<void> GnssCallback::gnssRequestTimeCb() {
    lock_guard guard(sGnssLock);
    if (sGnssCbs != nullptr) {
        sGnssCbs->gnssRequestUtcTimeCb();
    }
    return Void();
}

Return<void> GnssCallback::gnssSetSystemInfoCb(const IGnssCallback::GnssSystemInfo& info) {
    return Void();
}

Return<void> GnssBatchingCallback::gnssLocationBatchCb(
        const ::android::hardware::hidl_vec<GnssLocation>& locations) {
    size_t length = locations.size();
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, length);
    Location location;
    lock_guard guard(sFlpLock);
    if(sFlpCbs != nullptr) {
        for (int i=0; i<length; ++i) {
            memset(&location, 0, sizeof(Location));
            translateLocation(location, locations[i]);
            sFlpCbs->batchingCb(length, &location);
        }
    }
    return Void();
}

Return<void> GnssGeofenceCallback::gnssGeofenceTransitionCb( int32_t geofenceId,
        const GnssLocation& gnssLocation, GeofenceTransition transition,
        android::hardware::gnss::V1_0::GnssUtcTime timestamp) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    Location location;
    memset(&location, 0, sizeof(Location));
    translateLocation(location, gnssLocation);
    GeofenceBreachType geoBreach = GEOFENCE_BREACH_UNKNOWN;
    if (IGnssGeofenceCallback::GeofenceTransition::ENTERED == transition) {
        geoBreach = GEOFENCE_BREACH_ENTER;
    } else if (IGnssGeofenceCallback::GeofenceTransition::EXITED == transition) {
        geoBreach = GEOFENCE_BREACH_EXIT;
    }
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->geofenceBreachCb(geofenceId, location, geoBreach,
                static_cast<uint64_t>(timestamp));
    }
    return Void();
}

Return<void> GnssGeofenceCallback::gnssGeofenceStatusCb(GeofenceAvailability status,
        const GnssLocation& location) {
    LOC_LOGD("%s]: (%d)", __func__, status);
    GeofenceStatusAvailable available = GEOFENCE_STATUS_AVAILABILE_NO;
    if (IGnssGeofenceCallback::GeofenceAvailability::AVAILABLE == status) {
        available = GEOFENCE_STATUS_AVAILABILE_YES;
    }
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->geofenceStatusCb(available);
    }
    return Void();
}

Return<void> GnssGeofenceCallback::gnssGeofenceAddCb(int32_t geofenceId, GeofenceStatus status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->addGeofencesCb(geofenceId, error);
    }
    return Void();
}

Return<void> GnssGeofenceCallback::gnssGeofenceRemoveCb(int32_t geofenceId,
        GeofenceStatus status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->removeGeofencesCb(geofenceId, error);
    }
    return Void();
}

Return<void> GnssGeofenceCallback::gnssGeofencePauseCb(int32_t geofenceId,
        GeofenceStatus status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->pauseGeofencesCb(geofenceId, error);
    }
    return Void();
}

Return<void> GnssGeofenceCallback::gnssGeofenceResumeCb(int32_t geofenceId,
        GeofenceStatus status) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    LocationError error;
    translateStatus(error, status);
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->resumeGeofencesCb(geofenceId, error);
    }
    return Void();
}
/* ===========================================================
 *   HIDL Callbacks : ILocHidlFlpServiceCallback.hal
 * ===========================================================*/
struct GnssFlpServiceCallback : public ILocHidlFlpServiceCallback {

    Return<void> gnssLocationTrackingCb(
      const LocHidlLocation& gnssLocation) override;

    Return<void> gnssLocationBatchingCb(
      const LocHidlBatchOptions &batchOptions,
      const hidl_vec<LocHidlLocation>& gnssLocations) override;

    Return<void> gnssBatchingStatusCb(
            const LocHidlBatchStatusInfo &batchStatus,
            const hidl_vec<uint32_t>& listOfCompletedTrips) override;

    Return<void> gnssMaxPowerAllocatedCb(int32_t powerInMW) override;
};

Return<void> GnssFlpServiceCallback::gnssLocationTrackingCb(
        const LocHidlLocation& gnssLocation) {
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, gnssLocation.locationFlagsMask);
    Location location;
    memset(&location, 0, sizeof(Location));
    translateLocation(location, gnssLocation);
    lock_guard guard(sFlpLock);
    if (sFlpCbs != nullptr) {
        sFlpCbs->gnssLocationCb(location);
    }
    return Void();
}

Return<void> GnssFlpServiceCallback::gnssLocationBatchingCb(
        const LocHidlBatchOptions &batchOptions,
        const hidl_vec<LocHidlLocation>& gnssLocations) {
    size_t length = gnssLocations.size();
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, length);
    Location location;
    lock_guard guard(sFlpLock);
    if(sFlpCbs != nullptr) {
        for (int i=0; i<length; ++i) {
            memset(&location, 0, sizeof(Location));
            translateLocation(location, gnssLocations[i]);
            sFlpCbs->batchingCb(length, &location);
        }
    }
    return Void();
}

Return<void> GnssFlpServiceCallback::gnssBatchingStatusCb(
        const LocHidlBatchStatusInfo &batchStatusInfo,
        const hidl_vec<uint32_t>& listOfCompletedTrips) {
    return Void();
}

Return<void> GnssFlpServiceCallback::gnssMaxPowerAllocatedCb(
        int32_t powerInMW) {
    return Void();
}



/*
 * GnssNiCallback implements callback methods required by the IGnssNi interface.
 */
struct GnssNiCallback : public IGnssNiCallback {
    Return<void> niNotifyCb(const IGnssNiCallback::GnssNiNotification& notification)
            override;
};

Return<void> GnssNiCallback::niNotifyCb(
        const IGnssNiCallback::GnssNiNotification& notification) {
    LOC_LOGD("%s]: (id: %d)", __FUNCTION__, notification.notificationId);
    ::GnssNiNotification gnssNiNotification;
    memset(&gnssNiNotification, 0, sizeof(::GnssNiNotification));
    gnssNiNotification.size = sizeof(::GnssNiNotification);
    if (static_cast<uint8_t>(notification.niType) != 0) {
        gnssNiNotification.type = static_cast<::GnssNiType>((uint8_t)notification.niType-1);
    }
    gnssNiNotification.options = static_cast<uint16_t>(notification.notifyFlags);
    gnssNiNotification.timeout = notification.timeoutSec;
    gnssNiNotification.timeoutResponse = static_cast<GnssNiResponse>(notification.defaultResponse);
    memcpy(&gnssNiNotification.requestor, notification.requestorId.c_str(),
            notification.requestorId.size()<GNSS_NI_REQUESTOR_MAX?
            notification.requestorId.size():GNSS_NI_REQUESTOR_MAX);
    gnssNiNotification.requestorEncoding =
            static_cast<::GnssNiEncodingType>(notification.requestorIdEncoding);
    memcpy(&gnssNiNotification.message, notification.notificationMessage.c_str(),
            notification.notificationMessage.size()<GNSS_NI_MESSAGE_ID_MAX?
            notification.notificationMessage.size():GNSS_NI_MESSAGE_ID_MAX);
    gnssNiNotification.messageEncoding =
            static_cast<::GnssNiEncodingType>(notification.notificationIdEncoding);
    lock_guard guard(sGnssLock);
    if (sGnssCbs == nullptr) {
        gardenPrint("%s]: mGnssNiCbIface is nullptr", __FUNCTION__);
        return Void();
    }
    sGnssCbs->gnssNiNotifyCb(notification.notificationId, gnssNiNotification);
    return Void();
}

/* ===========================================================
 *   HIDL Callbacks : ILocHidlGeofenceServiceCallback.hal
 * ===========================================================*/
struct GnssGeofenceServiceProviderCallback :
        public ILocHidlGeofenceServiceCallback {

    Return<void> gnssGeofenceBreachCb(
            int32_t count, const hidl_vec<int32_t>& idList,
            const LocHidlLocation& hidlLocation, int32_t breachType,
            uint64_t timestamp) override;
    Return<void> gnssGeofenceStatusCb(
            int32_t statusAvailable, int32_t locTechType) override;
    Return<void> gnssAddGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
    Return<void> gnssRemoveGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
    Return<void> gnssPauseGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
    Return<void> gnssResumeGeofencesCb(
            int32_t count, const hidl_vec<int32_t>& locationErrorList,
            const hidl_vec<int32_t>& idList) override;
};

Return<void> GnssGeofenceServiceProviderCallback::gnssGeofenceBreachCb(
        int32_t count, const hidl_vec<int32_t>& idList,
        const LocHidlLocation& hidlLocation, int32_t breachType,
        uint64_t timestamp) {
    LOC_LOGD("%s]: (%zu)", __func__, count);
    Location location;
    translateLocation(location, hidlLocation);
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        for (size_t i = 0; i < count; i++) {
            sGeofenceCbs->geofenceBreachCb(idList[i], location,
                    static_cast<GeofenceBreachType>(breachType), timestamp);
        }
    }
    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssGeofenceStatusCb(
        int32_t statusAvailable, int32_t /*locTechType*/) {
    LOC_LOGD("%s]: (%d)", __func__, statusAvailable);
    lock_guard guard(sGeofenceLock);
    if (sGeofenceCbs) {
        sGeofenceCbs->geofenceStatusCb(static_cast<GeofenceStatusAvailable>(statusAvailable));
    }
    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssAddGeofencesCb(
        int32_t count, const hidl_vec<int32_t>& locationErrorList,
        const hidl_vec<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->addGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssRemoveGeofencesCb(
        int32_t count, const hidl_vec<int32_t>& locationErrorList,
        const hidl_vec<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->removeGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssPauseGeofencesCb(
        int32_t count, const hidl_vec<int32_t>& locationErrorList,
        const hidl_vec<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->pauseGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return Void();
}

Return<void> GnssGeofenceServiceProviderCallback::gnssResumeGeofencesCb(
        int32_t count, const hidl_vec<int32_t>& locationErrorList,
        const hidl_vec<int32_t>& idList) {
    LOC_LOGD("%s]: (%d)", __func__, count);
    lock_guard guard(sGeofenceLock);
    for (size_t i = 0; i < count; i++) {
        if (sGeofenceCbs) {
            sGeofenceCbs->resumeGeofencesCb(idList[i],
                    static_cast<LocationError>(locationErrorList[i]));
        }
    }
    return Void();
}

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlGeofenceServiceCallback> gnssGeofenceServiceProviderCb = nullptr;

sp<IGnssBatching>& getBatchingIface() {
    if (gnssBatchingIface == nullptr) {
        if (gnssHal != nullptr) {
            gnssBatchingIface = gnssHal->getExtensionGnssBatching();
        }
        if (gnssBatchingIface == nullptr) {
            ALOGE("Unable to get a handle to IGnssBatching extension!");
        }
    }
    return gnssBatchingIface;
}

sp<IGnssGeofencing>& getGeofenceIface() {
    if (gnssGeofencingIface == nullptr) {
        if (gnssHal != nullptr) {
            gnssGeofencingIface = gnssHal->getExtensionGnssGeofencing();
        }
        if (gnssGeofencingIface == nullptr) {
            ALOGE("Unable to get a handle to IGnssGeofencing extension!");
        }
    }
    return gnssGeofencingIface;
}

sp<ILocHidlFlpService>& getLocHidlFlpIface() {
    if (locHidlFlpIface == nullptr) {
        if (gnssHidlHal != nullptr) {
            locHidlFlpIface = gnssHidlHal->getExtensionLocHidlFlpService();
        }
        if (locHidlFlpIface == nullptr) {
            ALOGE("Unable to get a handle to ILocHidlFlpService extension!");
        }
    }
    return locHidlFlpIface;
}

sp<ILocHidlGeofenceService>& getLocHidlGeofenceIface() {
    if (locHidlGeofenceIface == nullptr) {
        if (gnssHidlHal != nullptr) {
            locHidlGeofenceIface = gnssHidlHal->getExtensionLocHidlGeofenceService();
        }
        if (locHidlGeofenceIface == nullptr) {
            ALOGE("Unable to get a handle to ILocHidlGeofenceService extension!");
        }
    }
    return locHidlGeofenceIface;
}

void HidlAPI::setGnssCbs(GnssCbBase* callbacks) {
    {
        lock_guard guard(sGnssLock);
        sGnssCbs = callbacks;
    }
    sp<IGnssCallback> gnssCbIface = new GnssCallback();
    /*
     * Fail if the main interface fails to initialize
     */
    if (gnssHal == nullptr) {
        ALOGE("Unable to Initialize GNSS HAL\n");
    }

    auto result = gnssHal->setCallback(gnssCbIface);
    if (!result.isOk() || !result) {
        ALOGE("SetCallback for Gnss Interface fails\n");
    }

    auto gnssNi = gnssHal->getExtensionGnssNi();
    if (!gnssNi.isOk()) {
        ALOGD("Unable to get a handle to GnssNi");
    } else {
        gnssNiIface = gnssNi;
    }
    sp<IGnssNiCallback> gnssNiCbIface = new GnssNiCallback();
    if (gnssNiIface != nullptr) {
        gnssNiIface->setCallback(gnssNiCbIface);
    } else {
        ALOGE("Unable to initialize GNSS NI interface");
    }
}

void HidlAPI::setFlpCbs(GnssCbBase* callbacks) {
    {
        lock_guard guard(sFlpLock);
        sFlpCbs = callbacks;
    }
    gardenPrint("entry of setFlpCbs.");
#ifdef LOCHIDL
    gnssHidlHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
    auto flpHidlIface = getLocHidlFlpIface();
    if (flpHidlIface == nullptr) {
        ALOGE("ILocHidlFlpService Iface NULL!");
        return;
    }
    sp<GnssFlpServiceCallback> gnssFlpServiceCb =
        new GnssFlpServiceCallback();
    sp<ILocHidlFlpService> flpVendorIface = flpHidlIface;
    auto init = flpVendorIface->init(gnssFlpServiceCb);
#else
    sp<IGnssBatchingCallback> gnssBatchingCbIface = new GnssBatchingCallback();
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return;
    }
    iface->init(gnssBatchingCbIface);
#endif
}

void HidlAPI::setGeofenceCbs(GnssCbBase* callbacks) {
    {
        lock_guard guard(sGeofenceLock);
        sGeofenceCbs = callbacks;
    }
    gardenPrint("entry of setGeofenceCbs.");
#ifdef LOCHIDL
    auto geofenceVendor = gnssHidlHal->getExtensionLocHidlGeofenceService();
    sp<GnssGeofenceServiceProviderCallback> gnssGeofenceServiceProviderCb =
        new GnssGeofenceServiceProviderCallback();
    sp<ILocHidlGeofenceService> geofenceVendorIface = geofenceVendor;
    auto init = geofenceVendorIface->init(gnssGeofenceServiceProviderCb);
#else
    sp<IGnssGeofenceCallback> gnssGeofencingCbIface = new GnssGeofenceCallback();
    auto iface = getGeofenceIface();
    if (iface == nullptr) {
        ALOGE("IGnssGeofencing Iface NULL!");
        return;
    }
    iface->setCallback(gnssGeofencingCbIface);
#endif
}

HidlAPI::HidlAPI() {
    gnssHal = IGnss::getService();
#ifdef LOCHIDL
    gnssHidlHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
    if (gnssHidlHal != nullptr) {
        gardenPrint("get Lochidl api success.");
        gnssLocHalDeathRecipient = new GnssDeathRecipient();
        android::hardware::Return<bool> loclinked = gnssHal->linkToDeath(gnssLocHalDeathRecipient, 0);
        if (!loclinked.isOk()) {
            ALOGE("Transaction error in linking to GnssHAL death: %s",
                    loclinked.description().c_str());
        } else if (!loclinked) {
            ALOGW("Unable to link to GnssHidlHal death notifications");
        } else {
            ALOGD("Link to death notification successful");
        }
    } else {
        gardenPrint("get Lochidl api failed.");
    }
#endif
    if (gnssHal != nullptr) {
        gardenPrint("get hidl api success.");
        gnssHalDeathRecipient = new GnssDeathRecipient();
        android::hardware::Return<bool> linked = gnssHal->linkToDeath(gnssHalDeathRecipient, 0);
        if (!linked.isOk()) {
            ALOGE("Transaction error in linking to GnssHAL death: %s",
                    linked.description().c_str());
        } else if (!linked) {
            ALOGW("Unable to link to GnssHal death notifications");
        } else {
            ALOGD("Link to death notification successful");
        }
    } else {
        gardenPrint("get hidl api failed.");
    }
}

int HidlAPI::gnssStart() {
    if (gnssHal != nullptr) {
        gnssHal->start();
    }
    return 0;
}

int HidlAPI::gnssStop() {
    if (gnssHal != nullptr) {
        gnssHal->stop();
    }
    return 0;
}

void HidlAPI::gnssCleanup() {
    if (gnssHal != nullptr) {
        gnssHal->cleanup();
    }
}

int HidlAPI::gnssInjectLocation(double latitude, double longitude, float accuracy) {
    if (gnssHal != nullptr) {
        gnssHal->injectLocation(latitude, longitude, accuracy);
    }
    return 0;
}

void HidlAPI::gnssDeleteAidingData(LocGpsAidingData flags) {
    if (gnssHal != nullptr) {
        gnssHal->deleteAidingData(static_cast<IGnss::GnssAidingData>(flags));
    }
}

int HidlAPI::gnssSetPositionMode(LocGpsPositionMode mode,
        LocGpsPositionRecurrence recurrence, uint32_t min_interval,
        uint32_t preferred_accuracy, uint32_t preferred_time) {
    if (gnssHal != nullptr) {
        gnssHal->setPositionMode(static_cast<IGnss::GnssPositionMode>(mode),
                static_cast<IGnss::GnssPositionRecurrence>(recurrence),
                min_interval, preferred_accuracy, preferred_time);
    }
    return 0;
}

void HidlAPI::configurationUpdate(const char* config_data, int32_t length) {
    LOC_LOGD("%s]: (%s %d)", __func__, config_data, length);
    const int n = 10;
    uint8_t flags[n];
    memset(&flags, 0, sizeof(uint8_t) * n);
    GnssConfig data;
    memset(&data, 0, sizeof(GnssConfig));
    data.size = sizeof(GnssConfig);

    const loc_param_s_type gnssConfTable[] =
    {
        {"GPS_LOCK",                            &data.gpsLock,                      flags+0, 'n'},
        {"SUPL_VER",                            &data.suplVersion,                  flags+1, 'n'},
        {"LPP_PROFILE",                         &data.lppProfile,                   flags+3, 'n'},
        {"LPPE_CP_TECHNOLOGY",                  &data.lppeControlPlaneMask,         flags+4, 'n'},
        {"LPPE_UP_TECHNOLOGY",                  &data.lppeUserPlaneMask,            flags+5, 'n'},
        {"A_GLONASS_POS_PROTOCOL_SELECT",       &data.aGlonassPositionProtocolMask, flags+6, 'n'},
        {"USE_EMERGENCY_PDN_FOR_EMERGENCY_SUPL",&data.emergencyPdnForEmergencySupl, flags+7, 'n'},
        {"SUPL_ES",                             &data.suplEmergencyServices,        flags+8, 'n'},
        {"SUPL_MODE",                           &data.suplModeMask,                 flags+9, 'n'},
    };
    UTIL_UPDATE_CONF(config_data, length, gnssConfTable);

    for (int i = 0; i < n; i++) {
        if (flags[i] != 0)
            data.flags |= (0x1 << i);
    }
    auto gnssConfig = gnssHal->getExtensionGnssConfiguration();
    if (!gnssConfig.isOk()) {
        ALOGE("Unable to get a handle to IGnssConfiguration extension!");
    } else {
        sp<IGnssConfiguration> gnssConf = gnssConfig;
        gnssConf->setSuplEs(data.suplEmergencyServices);
        gnssConf->setSuplVersion(data.suplVersion);
        gnssConf->setSuplMode(data.suplModeMask);
        gnssConf->setGpsLock(data.gpsLock);
        gnssConf->setLppProfile(data.lppProfile);
        gnssConf->setGlonassPositioningProtocol(data.aGlonassPositionProtocolMask);
        gnssConf->setEmergencySuplPdn(data.emergencyPdnForEmergencySupl);
    }
}

void HidlAPI::updateNetworkAvailability(int /*available*/, const char* /*apn*/) {
}

void HidlAPI::gnssNiResponse(uint32_t id, GnssNiResponse response) {
    if (gnssHal != nullptr) {
        auto gnssNi = gnssHal->getExtensionGnssNi();
        if (!gnssNi.isOk()) {
            ALOGE("Unable to get a handle to IGnssNi extension!");
        } else {
            sp<IGnssNi> gnssNiIface = gnssNi;
            gnssNiIface->respond(id, static_cast<IGnssNiCallback::GnssUserResponseType>(response));
        }
    }
}

int HidlAPI::flpStartSession(uint32_t mode, LocationOptions& options, uint32_t /*id*/) {
    IGnssBatching::Options batchingOptions;
    memset(&batchingOptions, 0, sizeof(batchingOptions));
    batchingOptions.periodNanos = options.minInterval*1000000;
    if (SESSION_MODE_ON_FULL == mode)
        batchingOptions.flags = static_cast<uint8_t>(IGnssBatching::Flag::WAKEUP_ON_FIFO_FULL);
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return 1;
    }
    iface->start(batchingOptions);
    return 0;
}

int HidlAPI::flpStopSession(uint32_t /*id*/) {
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return 1;
    }
    iface->stop();
    return 0;
}

int HidlAPI::flpGetBatchSize() {
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return 1;
    }
    iface->getBatchSize();
    return 0;
}

int HidlAPI::flpUpdateSession(uint32_t sessionMode, LocationOptions& options,
        uint32_t id) {
    auto iface = getLocHidlFlpIface();
    if (iface  == nullptr) {
        ALOGE("ILocHidlFlpService Iface NULL!");
        return 1;
    }
    uint32_t flags = 0;
    if (SESSION_MODE_ON_FULL == sessionMode)
        flags = static_cast<uint32_t>(IGnssBatching::Flag::WAKEUP_ON_FIFO_FULL);
    iface->updateFlpSession(id, flags, options.minInterval*1000000, options.minDistance, 0);
    return 0;
}

int HidlAPI::flpGetBatchedLocation(int /*lastNLocations*/, uint32_t id) {
    auto iface = getLocHidlFlpIface();
    if (iface == nullptr) {
        ALOGE("ILocHidlFlpService Iface NULL!");
        return 1;
    }
    iface->getAllBatchedLocations(id);
    return 0;
}

void HidlAPI::flpCleanUp() {
    auto iface = getBatchingIface();
    if (iface == nullptr) {
        ALOGE("IGnssBatching Iface NULL!");
        return;
    }
    iface->cleanup();
}

void HidlAPI::addGeofence(uint32_t geofenceId, double latitudeDegrees, double longitudeDegrees,
        double radiusMeters, GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs) {
    auto iface = getGeofenceIface();
    if (iface == nullptr) {
        ALOGE("IGnssGeofencing Iface NULL!");
        return;
    }
    iface->addGeofence(geofenceId, latitudeDegrees, longitudeDegrees, radiusMeters,
           static_cast<IGnssGeofenceCallback::GeofenceTransition>(0x100), 0x111,
           notificationResponsivenessMs, 100);
}
void HidlAPI::removeGeofence(uint32_t geofenceId) {
    auto iface = getGeofenceIface();
    if (iface == nullptr) {
        ALOGE("IGnssGeofencing Iface NULL!");
        return;
    }
    iface->removeGeofence(geofenceId);
}
void HidlAPI::modifyGeofence(uint32_t geofenceId, GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs) {
    auto iface = getLocHidlGeofenceIface();
    if (iface == nullptr) {
        ALOGE("ILocHidlGeofenceService Iface NULL!");
        return;
    }
    iface->updateGeofence(geofenceId, monitorTransitions, notificationResponsivenessMs);
}
void HidlAPI::pauseGeofence(uint32_t geofenceId) {
    auto iface = getGeofenceIface();
    if (iface == nullptr) {
        ALOGE("IGnssGeofencing Iface NULL!");
        return;
    }
    iface->pauseGeofence(geofenceId);
}
void HidlAPI::resumeGeofence(uint32_t geofenceId, GeofenceBreachTypeMask monitorTransitions) {
    auto iface = getGeofenceIface();
    if (iface == nullptr) {
        ALOGE("IGnssGeofencing Iface NULL!");
        return;
    }
    iface->resumeGeofence(geofenceId, monitorTransitions);
}
void HidlAPI::removeAllGeofences() {
}
} // namespace garden
