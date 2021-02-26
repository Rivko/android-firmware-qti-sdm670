/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTICLIENT_LOCATIONAPICLIENT_H
#define GNSS_MULTICLIENT_LOCATIONAPICLIENT_H

#include <LocationAPIClientBase.h>
#include <loc_gps.h>
#include <location_interface.h>
#include <LocationAPI.h>
#include "GnssCbBase.h"

namespace garden {

class GnssMultiClientLocationAPIClient: public LocationAPIClientBase {
public:
    GnssMultiClientLocationAPIClient();
    ~GnssMultiClientLocationAPIClient();
    int createControlClient();
    int setCallbacks(GnssCbBase* callbacks);

    int gnssStart(void);
    int gnssInjectLocation(double latitude, double longitude, float accuracy);
    void gnssDeleteAidingData(LocGpsAidingData flags);
    int gnssSetPositionMode(LocGpsPositionMode mode, LocGpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time);

    void configurationUpdate(const char* config_data, int32_t length);
    void updateNetworkAvailability(int available, const char* apn);

    //callbacks
    void onTrackingCb(Location location) final;
    void onStartTrackingCb(LocationError error) final;
    void onStopTrackingCb(LocationError error) final;
    void onGnssSvCb(GnssSvNotification gnssSvNotification) final;
    void onGnssNmeaCb(GnssNmeaNotification gnssNmeaNotification) final;
    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification) final;

    LocationAPIControlClient* mControlClient;
private:
    GnssInterface* getGnssInterface();
private:
    mutex mGnssLock;
    LocationOptions mTrackingOptions;
    GnssInterface* mGnssInterface;
    LocationCapabilitiesMask mLocationCapabilitiesMask;
    GnssCbBase* mGnssCallbacks;
};

} // namespace garden

#endif //GNSS_MULTICLIENT_LOCATIONAPICLIENT_H

