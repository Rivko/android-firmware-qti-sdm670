/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef LOCATIONCLIENTAPI_CASE_H
#define LOCATIONCLIENTAPI_CASE_H

#include "GardenFramework.h"
#include "LocationClientApi.h"
#include "LocationClientApiImpl.h"

using namespace location_client;

namespace garden {

class LocationClientApiCase : public IGardenCase{
public:
    LocationClientApiCase();
    virtual ~LocationClientApiCase();
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();
    GARDEN_RESULT menuCase();
    GARDEN_RESULT autoStartCases(uint32_t interval, uint32_t loop, uint32_t option);

private:
    LocationClientApi* mPClient_1;
    LocationClientApi* mPClient_2;
    LocationClientApiImpl* mPClientImpl;
    vector<Geofence> mGeofences;
    void testDeleteAll();
    GARDEN_RESULT testGeofence(int clientId);
    void testGetEnergy();
    void testRegSystemInfo();
    void testUnregSystemInfo();
    void menuAddGeofence(int clientId);
    void menuModifyGeofence(int clientId);
    void menuPauseGeofence(int clientId);
    void menuResumeGeofence(int clientId);
    void menuRemoveGeofence(int clientId);
};

} // namespace garden
#endif // LOCATIONCLIENTAPI_CASE_H
