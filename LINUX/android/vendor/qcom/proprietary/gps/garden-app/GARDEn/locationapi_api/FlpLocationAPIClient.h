/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef FLPLOCATIONAPICLIENT_H
#define FLPLOCATIONAPICLIENT_H
#include "LocationAPIClientBase.h"
#include "GnssCbBase.h"

namespace garden {

class FlpLocationAPIClient: public LocationAPIClientBase
{
public:
    virtual ~FlpLocationAPIClient() {}
    FlpLocationAPIClient();
    void setCallbacks(GnssCbBase* callbacks);
    int flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id=1);
    int flpStopSession(uint32_t id=1);
    int getBatchSize();
    int updateSession(uint32_t sessionMode, LocationOptions& options, uint32_t id=1);
    int getBatchedLocation(int lastNLocations, uint32_t id=1);

    //callbacks
    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onTrackingCb(Location location) final;
    void onStartTrackingCb(LocationError error) final;
    void onStopTrackingCb(LocationError error) final;
    void onUpdateTrackingOptionsCb(LocationError error) final;

    void onBatchingCb(size_t count, Location* location, BatchingOptions batchingOptions) final;
    void onStartBatchingCb(LocationError error) final;
    void onStopBatchingCb(LocationError error) final;
    void onUpdateBatchingOptionsCb(LocationError error) final;
    void onGetBatchedLocationsCb(LocationError error) final;

private:
    mutex mFlpLock;
    GnssCbBase* mFlpCbs;
    LocationCapabilitiesMask mLocationCapabilitiesMask;
};

}// namespace garden

#endif //FLPLOCATIONAPICLIENT_H
