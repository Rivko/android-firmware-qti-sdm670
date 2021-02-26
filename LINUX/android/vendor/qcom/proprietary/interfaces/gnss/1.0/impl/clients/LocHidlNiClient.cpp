/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_NiClient"
#define LOG_NDEBUG 0

#include "LocHidlNiClient.h"
#include "LocHidlUtils.h"
#include <LocDualContext.h>
#include <LocationAPI.h>
#include <log_util.h>
#include <loc_gps.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;

LocHidlNiClient::LocHidlNiClient(
        const sp<ILocHidlGnssNiCallback>& callback) :
                LocationAPIClientBase(), mGnssCbIface(callback) {

    ENTRY_LOG();

    LocationCallbacks locationCallbacks;
    memset(&locationCallbacks, 0, sizeof(LocationCallbacks));
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;

    locationCallbacks.gnssNiCb = [this](uint32_t id, GnssNiNotification gnssNiNotification) {
        onGnssNiCb(id, gnssNiNotification);
    };

    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

void LocHidlNiClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {

    LOC_LOGV("%s] capabilities mask 0x%x", __FUNCTION__, capabilitiesMask);

    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }

    uint32_t capabilities = 0;
    if ((capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT) ||
            (capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT) ||
            (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT) ||
            (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT))
        capabilities |= LOC_GPS_CAPABILITY_SCHEDULING;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GEOFENCE_BIT)
        capabilities |= LOC_GPS_CAPABILITY_GEOFENCING;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MEASUREMENTS_BIT)
        capabilities |= LOC_GPS_CAPABILITY_MEASUREMENTS;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
        capabilities |= LOC_GPS_CAPABILITY_MSB;
    if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSA_BIT)
        capabilities |= LOC_GPS_CAPABILITY_MSA;

    TO_HIDL_CLIENT();
    auto r = mGnssCbIface->gnssCapabilitiesCb(capabilities);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlNiClient::onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification)
{
    ENTRY_LOG();

    ILocHidlGnssNiCallback::GnssNiNotificationExt cbNotification;
    memset(&cbNotification, 0, sizeof(cbNotification));

    cbNotification.notificationId = id;

    if (gnssNiNotification.type == GNSS_NI_TYPE_VOICE)
        cbNotification.niType = ILocHidlGnssNiCallback::GnssNiTypeExt::VOICE;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_SUPL)
        cbNotification.niType = ILocHidlGnssNiCallback::GnssNiTypeExt::UMTS_SUPL;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_CONTROL_PLANE)
        cbNotification.niType = ILocHidlGnssNiCallback::GnssNiTypeExt::UMTS_CTRL_PLANE;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_EMERGENCY_SUPL)
        cbNotification.niType = ILocHidlGnssNiCallback::GnssNiTypeExt::EMERGENCY_SUPL;

    if (gnssNiNotification.options & GNSS_NI_OPTIONS_NOTIFICATION_BIT)
        cbNotification.notifyFlags |=
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::NEED_NOTIFY);
    if (gnssNiNotification.options & GNSS_NI_OPTIONS_VERIFICATION_BIT)
        cbNotification.notifyFlags |=
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::NEED_VERIFY);
    if (gnssNiNotification.options & GNSS_NI_OPTIONS_PRIVACY_OVERRIDE_BIT)
        cbNotification.notifyFlags |=
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::PRIVACY_OVERRIDE);

    cbNotification.timeoutSec = gnssNiNotification.timeout;

    if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_ACCEPT)
        cbNotification.defaultResponse = IGnssNiCallback::GnssUserResponseType::RESPONSE_ACCEPT;
    else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_DENY)
        cbNotification.defaultResponse = IGnssNiCallback::GnssUserResponseType::RESPONSE_DENY;
    else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_NO_RESPONSE ||
            gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_IGNORE)
        cbNotification.defaultResponse = IGnssNiCallback::GnssUserResponseType::RESPONSE_NORESP;

    cbNotification.requestorId = gnssNiNotification.requestor;
    cbNotification.notificationMessage = gnssNiNotification.message;

    if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_NONE)
        cbNotification.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_NONE;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
        cbNotification.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_GSM_DEFAULT;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
        cbNotification.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UTF8;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
        cbNotification.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UCS2;

    if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_NONE)
        cbNotification.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_NONE;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
        cbNotification.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_GSM_DEFAULT;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
        cbNotification.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UTF8;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
        cbNotification.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UCS2;

    cbNotification.extras = gnssNiNotification.extras;
    cbNotification.esEnabled = loc_core::ContextBase::mGps_conf.SUPL_ES;

    TO_HIDL_CLIENT();
    auto r = mGnssCbIface->niNotifyCbExt(cbNotification);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlNiClient::gnssNiRespond(int32_t notifId,
        IGnssNiCallback::GnssUserResponseType userResponse) {

    LOC_LOGD("%s]: (%d %d)", __FUNCTION__, notifId, static_cast<int>(userResponse));

    GnssNiResponse data = GNSS_NI_RESPONSE_IGNORE;
    if (userResponse == IGnssNiCallback::GnssUserResponseType::RESPONSE_ACCEPT)
        data = GNSS_NI_RESPONSE_ACCEPT;
    else if (userResponse == IGnssNiCallback::GnssUserResponseType::RESPONSE_DENY)
        data = GNSS_NI_RESPONSE_DENY;
    else if (userResponse == IGnssNiCallback::GnssUserResponseType::RESPONSE_NORESP)
        data = GNSS_NI_RESPONSE_NO_RESPONSE;

    locAPIGnssNiResponse(notifId, data);
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
