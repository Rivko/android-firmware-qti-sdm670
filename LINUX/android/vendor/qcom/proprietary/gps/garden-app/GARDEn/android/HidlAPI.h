/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef HIDLAPI_H
#define HIDLAPI_H

#include "IGnssAPI.h"
#include <LocationAPI.h>

namespace garden {

class HidlAPI : public IGnssAPI {
public:
    HidlAPI();
    virtual ~HidlAPI() = default;
    void setGnssCbs(GnssCbBase* callbacks) override;
    void setFlpCbs(GnssCbBase* callbacks) override;
    void setGeofenceCbs(GnssCbBase* callbacks) override;
    int gnssStart(void) override;
    int gnssStop(void) override;
    void gnssCleanup(void) override;
    int gnssInjectLocation(double latitude, double longitude, float accuracy) override;
    void gnssDeleteAidingData(LocGpsAidingData flags) override;
    int gnssSetPositionMode(LocGpsPositionMode mode,
            LocGpsPositionRecurrence recurrence, uint32_t min_interval,
            uint32_t preferred_accuracy, uint32_t preferred_time) override;
    void configurationUpdate(const char* config_data, int32_t length) override;
    void updateNetworkAvailability(int available, const char* apn) override;

    void gnssNiResponse(uint32_t id, GnssNiResponse response) override;

    //flp
    virtual int flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id=1) override;
    virtual int flpStopSession(uint32_t id=1) override;
    virtual int flpGetBatchSize() override;
    virtual int flpUpdateSession(uint32_t /*sessionMode*/, LocationOptions& /*options*/,
            uint32_t id=1) override;
    virtual int flpGetBatchedLocation(int /*lastNLocations*/, uint32_t id=1) override;
    virtual void flpCleanUp() override;

    // geofence
    virtual void addGeofence(uint32_t geofenceId, double latitudeDegrees, double longitudeDegrees,
            double radiusMeters, GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs) override;
    virtual void removeGeofence(uint32_t geofenceId) override;
    virtual void modifyGeofence(uint32_t /*geofenceId*/,
            GeofenceBreachTypeMask /*monitorTransitions*/,
            uint32_t /*notificationResponsivenessMs*/) override;
    virtual void pauseGeofence(uint32_t geofenceId) override;
    virtual void resumeGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask /*monitorTransitions*/) override;
    virtual void removeAllGeofences() override;

private:
    GnssCbBase* mGnssCallbacks;
    GnssCbBase* mFlpCbs;
    GnssCbBase* mGeofenceCbs;

};

} // namespace garden

#endif //GNSSLOCATION_H
