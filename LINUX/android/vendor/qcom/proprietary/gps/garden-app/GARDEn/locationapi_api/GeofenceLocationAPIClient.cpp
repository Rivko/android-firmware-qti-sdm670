/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GeofenceLocationAPIClient.h"
#include <loc_pla.h>
#include <log_util.h>
#include "GnssCase.h"


namespace garden {

void GeofenceLocationAPIClient::setCallbacks(GnssCbBase* callbacks) {
    LOC_LOGD("%s]: (%p)", __func__, callbacks);
    {
        lock_guard guard(mGeofenceLock);
        mGeofenceCbs = callbacks;
    }
    LocationCallbacks locationCallbacks;
    locationCallbacks.size = sizeof(LocationCallbacks);
    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssDataCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;
    if(callbacks != nullptr) {
        locationCallbacks.geofenceBreachCb =
            [this](GeofenceBreachNotification geofenceBreachNotification) {
                onGeofenceBreachCb(geofenceBreachNotification);
            };

        locationCallbacks.geofenceStatusCb =
            [this](GeofenceStatusNotification geofenceStatusNotification) {
                onGeofenceStatusCb(geofenceStatusNotification);
            };
    }
    locAPISetCallbacks(locationCallbacks);
}

void GeofenceLocationAPIClient::addGeofence(uint32_t geofenceId,
            double latitudeDegrees, double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs) {
    LOC_LOGD("%s]: (%d %f %f %f %d %d)", __FUNCTION__, geofenceId,
            latitudeDegrees, longitudeDegrees, radiusMeters, monitorTransitions,
            notificationResponsivenessMs);
    GeofenceOption options;
    memset(&options, 0, sizeof(GeofenceOption));
    options.size = sizeof(GeofenceOption);
    options.breachTypeMask = monitorTransitions;
    options.responsiveness = notificationResponsivenessMs;

    GeofenceInfo data;
    data.size = sizeof(GeofenceInfo);
    data.latitude = latitudeDegrees;
    data.longitude = longitudeDegrees;
    data.radius = radiusMeters;

    LocationError err = (LocationError)locAPIAddGeofences(1, &geofenceId, &options, &data);
    if(LOCATION_ERROR_SUCCESS != err) {
        onAddGeofencesCb(1, &err, &geofenceId);
    }
}

void GeofenceLocationAPIClient::removeGeofence(uint32_t geofenceId) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, geofenceId);
    locAPIRemoveGeofences(1, &geofenceId);
}

void GeofenceLocationAPIClient::modifyGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs) {
    LOC_LOGD("%s]: (%d  %d %d )", __FUNCTION__, geofenceId,
            monitorTransitions, notificationResponsivenessMs);
    GeofenceOption options;
    memset(&options, 0, sizeof(GeofenceOption));
    options.size = sizeof(GeofenceOption);
    options.breachTypeMask = monitorTransitions;
    options.responsiveness = notificationResponsivenessMs;

    locAPIModifyGeofences(1, &geofenceId, &options);
}

void GeofenceLocationAPIClient::pauseGeofence(uint32_t geofenceId) {
    LOC_LOGD("%s]: (%d)", __func__, geofenceId);
    locAPIPauseGeofences(1, &geofenceId);
}

void GeofenceLocationAPIClient::resumeGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitorTransitions) {
    LOC_LOGD("%s]: (%d %d)", __func__, geofenceId, monitorTransitions);
    locAPIResumeGeofences(1, &geofenceId, &monitorTransitions);
}

void GeofenceLocationAPIClient::removeAllGeofences() {
    LOC_LOGD("%s]", __FUNCTION__);
}



// callbacks
void GeofenceLocationAPIClient::onGeofenceBreachCb(GeofenceBreachNotification geofenceBreachNotification)
{
    LOC_LOGD("%s]: (%zu)", __func__, geofenceBreachNotification.count);
    lock_guard guard(mGeofenceLock);
    if (mGeofenceCbs) {
        for (size_t i = 0; i < geofenceBreachNotification.count; i++) {
            GeofenceBreachType transition;
            if(geofenceBreachNotification.type == GEOFENCE_BREACH_ENTER ||
                    geofenceBreachNotification.type == GEOFENCE_BREACH_EXIT) {
                transition = geofenceBreachNotification.type;
            }
            else {
                // continue with other breach if transition is
                // nether GPS_GEOFENCE_ENTERED nor GPS_GEOFENCE_EXITED
                continue;
            }
            mGeofenceCbs->geofenceBreachCb(geofenceBreachNotification.ids[i],
                    geofenceBreachNotification.location, transition,
                    geofenceBreachNotification.timestamp);
        }
    }
}

void GeofenceLocationAPIClient::onGeofenceStatusCb(GeofenceStatusNotification geofenceStatusNotification)
{
    LOC_LOGD("%s]: (%d)", __func__, geofenceStatusNotification.available);
    lock_guard guard(mGeofenceLock);
    if (mGeofenceCbs) {
        mGeofenceCbs->geofenceStatusCb(geofenceStatusNotification.available);
    }
}

void GeofenceLocationAPIClient::onAddGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    lock_guard guard(mGeofenceLock);
    if (mGeofenceCbs) {
        for (size_t i = 0; i < count; i++) {
            mGeofenceCbs->addGeofencesCb(ids[i], errors[i]);
        }
    }
}

void GeofenceLocationAPIClient::onRemoveGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    lock_guard guard(mGeofenceLock);
    if (mGeofenceCbs) {
        for (size_t i = 0; i < count; i++) {
            mGeofenceCbs->removeGeofencesCb(ids[i], errors[i]);
        }
    }
}

void GeofenceLocationAPIClient::onPauseGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    lock_guard guard(mGeofenceLock);
    if (mGeofenceCbs) {
        for (size_t i = 0; i < count; i++) {
            mGeofenceCbs->pauseGeofencesCb(ids[i], errors[i]);
        }
    }
}

void GeofenceLocationAPIClient::onResumeGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    lock_guard guard(mGeofenceLock);
    if (mGeofenceCbs) {
        for (size_t i = 0; i < count; i++) {
            mGeofenceCbs->resumeGeofencesCb(ids[i], errors[i]);
        }
    }
}

void GeofenceLocationAPIClient::onModifyGeofencesCb(size_t count, LocationError* errors,
        uint32_t* ids) {
    LOC_LOGD("%s]: (%zu)", __func__, count);
    lock_guard guard(mGeofenceLock);
    if (mGeofenceCbs) {
        for (size_t i = 0; i < count; i++) {
            mGeofenceCbs->modifyGeofencesCb(ids[i], errors[i]);
        }
    }
}

} //namespace garden
