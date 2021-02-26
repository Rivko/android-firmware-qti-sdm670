/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GnssLocationAPI.h"
#include <dlfcn.h>
#include <loc_pla.h>
#include <log_util.h>
#include "GardenUtil.h"
#include <GeofenceCb.h>
#include "GeofenceLocationAPIClient.h"

namespace garden {

void GnssLocationAPI::setGnssCbs(GnssCbBase* callbacks) {
    getGnssClient()->setCallbacks(callbacks);
}

void GnssLocationAPI::setFlpCbs(GnssCbBase* callbacks) {
    getFlpClient()->setCallbacks(callbacks);
}

void GnssLocationAPI::setGeofenceCbs(GnssCbBase* callbacks) {
    getGeofenceClient()->setCallbacks(callbacks);
}

GnssLocationAPI::GnssLocationAPI() {}

GnssLocationAPI::~GnssLocationAPI() {
    if(mGnssAPI) {
        delete mGnssAPI;
        mGnssAPI = nullptr;
    }
    if(mFlpAPI) {
        delete mFlpAPI;
        mFlpAPI = nullptr;
    }
    if(mGeofenceAPI) {
        delete mGeofenceAPI;
        mGeofenceAPI = nullptr;
    }
}

int GnssLocationAPI::gnssStart() {
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;
    retVal = getGnssClient()->gnssStart();
    return retVal;
}

int GnssLocationAPI::gnssStop() {
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;
    getGnssClient()->locAPIStopTracking();
    return retVal;
}

void GnssLocationAPI::gnssCleanup() {
    getGnssClient()->mControlClient->locAPIDisable();
}

int GnssLocationAPI::gnssInjectLocation(double latitude, double longitude,
        float accuracy) {
    ENTRY_LOG_CALLFLOW();
    return getGnssClient()->gnssInjectLocation(latitude, longitude, accuracy);
}

void GnssLocationAPI::gnssDeleteAidingData(LocGpsAidingData flags) {
    LOC_LOGD("%s]: (%02x)", __func__, flags);
    getGnssClient()->gnssDeleteAidingData(flags);
}

int GnssLocationAPI::gnssSetPositionMode(LocGpsPositionMode mode,
        LocGpsPositionRecurrence recurrence, uint32_t min_interval,
        uint32_t preferred_accuracy, uint32_t preferred_time) {
    LOC_LOGD("%s]: (%d %d %d %d %d)", __func__,
            mode, recurrence, min_interval, preferred_accuracy, preferred_time);
    int retVal = 0;
    if(!mGnssAPI)
        return -1;
    getGnssClient()->gnssSetPositionMode(mode, recurrence, min_interval, preferred_accuracy,
            preferred_time);
    return retVal;
}

void GnssLocationAPI::configurationUpdate(const char* config_data, int32_t length) {
    LOC_LOGD("%s]: (%s %d)", __func__, config_data, length);
    if(!mGnssAPI)
        return;
    getGnssClient()->configurationUpdate(config_data, length);
}

void GnssLocationAPI::updateNetworkAvailability(int /*available*/, const char* /*apn*/) {
}

void GnssLocationAPI::gnssNiResponse(uint32_t id, GnssNiResponse response) {
    getGnssClient()->locAPIGnssNiResponse(id, response);
}


//flp
int GnssLocationAPI::flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id) {
    gardenPrint("%s ()]:, id: %u, mode: %d, size: %d, minInterval: %d, minDistance: %d,"
           " suplMode: %d", __func__, id, mode, options.size, options.minInterval,
            options.minDistance, options.mode);
    return getFlpClient()->flpStartSession(mode, options, id);
}

int GnssLocationAPI::flpStopSession(uint32_t id) {
    return getFlpClient()->flpStopSession(id);
}

int GnssLocationAPI::flpGetBatchSize() {
    int batchSize = 0;
    gardenPrint("calling %s...", __func__);
    return getFlpClient()->getBatchSize();
}

int GnssLocationAPI::flpUpdateSession(uint32_t sessionMode, LocationOptions& options, uint32_t id) {
    LOC_LOGD("%s]: (%u %ld %d)", __FUNCTION__,
            id, options.minInterval*1000000L, sessionMode);
    return getFlpClient()->updateSession(sessionMode, options, id);
}

int GnssLocationAPI::flpGetBatchedLocation(int lastNLocations, uint32_t id) {
    LOC_LOGD("%s]: (%u %d)", __FUNCTION__, id, lastNLocations);
    return getFlpClient()->getBatchedLocation(lastNLocations, id);
}

void GnssLocationAPI::flpCleanUp() {
    LOC_LOGD("%s]:", __FUNCTION__);
    if(mFlpAPI) {
        delete mFlpAPI;
        mFlpAPI = nullptr;
    }
}


//geofence
void GnssLocationAPI::addGeofence(uint32_t geofenceId, double latitudeDegrees,
            double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs) {
    LOC_LOGD("%s]: (%d %f %f %f %d %d)", __FUNCTION__, geofenceId,
            latitudeDegrees, longitudeDegrees, radiusMeters, monitorTransitions,
            notificationResponsivenessMs);
    getGeofenceClient()->addGeofence(geofenceId, latitudeDegrees, longitudeDegrees,
            radiusMeters, monitorTransitions, notificationResponsivenessMs);
}

void GnssLocationAPI::removeGeofence(uint32_t geofenceId) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, geofenceId);
    getGeofenceClient()->removeGeofence(geofenceId);
}

void GnssLocationAPI::modifyGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs) {
    LOC_LOGD("%s]: (%d  %d %d )", __FUNCTION__, geofenceId,
            monitorTransitions, notificationResponsivenessMs);
    getGeofenceClient()->modifyGeofence(geofenceId,
            monitorTransitions, notificationResponsivenessMs);
}

void GnssLocationAPI::pauseGeofence(uint32_t geofenceId) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    getGeofenceClient()->pauseGeofence(geofenceId);
}

void GnssLocationAPI::resumeGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitorTransitions) {
    LOC_LOGD("%s]: (%d %d)", __func__, geofenceId, monitorTransitions);
    getGeofenceClient()->resumeGeofence(geofenceId, monitorTransitions);
}

void GnssLocationAPI::removeAllGeofences() {
    LOC_LOGD("%s]", __FUNCTION__);
}

GnssLocationAPIClient* GnssLocationAPI::getGnssClient() {
    if(!mGnssAPI) {
        mGnssAPI = new GnssLocationAPIClient();
    }
    return mGnssAPI;
}

FlpLocationAPIClient* GnssLocationAPI::getFlpClient() {
    if(!mFlpAPI) {
        mFlpAPI = new FlpLocationAPIClient();
    }
    return mFlpAPI;
}

GeofenceLocationAPIClient* GnssLocationAPI::getGeofenceClient() {
    if(!mGeofenceAPI) {
        mGeofenceAPI = new GeofenceLocationAPIClient();
    }
    return mGeofenceAPI;
}

} // namespace garden
