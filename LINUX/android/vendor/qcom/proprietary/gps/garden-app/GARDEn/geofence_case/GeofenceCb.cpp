/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GeofenceCb.h"
#include "GardenUtil.h"

namespace garden {

GeofenceCb::GeofenceCb(GnssCase* gCase): mGeofenceCase(gCase) {}

void GeofenceCb::geofenceBreachCb(uint32_t id, Location location,
        GeofenceBreachType transition, uint64_t timeStamp) {
    gardenPrint("EVENT GPS GEOFENCE TRANSITION id=%d transition=%d lat=%-8.2f "
            "long=%-8.2f accuracy=%-8.2f speed=%-8.2f bearing=%-8.2f time=%lld\n",
            id, transition, location.latitude, location.longitude,
            location.accuracy, location.speed, location.bearing, timeStamp);
}

void GeofenceCb::geofenceStatusCb(GeofenceStatusAvailable status) {
    gardenPrint("EVENT GPS GEOFENCE STATUS status=%d\n", status);
}

void GeofenceCb::addGeofencesCb(uint32_t id, LocationError error) {
    gardenPrint("EVENT GPS GEOFENCE ADD geofence_id=%d status=%d\n", id, error);
}

void GeofenceCb::removeGeofencesCb(uint32_t id, LocationError error) {
    gardenPrint("EVENT GPS GEOFENCE REMOVE geofence_id=%d status=%d\n", id, error);
}

void GeofenceCb::pauseGeofencesCb(uint32_t id, LocationError error) {
    gardenPrint("EVENT GPS GEOFENCE PAUSE geofence_id=%d status=%d\n", id, error);
}

void GeofenceCb::resumeGeofencesCb(uint32_t id, LocationError error) {
    gardenPrint("EVENT GPS GEOFENCE RESUME geofence_id=%d status=%d\n", id, error);
}

void GeofenceCb::modifyGeofencesCb(uint32_t id, LocationError error) {
    gardenPrint("EVENT GPS GEOFENCE MODIFY geofence_id=%d status=%d\n", id, error);
}

} //namespace garden
