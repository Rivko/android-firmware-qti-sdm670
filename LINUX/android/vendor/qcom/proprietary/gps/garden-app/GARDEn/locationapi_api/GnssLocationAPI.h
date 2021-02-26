/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GNSSLOCATIONAPI_H
#define GNSSLOCATIONAPI_H

#include <gps_extended_c.h>
#include <LocationAPIClientBase.h>
#include <loc_gps.h>
#include "IGnssAPI.h"
#include "GnssLocationAPIClient.h"
#include "FlpLocationAPIClient.h"

namespace garden {

class GnssLocationAPI : public IGnssAPI {
private:
    GnssLocationAPIClient* mGnssAPI = nullptr;
    FlpLocationAPIClient* mFlpAPI = nullptr;
    GeofenceLocationAPIClient* mGeofenceAPI = nullptr;
public:
    GnssLocationAPI();
    virtual ~GnssLocationAPI();
    void setGnssCbs(GnssCbBase* callbacks) override;
    void setFlpCbs(GnssCbBase* callbacks) override;
    void setGeofenceCbs(GnssCbBase* callbacks) override;
    int gnssStart(void) override;
    int gnssStop(void) override;
    void gnssCleanup(void) override;
    int gnssInjectLocation(double latitude, double longitude,
            float accuracy) override;
    void gnssDeleteAidingData(LocGpsAidingData flags) override;
    int gnssSetPositionMode(LocGpsPositionMode mode,
            LocGpsPositionRecurrence recurrence, uint32_t min_interval,
            uint32_t preferred_accuracy, uint32_t preferred_time) override;
    void configurationUpdate(const char* config_data, int32_t length) override;
    void updateNetworkAvailability(int available, const char* apn) override;
    void gnssNiResponse(uint32_t id, GnssNiResponse response) override;

    //flp
    int flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id=1) override;
    int flpStopSession(uint32_t id=1) override;
    int flpGetBatchSize() override;
    int flpUpdateSession(uint32_t sessionMode, LocationOptions& options, uint32_t id=1) override;
    int flpGetBatchedLocation(int lastNLocations, uint32_t id=1) override;
    void flpCleanUp() override;

    // geofence
    virtual void addGeofence(uint32_t geofenceId, double latitudeDegrees,
            double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs) override;
    virtual void removeGeofence(uint32_t geofenceId) override;
    virtual void modifyGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs) override;
    virtual void pauseGeofence(uint32_t geofenceId) override;
    virtual void resumeGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions) override;
    virtual void removeAllGeofences() override;

    GnssLocationAPIClient* getGnssClient();
    FlpLocationAPIClient* getFlpClient();
    GeofenceLocationAPIClient* getGeofenceClient();
};

} // namespace garden

#endif //GNSSLOCATION_H
