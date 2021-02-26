/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IGNSSAPI_H
#define IGNSSAPI_H
#include <loc_gps.h>
#include <gps_extended.h>
#include "GnssCbBase.h"
#include "GnssLocationAPIClient.h"
#include "GeofenceLocationAPIClient.h"
#include "FlpLocationAPIClient.h"

namespace garden {

class IGnssAPI {
public:
    IGnssAPI() = default;
    virtual ~IGnssAPI() = default;
    virtual void setGnssCbs(GnssCbBase* callbacks) = 0;
    virtual void setFlpCbs(GnssCbBase* callbacks) = 0;
    virtual void setGeofenceCbs(GnssCbBase* callbacks) = 0;
    virtual int gnssStart(void) = 0;
    virtual int gnssStop(void) = 0;
    virtual void gnssCleanup(void) = 0;
    virtual int gnssInjectLocation(double latitude, double longitude, float accuracy) = 0;
    virtual void gnssDeleteAidingData(LocGpsAidingData flags) = 0;
    virtual int gnssSetPositionMode(LocGpsPositionMode mode, LocGpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time) = 0;

    virtual void configurationUpdate(const char* config_data, int32_t length) = 0;
    virtual void updateNetworkAvailability(int available, const char* apn) = 0;
    virtual void gnssNiResponse(uint32_t id, GnssNiResponse response) = 0;


    //flp
    virtual int flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id=1) = 0;
    virtual int flpStopSession(uint32_t id=1) = 0;
    virtual int flpGetBatchSize() = 0;
    virtual int flpUpdateSession(uint32_t sessionMode, LocationOptions& options, uint32_t id=1) = 0;
    virtual int flpGetBatchedLocation(int lastNLocations, uint32_t id=1) = 0;
    virtual void flpCleanUp() = 0;


    // geofence
    virtual void addGeofence(uint32_t geofenceId, double latitudeDegrees,
            double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs) = 0;
    virtual void removeGeofence(uint32_t geofenceId) = 0;
    virtual void modifyGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs) = 0;
    virtual void pauseGeofence(uint32_t geofenceId) = 0;
    virtual void resumeGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions) = 0;
    virtual void removeAllGeofences() = 0;
};

} // namespace garden
#endif //IGNSSAPI_H
