/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "FlpCb.h"
#include "GardenUtil.h"

namespace garden {
    FlpCb::FlpCb(GnssCase* gCase): mFlpCase(gCase) {}

    void FlpCb::batchingCb(size_t num_locations, Location* location) {
        gardenPrint("## %s]: () num_locations = %d", __FUNCTION__, num_locations);
        if(num_locations <= 0) {
            return;
        }
        gardenPrint(" flags | latitude | longitude | altitude | accuracy | speed | bearing |\
                time | sources\n");
        for (size_t i = 0; i < num_locations; ++i) {
            gardenPrint("%6d | %8.4f | %9.4f | %8.2f | %8.2f | %5.2f | %7.2f | %13lld | %d\n",
                    location[i].flags, location[i].latitude, location[i].longitude,
                    location[i].altitude, location[i].accuracy, location[i].speed,
                    location[i].bearing, location[i].timestamp, location[i].techMask);
        }
    }

    void FlpCb::startBatchingCb(LocationError error) {
        gardenPrint("EVENT FLP BATCHING START status=%d\n", error);
    }

    void FlpCb::stopBatchingCb(LocationError error) {
        gardenPrint("EVENT FLP BATCHING STOP status=%d\n", error);
    }

    void FlpCb::updateBatchingCb(LocationError error) {
        gardenPrint("EVENT FLP BATCHING OPTIONS UPDATE status=%d\n", error);
    }

    void FlpCb::getBatchedLocationCb(LocationError error) {
        gardenPrint("EVENT FLP GET BATCHED LOCATION status=%d\n", error);
    }

    void FlpCb::startTrackingCb(LocationError error) {
        gardenPrint("EVENT FLP TRACKING START status=%d\n", error);
    }

    void FlpCb::stopTrackingCb(LocationError error) {
        gardenPrint("EVENT FLP TRACKING STOP status=%d\n", error);
    }
    void FlpCb::updateTrackingCb(LocationError error) {
        gardenPrint("EVENT FLP TRACKING OPTIONS UPDATE status=%d\n", error);
    }
}
