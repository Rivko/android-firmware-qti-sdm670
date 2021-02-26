/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_Utils"
#define LOG_NDEBUG 0

#include "LocHidlUtils.h"
#include <log_util.h>
#include <inttypes.h>
#include <android/hidl/base/1.0/IBase.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

void LocHidlDeathRecipient::registerToPeer(const sp<::android::hidl::base::V1_0::IBase>& peer,
        uint64_t cookie) {
    mPeerDied = true;
    if (peer == nullptr) {
        ALOGE("Peer is null");
        return;
    }
    auto r = peer->linkToDeath(this, cookie);
    if (!r.isOk() || r == false) {
        ALOGE("Failed to register death recipient [%s]", r.description().c_str());
    } else {
        mPeerDied = false;
    }
}

void LocHidlDeathRecipient::unregisterFromPeer(const sp<::android::hidl::base::V1_0::IBase>& peer) {
    mPeerDied = true;
    if (peer == nullptr) {
        ALOGE("Peer is null");
        return;
    }
    auto r = peer->unlinkToDeath(this);
    if (!r.isOk() || r == false) {
        ALOGE("Failed to unregister death recipient [%s]", r.description().c_str());
    }
}

void LocHidlDeathRecipient::serviceDied(uint64_t cookie,
        const ::android::wp<::android::hidl::base::V1_0::IBase>& who) {
    mPeerDied = true;
    LOC_LOGE("%s] Peer died. cookie: %" PRIu64 ", who: %p",
            __FUNCTION__, cookie, &who);
    if (mFun != nullptr) {
        mFun();
    }
}

void LocHidlUtils::locationToLocHidlLocation(
        const Location& l, LocHidlLocation& gl) {

    gl.locationFlagsMask = l.flags;
    gl.timestamp = l.timestamp;
    gl.latitude = l.latitude;
    gl.longitude = l.longitude;
    gl.altitude = l.altitude;
    gl.speed = l.speed;
    gl.bearing = l.bearing;
    gl.accuracy = l.accuracy;
    gl.verticalAccuracy = l.verticalAccuracy;
    gl.speedAccuracy = l.speedAccuracy;
    gl.bearingAccuracy = l.bearingAccuracy;
    gl.locationTechnologyMask = l.techMask;
}

void LocHidlUtils::izatLocationToGnssIzatLocation(
        const IzatLocation& il, LocHidlIzatLocation& gil) {

    gil.hasUtcTimestampInMsec = il.mHasUtcTimestampInMsec;
    gil.utcTimestampInMsec = il.mUtcTimestampInMsec;

    gil.hasElapsedRealTimeInNanoSecs = il.mHasElapsedRealTimeInNanoSecs;
    gil.elapsedRealTimeInNanoSecs = il.mElapsedRealTimeInNanoSecs;

    gil.hasLatitude = il.mHasLatitude;
    gil.latitude = il.mLatitude;

    gil.hasLongitude = il.mHasLongitude;
    gil.longitude = il.mLongitude;

    gil.hasHorizontalAccuracy = il.mHasHorizontalAccuracy;
    gil.horizontalAccuracy = il.mHorizontalAccuracy;

    gil.hasAltitudeWrtEllipsoid = il.mHasAltitudeWrtEllipsoid;
    gil.altitudeWrtEllipsoid = il.mAltitudeWrtEllipsoid;

    gil.hasAltitudeWrtMeanSeaLevel = il.mHasAltitudeWrtMeanSeaLevel;
    gil.altitudeWrtMeanSeaLevel = il.mAltitudeWrtMeanSeaLevel;

    gil.hasBearing = il.mHasBearing;
    gil.bearing = il.mBearing;

    gil.hasSpeed = il.mHasSpeed;
    gil.speed = il.mSpeed;

    gil.position_source = il.mPositionSource;

    gil.hasAltitudeMeanSeaLevel = il.mHasAltitudeMeanSeaLevel;
    gil.altitudeMeanSeaLevel = il.mAltitudeMeanSeaLevel;

    gil.hasDop = il.mHasDop;
    gil.pDop = il.mPdop;
    gil.hDop = il.mHdop;
    gil.vDop = il.mVdop;

    gil.hasMagneticDeviation = il.mHasMagneticDeviation;
    gil.magneticDeviation = il.mMagneticDeviation;

    gil.hasVertUnc = il.mHasVertUnc;
    gil.vertUnc = il.mVertUnc;

    gil.hasSpeedUnc = il.mHasSpeedUnc;
    gil.speedUnc = il.mSpeedUnc;

    gil.hasBearingUnc = il.mHasBearingUnc;
    gil.bearingUnc = il.mBearingUnc;

    gil.hasHorizontalReliability = il.mHasHorizontalReliability;
    gil.horizontalReliability = il.mHorizontalReliability;

    gil.hasVerticalReliability = il.mHasVerticalReliability;
    gil.verticalReliability = il.mVerticalReliability;

    gil.hasHorUncEllipseSemiMajor = il.mHasHorUncEllipseSemiMajor;
    gil.horUncEllipseSemiMajor = il.mHorUncEllipseSemiMajor;

    gil.hasHorUncEllipseSemiMinor = il.mHasHorUncEllipseSemiMinor;
    gil.horUncEllipseSemiMinor = il.mHorUncEllipseSemiMinor;

    gil.hasHorUncEllipseOrientAzimuth = il.mHasHorUncEllipseOrientAzimuth;
    gil.horUncEllipseOrientAzimuth = il.mHorUncEllipseOrientAzimuth;

    gil.hasNetworkPositionSource = il.mHasNetworkPositionSource;
    gil.networkPositionSource =
            static_cast<LocHidlNetworkPositionSourceType>(il.mNetworkPositionSource);

//    gil.hasNavSolutionMask = il.mHasNavSolutionMask;
//    gil.navSolutionMask = il.mNavSolutionMask;
//
//    gil.hasPositionTechMask = il.mHasPositionTechMask;
//    gil.positionTechMask = il.mPositionTechMask;
}

