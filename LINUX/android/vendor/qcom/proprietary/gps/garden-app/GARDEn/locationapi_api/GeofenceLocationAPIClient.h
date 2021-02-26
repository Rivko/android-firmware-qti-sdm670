/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GEOFENCELOCATIONAPICLIENT_H
#define GEOFENCELOCATIONAPICLIENT_H
#include "LocationAPIClientBase.h"
#include "GnssCbBase.h"

namespace garden {

class GeofenceLocationAPIClient: public LocationAPIClientBase
{
public:
    ~GeofenceLocationAPIClient() {}
    GeofenceLocationAPIClient() {}
    void setCallbacks(GnssCbBase* callbacks);

    void addGeofence(uint32_t geofenceId,
            double latitudeDegrees,
            double longitudeDegrees,
            double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs);
    void removeGeofence(uint32_t geofenceId);
    void modifyGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs);
    void pauseGeofence(uint32_t geofenceId);
    void resumeGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions);
    void removeAllGeofences();

    // callbacks
    void onGeofenceBreachCb(GeofenceBreachNotification geofenceBreachNotification) final;
    void onGeofenceStatusCb(GeofenceStatusNotification geofenceStatusNotification) final;
    void onAddGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onRemoveGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onPauseGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onResumeGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onModifyGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;

private:
    mutex mGeofenceLock;
    GnssCbBase* mGeofenceCbs;
};

}// namespace garden

#endif //GEOFENCELOCATIONAPICLIENT_H
