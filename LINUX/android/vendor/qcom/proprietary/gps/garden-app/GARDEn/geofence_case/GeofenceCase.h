/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GEOFENCE_CASE_H
#define GEOFENCE_CASE_H

#include "GnssCase.h"

namespace garden {

#define GEOFENCE_CASE_BASE GNSS_CASE_MAX
typedef enum {
    GEOFENCE_CASE_ADD       = (GEOFENCE_CASE_BASE << 0),
    GEOFENCE_CASE_REMOVE    = (GEOFENCE_CASE_BASE << 1),
    GEOFENCE_CASE_MODIFY    = (GEOFENCE_CASE_BASE << 2),
    GEOFENCE_CASE_PAUSE     = (GEOFENCE_CASE_BASE << 3),
    GEOFENCE_CASE_RESUME    = (GEOFENCE_CASE_BASE << 4),
    GEOFENCE_CASE_MAX       = (GEOFENCE_CASE_BASE << 5),
} GEOFENCE_CASE_FLAG;

class GeofenceCase : public GnssCase {
public:
    GeofenceCase();
    virtual ~GeofenceCase();
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();
    GARDEN_RESULT menuCase();
    GARDEN_RESULT stressRandomCase();
    GARDEN_RESULT stressAddRemoveCase();
    GARDEN_RESULT loadFromFileCase(std::string file);
private:
    void menuAddGeofence();
    void menuModifyGeofence();
    void menuPauseGeofence();
    void menuResumeGeofence();
    void menuRemoveGeofence();

    void randomAddGeofence();
    void randomModifyGeofence();
    void randomPauseGeofence();
    void randomResumeGeofence();
    void randomRemoveGeofence();
    void dumpGeofence();
    void clearGeofence();

    void syncAddGeofence(uint32_t geofenceId, double latitudeDegrees,
            double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs);
    void syncModifyGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs);
    void syncPauseGeofence(uint32_t geofenceId);
    void syncResumeGeofence(uint32_t geofenceId, GeofenceBreachTypeMask monitor_transitions);
    void syncRemoveGeofence(uint32_t geofenceId);
};

} // namespace garden
#endif // GEOFENCE_CASE_H