DataItemId LocHidlUtils::translateToDataItemId(LocHidlSubscriptionDataItemId id) {

    DataItemId ret = INVALID_DATA_ITEM_ID;
    switch (id) {
        case LocHidlSubscriptionDataItemId::AIRPLANEMODE_DATA_ITEM_ID :
            ret = AIRPLANEMODE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::ENH_DATA_ITEM_ID :
            ret = ENH_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::GPSSTATE_DATA_ITEM_ID :
            ret = GPSSTATE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::NLPSTATUS_DATA_ITEM_ID :
            ret = NLPSTATUS_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::WIFIHARDWARESTATE_DATA_ITEM_ID :
            ret = WIFIHARDWARESTATE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::NETWORKINFO_DATA_ITEM_ID :
            ret = NETWORKINFO_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::RILVERSION_DATA_ITEM_ID :
            ret = RILVERSION_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::RILSERVICEINFO_DATA_ITEM_ID :
            ret = RILSERVICEINFO_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::RILCELLINFO_DATA_ITEM_ID :
            ret = RILCELLINFO_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::SERVICESTATUS_DATA_ITEM_ID :
            ret = SERVICESTATUS_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::MODEL_DATA_ITEM_ID :
            ret = MODEL_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::MANUFACTURER_DATA_ITEM_ID :
            ret = MANUFACTURER_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::VOICECALL_DATA_ITEM :
            ret = VOICECALL_DATA_ITEM;
            break;
        case LocHidlSubscriptionDataItemId::ASSISTED_GPS_DATA_ITEM_ID :
            ret = ASSISTED_GPS_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::SCREEN_STATE_DATA_ITEM_ID :
            ret = SCREEN_STATE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::POWER_CONNECTED_STATE_DATA_ITEM_ID :
            ret = POWER_CONNECTED_STATE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::TIMEZONE_CHANGE_DATA_ITEM_ID :
            ret = TIMEZONE_CHANGE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::TIME_CHANGE_DATA_ITEM_ID :
            ret = TIME_CHANGE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID :
            ret = WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::SHUTDOWN_STATE_DATA_ITEM_ID :
            ret = SHUTDOWN_STATE_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::TAC_DATA_ITEM_ID :
            ret = TAC_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::MCCMNC_DATA_ITEM_ID :
            ret = MCCMNC_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::BTLE_SCAN_DATA_ITEM_ID :
            ret = BTLE_SCAN_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::BT_SCAN_DATA_ITEM_ID :
            ret = BT_SCAN_DATA_ITEM_ID;
            break;
        case LocHidlSubscriptionDataItemId::MAX_DATA_ITEM_ID :
            ret = MAX_DATA_ITEM_ID;
            break;
        default:
            ret = INVALID_DATA_ITEM_ID;
            break;
    }
    return ret;
}

