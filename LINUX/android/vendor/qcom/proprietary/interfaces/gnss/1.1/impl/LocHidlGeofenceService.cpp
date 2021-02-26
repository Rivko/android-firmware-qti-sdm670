/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_GeofenceService"
#define LOG_NDEBUG 0
#include "LocHidlGeofenceService.h"
#include <log_util.h>
#include <gps_extended_c.h>
#include <fused_location_extended.h>
#include <LocationAPI.h>
#include <LocHidlUtils.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_1 {
namespace implementation {

LocHidlGeofenceService::LocHidlGeofenceService() :
        mIzatGeofenceApiClient(nullptr), mCallbackIface(nullptr) {

    ENTRY_LOG();
}

LocHidlGeofenceService::~LocHidlGeofenceService() {

    ENTRY_LOG();
    if (mIzatGeofenceApiClient != nullptr) {
        delete mIzatGeofenceApiClient;
        mIzatGeofenceApiClient = nullptr;
    }
}

// Methods from ::vendor::qti::gnss::V1_0::IGnssGeofenceServiceProvider follow.
Return<bool> LocHidlGeofenceService::init(
        const sp<ILocHidlGeofenceServiceCallback>& callback) {
    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    if (mIzatGeofenceApiClient != nullptr) {
        LOC_LOGE("mIzatGeofenceApiClient not null !");
        return false;
    }
    mIzatGeofenceApiClient = new LocHidlGeofenceClient(callback);

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([this] {
                LOC_LOGE("%s]: ILocHidlGeofenceServiceCallback died.", __func__);
                removeAllGeofences();
                if (mIzatGeofenceApiClient != nullptr) {
                    delete mIzatGeofenceApiClient;
                    mIzatGeofenceApiClient = nullptr;
                }
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);

    mCallbackIface = callback;

    return true;
}

Return<void> LocHidlGeofenceService::addGeofence(
        int32_t id, double latitude, double longitude, double radius,
        uint32_t transitionTypes, int32_t responsiveness, int32_t /*confidence*/,
        int32_t dwellTime, uint32_t /*dwellTimeMask*/) {

    FROM_HIDL_CLIENT();

    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGE("Geofence API Client null !");
        return Void();
    }

    GeofenceOption options;
    memset(&options, 0, sizeof(GeofenceOption));
    options.size = sizeof(GeofenceOption);
    if (transitionTypes & LOC_GPS_GEOFENCE_ENTERED)
        options.breachTypeMask |= GEOFENCE_BREACH_ENTER_BIT;
    if (transitionTypes & LOC_GPS_GEOFENCE_EXITED)
        options.breachTypeMask |=  GEOFENCE_BREACH_EXIT_BIT;
    options.responsiveness = responsiveness;
    options.dwellTime = dwellTime;

    GeofenceInfo data;
    data.size = sizeof(GeofenceInfo);
    data.latitude = latitude;
    data.longitude = longitude;
    data.radius = radius;

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIAddGeofences(1, &gfId, &options, &data);
    return Void();
}

Return<void> LocHidlGeofenceService::removeGeofence(int32_t id) {
    FROM_HIDL_CLIENT();

    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGE("Geofence API Client null !");
    return Void();
}

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIRemoveGeofences(1, &gfId);

    return Void();
}

Return<void> LocHidlGeofenceService::updateGeofence(
        int32_t id, uint32_t transitionTypes, int32_t responsiveness) {

    FROM_HIDL_CLIENT();

    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGE("Geofence API Client null !");
        return Void();
    }

    GeofenceOption options;
    memset(&options, 0, sizeof(GeofenceOption));
    options.size = sizeof(GeofenceOption);
    if (transitionTypes & LOC_GPS_GEOFENCE_ENTERED)
        options.breachTypeMask |= GEOFENCE_BREACH_ENTER_BIT;
    if (transitionTypes & LOC_GPS_GEOFENCE_EXITED)
        options.breachTypeMask |=  GEOFENCE_BREACH_EXIT_BIT;
    options.responsiveness = responsiveness;

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIModifyGeofences(1, &gfId, &options);
    return Void();
}

Return<void> LocHidlGeofenceService::pauseGeofence(int32_t id) {
    FROM_HIDL_CLIENT();

    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGE("Geofence API Client null !");
    return Void();
}

    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIPauseGeofences(1, &gfId);
    return Void();
}

Return<void> LocHidlGeofenceService::resumeGeofence(
        int32_t id, uint32_t transitionTypes) {

    FROM_HIDL_CLIENT();

    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGE("Geofence API Client null !");
        return Void();
    }

    GeofenceBreachTypeMask mask = 0;
    if (transitionTypes & LOC_GPS_GEOFENCE_ENTERED)
        mask |= GEOFENCE_BREACH_ENTER_BIT;
    if (transitionTypes & LOC_GPS_GEOFENCE_EXITED)
        mask |=  GEOFENCE_BREACH_EXIT_BIT;
    uint32_t gfId = id;
    mIzatGeofenceApiClient->locAPIResumeGeofences(1, &gfId, &mask);

    return Void();
}

Return<void> LocHidlGeofenceService::removeAllGeofences()  {
    if (mIzatGeofenceApiClient == nullptr) {
        LOC_LOGE("Geofence API Client null !");
        return Void();
    }

    mIzatGeofenceApiClient->locAPIRemoveAllGeofences();

    return Void();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
