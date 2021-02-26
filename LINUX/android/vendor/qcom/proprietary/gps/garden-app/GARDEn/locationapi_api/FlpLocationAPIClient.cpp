/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "FlpLocationAPIClient.h"
#include <loc_pla.h>
#include <log_util.h>
#include "GardenUtil.h"
#include "GnssCase.h"

namespace garden {

FlpLocationAPIClient::FlpLocationAPIClient():
        LocationAPIClientBase(), mFlpCbs(nullptr),
        mLocationCapabilitiesMask(0) {}

void FlpLocationAPIClient::setCallbacks(GnssCbBase* callbacks) {
    {
        lock_guard guard(mFlpLock);
        mFlpCbs = callbacks;
    }

    LocationCallbacks locationCallbacks;
    locationCallbacks.size = sizeof(LocationCallbacks);
    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb =  nullptr;
    locationCallbacks.gnssDataCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;
    if(callbacks != nullptr) {
        locationCallbacks.trackingCb = [this](Location location) {
            onTrackingCb(location);
        };
        locationCallbacks.batchingCb = [this](size_t count, Location* location,
                BatchingOptions batchingOptions) {
            onBatchingCb(count, location, batchingOptions);
        };
    }
    locAPISetCallbacks(locationCallbacks);
}

int FlpLocationAPIClient::flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id) {
    int retVal = -1;
    LOC_LOGD("%s]: (%d %d)", __FUNCTION__, options.minInterval, mode);
    gardenPrint("%s]: (minInterval: %d, mode: %d, id: %d)", __func__,
            options.minInterval, mode, id);
    options.mode = GNSS_SUPL_MODE_STANDALONE;
    if (mLocationCapabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSA_BIT) {
        options.mode = GNSS_SUPL_MODE_MSA;
    }
    if (mLocationCapabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT) {
        options.mode = GNSS_SUPL_MODE_MSB;
    }
    if(locAPIStartSession(id, mode, options) == LOCATION_ERROR_SUCCESS) {
        retVal = 0;
    }
    return retVal;
}

int FlpLocationAPIClient::flpStopSession(uint32_t id) {
    LOC_LOGD("%s]: ", __FUNCTION__);
    int retVal = -1;
    if(locAPIStopSession(id) == LOCATION_ERROR_SUCCESS) {
        retVal = 0;
    }
    return retVal;
}

int FlpLocationAPIClient::getBatchSize() {
    LOC_LOGD("%s]: ()", __FUNCTION__);
    return locAPIGetBatchSize();
}

int FlpLocationAPIClient::updateSession(uint32_t sessionMode, LocationOptions& options,
                                        uint32_t id) {
    LOC_LOGD("%s]: (%u %ld %d)", __FUNCTION__,
            id, options.minInterval*1000000L, sessionMode);
    int retVal = -1;
    if(locAPIUpdateSessionOptions(id, sessionMode, options) ==
            LOCATION_ERROR_SUCCESS) {
        retVal = 0;
    }
    return retVal;
}

int FlpLocationAPIClient::getBatchedLocation(int lastNLocations, uint32_t id) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, lastNLocations);
    int retVal = -1;
    if (locAPIGetBatchedLocations(id, lastNLocations) == LOCATION_ERROR_SUCCESS) {
        retVal = 0;
    }
    return retVal;
}

void FlpLocationAPIClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, capabilitiesMask);
    mLocationCapabilitiesMask = capabilitiesMask;
}

void FlpLocationAPIClient::onTrackingCb(Location location) {
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, location.flags);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->gnssLocationCb(location);
    }
}

void FlpLocationAPIClient::onStartTrackingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->startTrackingCb(error);
    }
}

void FlpLocationAPIClient::onStopTrackingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->stopTrackingCb(error);
    }
}

void FlpLocationAPIClient::onUpdateTrackingOptionsCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->updateTrackingCb(error);
    }
}

void FlpLocationAPIClient::onBatchingCb(size_t count, Location* location,
        BatchingOptions /*batchingOptions*/) {
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, count);
    lock_guard guard(mFlpLock);
    if(mFlpCbs != nullptr) {
        mFlpCbs->batchingCb(count, location);
    }
}

void FlpLocationAPIClient::onStartBatchingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->startBatchingCb(error);
    }
}

void FlpLocationAPIClient::onStopBatchingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->stopBatchingCb(error);
    }
}

void FlpLocationAPIClient::onUpdateBatchingOptionsCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->updateBatchingCb(error);
    }
}

void FlpLocationAPIClient::onGetBatchedLocationsCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mFlpLock);
    if (mFlpCbs != nullptr) {
        mFlpCbs->getBatchedLocationCb(error);
    }
}


} // namespace garden
