/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_FlpService"
#define LOG_NDEBUG 0
#include "LocHidlFlpService.h"
#include <log_util.h>
#include <gps_extended_c.h>
#include <fused_location_extended.h>
#include <LocationAPI.h>
#include <dlfcn.h>
#include <LocHidlUtils.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_1 {
namespace implementation {

#define FLP_SERVICE_PROVIDER_ERROR 1
#define FLP_SERVICE_PROVIDER_SUCCESS 0

sp<ILocHidlFlpServiceCallback> LocHidlFlpService::sCallbackIface = nullptr;
sp<LocHidlDeathRecipient> LocHidlFlpService::mDeathRecipient = nullptr;

LocHidlFlpService::LocHidlFlpService() :
        mIzatFlpApiClient(nullptr), mCallbackIface(nullptr) {

    ENTRY_LOG();
}

LocHidlFlpService::~LocHidlFlpService() {

    ENTRY_LOG();
    if (mIzatFlpApiClient != nullptr) {
        delete mIzatFlpApiClient;
        mIzatFlpApiClient = nullptr;
    }
}

// Methods from ::vendor::qti::gnss::V1_0::IGnssFlpServiceProvider follow.
Return<bool> LocHidlFlpService::init(const sp<ILocHidlFlpServiceCallback>& callback) {
    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient != nullptr) {
        LOC_LOGE("mIzatFlpApiClient not null !");
        return false;
    }
    mIzatFlpApiClient = new LocHidlFlpClient(callback);

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([this] {
                LOC_LOGE("%s]: ILocHidlFlpServiceCallback died.", __func__);
                if (mIzatFlpApiClient != nullptr) {
                    delete mIzatFlpApiClient;
                    mIzatFlpApiClient = nullptr;
                }
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);
    mCallbackIface = callback;

    return true;
}

Return<uint32_t> LocHidlFlpService::getAllSupportedFeatures() {
    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGE("FLP API Client null !");
        return 0;
    }

    uint32_t result = 0, i = 0;
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT) {
        result |= FLP_TIME_BASED_BATCHING_IS_SUPPORTED;
        i++;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT) {
        result |= FLP_DISTANCE_BASED_BATCHING_IS_SUPPORTED;
        i++;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT) {
        result |= FLP_DISTANCE_BASED_TRACKING_IS_SUPPORTED;
        i++;
    }
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_OUTDOOR_TRIP_BATCHING_BIT) {
        result |= FLP_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED;
        i++;
    }
    if (i > 1)
        result |= FLP_ADAPTIVE_BATCHING_IS_SUPPORTED;

    return result;
}

Return<int32_t> LocHidlFlpService::getAllBatchedLocations(int32 sessionId) {

    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGE("FLP API Client null !");
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    mIzatFlpApiClient->locAPIGetBatchedLocations(sessionId, SIZE_MAX);
    return FLP_SERVICE_PROVIDER_SUCCESS;
}

Return<int32_t> LocHidlFlpService::stopFlpSession(int32_t sessionId) {
    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGE("FLP API Client null !");
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    int32_t result = mIzatFlpApiClient->locAPIStopSession(sessionId);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("locAPIStopSession() failed, ret %d", result);
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    return FLP_SERVICE_PROVIDER_SUCCESS;
}

Return<int32_t> LocHidlFlpService::startFlpSession(
        int32_t id, uint32_t flags, int64_t minIntervalNanos, int32_t minDistanceMetres,
        uint32_t tripDistanceMeters) {

    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGE("FLP API Client null !");
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    LocationOptions options;
    memset(&options, 0, sizeof(LocationOptions));
    options.size = sizeof(LocationOptions);
    options.minInterval = (uint32_t)(minIntervalNanos / 1000000L);
    options.minDistance = minDistanceMetres;
    if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
        options.minDistance = tripDistanceMeters;
    }

    options.mode = GNSS_SUPL_MODE_STANDALONE;
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
        options.mode = GNSS_SUPL_MODE_MSB;

    uint32_t mode = 0;
    if (flags == FLP_EXT_BATCHING_ON_FIX) {
         mode = SESSION_MODE_ON_FIX;
    } else if (flags == FLP_EXT_BATCHING_ON_FULL) {
         mode = SESSION_MODE_ON_FULL;
    }  else if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
         mode = SESSION_MODE_ON_TRIP_COMPLETED;
    } else {
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    int32_t result = mIzatFlpApiClient->locAPIStartSession(id, mode, options);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("startFlpSession() failed, ret %d", result);
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    return FLP_SERVICE_PROVIDER_SUCCESS;
}