LocHidlSubscriptionDataItemId
LocHidlUtils::translateToSubscriptionDataItemId(DataItemId id) {

    LocHidlSubscriptionDataItemId ret =
            LocHidlSubscriptionDataItemId::INVALID_DATA_ITEM_ID;
    switch (id) {
        case AIRPLANEMODE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::AIRPLANEMODE_DATA_ITEM_ID;
            break;
        case ENH_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::ENH_DATA_ITEM_ID;
            break;
        case GPSSTATE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::GPSSTATE_DATA_ITEM_ID;
            break;
        case NLPSTATUS_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::NLPSTATUS_DATA_ITEM_ID;
            break;
        case WIFIHARDWARESTATE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::WIFIHARDWARESTATE_DATA_ITEM_ID;
            break;
        case NETWORKINFO_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::NETWORKINFO_DATA_ITEM_ID;
            break;
        case RILVERSION_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::RILVERSION_DATA_ITEM_ID;
            break;
        case RILSERVICEINFO_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::RILSERVICEINFO_DATA_ITEM_ID;
            break;
        case RILCELLINFO_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::RILCELLINFO_DATA_ITEM_ID;
            break;
        case SERVICESTATUS_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::SERVICESTATUS_DATA_ITEM_ID;
            break;
        case MODEL_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::MODEL_DATA_ITEM_ID;
            break;
        case MANUFACTURER_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::MANUFACTURER_DATA_ITEM_ID;
            break;
        case VOICECALL_DATA_ITEM :
            ret = LocHidlSubscriptionDataItemId::VOICECALL_DATA_ITEM;
            break;
        case ASSISTED_GPS_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::ASSISTED_GPS_DATA_ITEM_ID;
            break;
        case SCREEN_STATE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::SCREEN_STATE_DATA_ITEM_ID;
            break;
        case POWER_CONNECTED_STATE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::POWER_CONNECTED_STATE_DATA_ITEM_ID;
            break;
        case TIMEZONE_CHANGE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::TIMEZONE_CHANGE_DATA_ITEM_ID;
            break;
        case TIME_CHANGE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::TIME_CHANGE_DATA_ITEM_ID;
            break;
        case WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID;
            break;
        case SHUTDOWN_STATE_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::SHUTDOWN_STATE_DATA_ITEM_ID;
            break;
        case TAC_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::TAC_DATA_ITEM_ID;
            break;
        case MCCMNC_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::MCCMNC_DATA_ITEM_ID;
            break;
        case BTLE_SCAN_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::BTLE_SCAN_DATA_ITEM_ID;
            break;
        case BT_SCAN_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::BT_SCAN_DATA_ITEM_ID;
            break;
        case MAX_DATA_ITEM_ID :
            ret = LocHidlSubscriptionDataItemId::MAX_DATA_ITEM_ID;
            break;
        default:
            break;
    }
    return ret;
}

void LocHidlUtils::izatRequestToGnssIzatRequest(
        const IzatRequest& ir, LocHidlIzatRequest& gir) {

    gir.provider = static_cast<LocHidlIzatStreamType>(ir.getProvider());
    gir.numUpdates = ir.getNumUpdates();
    gir.suggestedResponseTimeForFirstFix = ir.getTimeForFirstFix();
    gir.timeIntervalBetweenFixes = ir.getInterval();
    gir.smallestDistanceBetweenFixes = ir.getDistance();
    gir.suggestedHorizontalAccuracy =
            static_cast<LocHidlIzatHorizontalAccuracy>(ir.getHorizontalAccuracy());
    gir.suggestedAltitudeAccuracy =
            static_cast<LocHidlIzatOtherAccuracy>(ir.getAltitudeAccuracy());
    gir.suggestedBearingAccuracy =
            static_cast<LocHidlIzatOtherAccuracy>(ir.getBearingAccuracy());
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
