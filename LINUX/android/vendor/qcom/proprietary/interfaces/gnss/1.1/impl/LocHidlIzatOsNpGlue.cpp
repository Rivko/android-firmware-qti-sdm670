/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_IzatOsNpGlue"
#define LOG_NDEBUG 0
#include "LocHidlIzatOsNpGlue.h"
#include <OsNpGlue.h>
#include <IzatLocation.h>
#include <IzatTypes.h>
#include <log_util.h>
#include <LocHidlUtils.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_1 {
namespace implementation {

// Methods from ::vendor::qti::gnss::V1_0::IIzatOsNpGlue follow.
Return<bool> LocHidlIzatOsNpGlue::setCallback(
        const sp<ILocHidlIzatOsNpGlueCallback>& callback){
    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    if (callback == nullptr) {
        return false;
    }

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([] {
                LOC_LOGE("%s]: ILocHidlIzatOsNpGlueCallback died.", __func__);
                OsNpGlue::getInstance()->setOsNp(nullptr);
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);
    mCallback = callback;
    OsNpGlue::getInstance()->setOsNp(mCallback);

    return true;
}

Return<void> LocHidlIzatOsNpGlue::locationChanged(const LocHidlIzatLocation& location)
{
    FROM_HIDL_CLIENT();

    IzatLocation izatLocation;

    izatLocation.mHasUtcTimestampInMsec = location.hasUtcTimestampInMsec;
    izatLocation.mHasElapsedRealTimeInNanoSecs =
            location.hasElapsedRealTimeInNanoSecs;
    izatLocation.mHasLatitude = location.hasLatitude;
    izatLocation.mHasLongitude = location.hasLongitude;
    izatLocation.mHasHorizontalAccuracy = location.hasHorizontalAccuracy;
    izatLocation.mHasVertUnc = location.hasVertUnc;
    izatLocation.mHasAltitudeWrtEllipsoid =
            location.hasAltitudeWrtEllipsoid;
    izatLocation.mHasBearing = location.hasBearing;
    izatLocation.mHasSpeed = location.hasSpeed;
    izatLocation.mHasNetworkPositionSource =
            location.hasNetworkPositionSource;

    izatLocation.mUtcTimestampInMsec = location.utcTimestampInMsec;
    izatLocation.mElapsedRealTimeInNanoSecs =
            location.elapsedRealTimeInNanoSecs;
    izatLocation.mLatitude = location.latitude;
    izatLocation.mLongitude = location.longitude;
    izatLocation.mHorizontalAccuracy = location.horizontalAccuracy;
    izatLocation.mVertUnc = location.vertUnc;
    izatLocation.mAltitudeWrtEllipsoid = location.altitudeWrtEllipsoid;
    izatLocation.mBearing = location.bearing;
    izatLocation.mSpeed = location.speed;
    izatLocation.mNetworkPositionSource =
            (IzatNetworkPositionSourceType)location.networkPositionSource;

    OsNpGlue::getInstance()->onLocationChanged(&izatLocation);

    return Void();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