Return<int32_t> LocHidlFlpService::updateFlpSession(
        int32_t id, uint32_t flags, int64_t minIntervalNanos, int32_t minDistanceMetres,
        uint32_t tripDistanceMeters) {

    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGE("FLP API Client null !");
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    LocationOptions options;
    memset(&options, 0, sizeof(LocationOptions));
    options.size = sizeof(LocationOptions);
    options.minInterval = (uint32_t)(minIntervalNanos / 1000000L);
    options.minDistance = minDistanceMetres;
    if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
        options.minDistance = tripDistanceMeters;
    }
    options.mode = GNSS_SUPL_MODE_STANDALONE;
    if (mIzatFlpApiClient->mCapabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
        options.mode = GNSS_SUPL_MODE_MSB;

    uint32_t mode = 0;
    if (flags == FLP_EXT_BATCHING_ON_FIX) {
         mode = SESSION_MODE_ON_FIX;
    } else if (flags == FLP_EXT_BATCHING_ON_FULL) {
         mode = SESSION_MODE_ON_FULL;
    } else if (flags == FLP_EXT_BATCHING_ON_TRIP_COMPLETED) {
         mode = SESSION_MODE_ON_TRIP_COMPLETED;
    } else {
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    int32_t result = mIzatFlpApiClient->locAPIUpdateSessionOptions(id, mode, options);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("updateFlpSession() failed, ret %d", result);
        return FLP_SERVICE_PROVIDER_ERROR;
    }

    return FLP_SERVICE_PROVIDER_SUCCESS;
}

Return<void> LocHidlFlpService::deleteAidingData(int64_t flags) {
    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGE("FLP API Client null !");
        return Void();
    }

    GnssAidingData data;
    memset(&data, 0, sizeof (GnssAidingData));
    data.sv.svTypeMask = GNSS_AIDING_DATA_SV_TYPE_GPS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT;

    if (flags == LOC_GPS_DELETE_ALL)
        data.deleteAll = true;
    else {
        if (flags & LOC_GPS_DELETE_EPHEMERIS)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_EPHEMERIS_BIT;
        if (flags & LOC_GPS_DELETE_ALMANAC)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_ALMANAC_BIT;
        if (flags & LOC_GPS_DELETE_POSITION)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_POSITION_BIT;
        if (flags & LOC_GPS_DELETE_TIME)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_TIME_BIT;
        if (flags & LOC_GPS_DELETE_IONO)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_IONOSPHERE_BIT;
        if (flags & LOC_GPS_DELETE_UTC)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_UTC_BIT;
        if (flags & LOC_GPS_DELETE_HEALTH)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_HEALTH_BIT;
        if (flags & LOC_GPS_DELETE_SVDIR)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_DIRECTION_BIT;
        if (flags & LOC_GPS_DELETE_SVSTEER)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_STEER_BIT;
        if (flags & LOC_GPS_DELETE_SADATA)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_SA_DATA_BIT;
        if (flags & LOC_GPS_DELETE_RTI)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_RTI_BIT;
        if (flags & LOC_GPS_DELETE_CELLDB_INFO)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_CELLDB_BIT;
    }

    int32_t result = mIzatFlpApiClient->locAPIGnssDeleteAidingData(data);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("deleteAidingData() failed, ret %d", result);
    }
    return Void();
}

Return<void> LocHidlFlpService::getMaxPowerAllocated() {
    FROM_HIDL_CLIENT();

    if (sCallbackIface != nullptr) {
        LOC_LOGE("Pending callback !");
        return Void();
    }
    sCallbackIface = mCallbackIface;

    FlpInterface* flpInterface = getFlpInterface();
    if (nullptr == flpInterface) {
        LOC_LOGE("Failed to get FLP Interface");
        return Void();
    }
    flpInterface->getPowerStateChanges(
            (void*)LocHidlFlpService::powerStatusCallback);

    return Void();
}

Return<void> LocHidlFlpService::updateXtraThrottle(bool enabled) {
    FROM_HIDL_CLIENT();

    if (mIzatFlpApiClient == nullptr) {
        LOC_LOGE("FLP API Client null !");
        return Void();
    }

    uint32_t result = mIzatFlpApiClient->updateXtraThrottle(enabled);
    if (result != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("updateXtraThrottle() failed, ret %d", result);
    }

    return Void();
}

void LocHidlFlpService::powerStatusCallback(bool isOn) {

    ENTRY_LOG();

    if (mDeathRecipient != nullptr && mDeathRecipient->peerDied()) {
        ALOGE("%s] Peer has gone.", __func__);
        sCallbackIface = nullptr;
        return;
    }

    if (sCallbackIface == nullptr) {
        LOC_LOGE("sCallbackIface NULL");
        return;
    }

    int32_t powerMW = isOn ? FLP_SESSION_POWER_LEVEL_HIGH : 0;

    TO_HIDL_CLIENT();
    auto r = sCallbackIface->gnssMaxPowerAllocatedCb(powerMW);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
    sCallbackIface = nullptr;
}

FlpInterface* LocHidlFlpService::getFlpInterface() {

    ENTRY_LOG();

    if (nullptr == mFlpInterface && !mGetFlpInterfaceFailed) {
        LOC_LOGD("%s]: loading libflp.so::getFlpInterface ...", __func__);
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libflp.so", RTLD_NOW);
        if (NULL == handle || (error = dlerror()) != NULL)  {
            LOC_LOGE("dlopen for libflp.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getFlpInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_LOGE("dlsym for libgnss.so::getFlpInterface failed, error = %s", error);
                getter = NULL;
            }
        }

        if (NULL == getter) {
            mGetFlpInterfaceFailed = true;
        } else {
            mFlpInterface = (FlpInterface*)(*getter)();
        }
    }
    return mFlpInterface;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
